/*******************************************************************************
 * File        : pihm.c                                                        *
 * Version     : Nov, 2007 (2.0)                                               *
 * Developer of PIHM2.0:        Mukesh Kumar (muk139@psu.edu)                  *
 * Developer of PIHM1.0:        Yizhong Qu   (quyizhong@gmail.com)             *
 *-----------------------------------------------------------------------------*
 *                                                                             *
 *..............MODIFICATIONS/ADDITIONS in PIHM 2.0............................*
 * a) All modifications in physical process representations  in this version   *
 *    are listed as header in f.c and is_sm_et.c.                  *
 * b) All addition/modifications in variable and structure definition/declarat-*
 *    -ion are listed as header in read_alloc.c and initialize.c	       *
 * c) 3 new input files have been added for geology, landcover and calibration *
 *    data								       *
 * d) Ported to Sundials 2.1.0                                                 *
 *-----------------------------------------------------------------------------*
 *                                                                             *
 * PIHM is an integrated finite volume based hydrologic model. It simulates    *
 * channel routing, overland flow, groundwater flow, macropore based infiltra- *
 * tion and stormflow, throughfall, evaporation from overlandflow-subsurface-  *
 * canopy, transpiration and  snowmelt by full coupling of processes.          *
 * It uses semi-discrete finite volume approach to discretize PDEs into ODEs,  *
 * and henceforth solving the global system of ODEs using CVODE. Global ODEs   *
 * are created in f.c. Any modifications in the process equations has to be    *
 * performed in f.c
 *                                                                             *
 *-----------------------------------------------------------------------------*
 * For questions or comments, please contact                                   *
 *      --> Mukesh Kumar (muk139@psu.edu)                                      *
 *      --> Prof. Chris Duffy (cxd11@psu.edu)                                  *
 * This code is free for research purpose only.                                *
 * Please provide relevant references if you use this code in your research work*
 *-----------------------------------------------------------------------------*
 *									       *
 * DEVELOPMENT RELATED REFERENCES:					       *
 * PIHM2.0:								       *
 *	a) Kumar, M., 2008, "Development and Implementation of a Multiscale,   *
 *	Multiprocess Hydrologic Model". PhD Thesis, Penn State University      *
 *	b) Kumar, M, G.Bhatt & C.Duffy, "Coupling of Data and Processes in     *
 *	Mesoscale Watershed", Advances in Water Resources (submitted)          *
 * PIHM1.0:								       *
 *	a) Qu, Y., 2005, "An Integrated hydrologic model for multiproces       *
 *	simulation using semi-discrete finite volume approach".PhD Thesis, PSU *
 *	b) Qu, Y. & C. Duffy, 2007, "A semidiscrete finite volume formulation  *
 *	for multiprocess watershed simulation". Water Resources Research       *
 *-----------------------------------------------------------------------------*
 * LICENSE:
 *******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/* SUNDIAL Header Files */
#include <sundials/sundials_types.h>   /* realtype, integertype, booleantype defination */
#include <cvode/cvode.h>           /* CVODE header file                             */
//#include <sundials/sundials_spgmr.h>         /* CVSPGMR linear header file                    */
#include <cvode/cvode_spgmr.h>
//#include "smalldense.h"      /* use generic DENSE linear solver for "small"   */
#include <nvector/nvector_serial.h>  /* contains the definition of type N_Vector      */
#include <sundials/sundials_math.h>    /* contains UnitRoundoff, RSqrt, SQR functions   */
#include <sundials/sundials_dense.h>         /* CVDENSE header file                           */
//#include "dense.h"           /* generic dense solver header file              */
#include "pihm.h"            /* Data Model and Variable Declarations     */
#define UNIT_C 1440      /* Unit Conversions */
//#include <windows.h>
#include <signal.h>

/* Function Declarations */
void initialize(char *, Model_Data, Control_Data *, N_Vector);
void is_sm_et(realtype, realtype, Model_Data, N_Vector);
/* Function to calculate right hand side of ODE systems */
int f(realtype, N_Vector, N_Vector, void *);
void read_alloc(char *, Model_Data, Control_Data *);  /* Variable definition */
void update(realtype, Model_Data);
void PrintData(FILE **,Control_Data *, Model_Data, N_Vector, realtype);

void abrt_handler( int i)
{
  printf ( "sig_abrt_hand\n");
  exit ( 1);
}

void segv_handler( int i)
{
  printf ( "segv_hand\n");
  exit ( 1);
}

/* Main Function */
int main(int argc, char *argv[])
{
  char tmpLName[11],tmpFName[11]; /* rivFlux File names */
  struct model_data_structure mData = {0};                 /* Model Data                */
  Control_Data cData = {0};               /* Solver Control Data       */
  N_Vector CV_Y;                    /* State Variables Vector    */
  void *cvode_mem = NULL;                  /* Model Data Pointer        */
  int flag;                         /* flag to test return value */
  FILE *Ofile[22] = {NULL};               /* Output file     */
  FILE *iproj = NULL;     /* Project File */
  int N;                            /* Problem size              */
  int i,j,k;                        /* loop index                */
  realtype t;           /* simulation time           */
  realtype NextPtr, StepSize;       /* stress period & step size */
  clock_t start, end_r, end_s;      /* system clock at points    */
  realtype cputime_r, cputime_s;    /* for duration in realtype  */
  char *filename = NULL;
  char buf[1024];

  signal(SIGABRT, &abrt_handler);
  signal(SIGSEGV, &segv_handler);

  /* Project Input Name */
  if(argc!=2)
  {
    iproj=fopen("projectName.txt","r");
    if(iproj==NULL)
    {
      printf("\t\nUsage ./pihm project_name");
      printf("\t\n         OR              ");
      printf("\t\nUsage ./pihm, and have a file in the current directory named projectName.txt with the project name in it");
      exit(0);
    }
    else
    {
      assert(filename = (char *)malloc(15*sizeof(char)));
      fscanf(iproj,"%s",filename);
    }
  }
  else
  {
    /* get user specified file name in command line */
    assert(filename = (char *)malloc(strlen(argv[1])*sizeof(char)));
    strcpy(filename,argv[1]);
  }
  /* Open Output Files */
  sprintf(buf,"%s.GW", filename);
  Ofile[0]=fopen(buf, "w");
  sprintf(buf,"%s.surf", filename);
  Ofile[1]=fopen(buf,"w");
  sprintf(buf, "%s.et0", filename);
  Ofile[2] = fopen(buf, "w");
  sprintf(buf, "%s.et1", filename);
  Ofile[3]=fopen(buf, "w");
  sprintf(buf, "%s.et2", filename);
  Ofile[4]=fopen(buf, "w");
  sprintf(buf, "%s.is", filename);
  Ofile[5]=fopen(buf, "w");
  sprintf(buf, "%s.snow", filename);
  Ofile[6]=fopen(buf, "w");
  for(i=0; i<11; i++)
  {
    sprintf(buf, "%s.rivFlx%d", filename, i);
    Ofile[7+i]=fopen(buf, "w");
  }
  sprintf(buf, "%s.stage", filename);
  Ofile[18]=fopen(buf, "w");
  sprintf(buf, "%s.unsat", filename);
  Ofile[19]=fopen(buf, "w");
  sprintf(buf, "%s.Rech", filename);
  Ofile[20]=fopen(buf, "w");

  /* allocate memory for model data structure */
//    assert(mData = (Model_Data)malloc(sizeof *mData));

  printf("\n ...  PIHM 2.0 is starting ... \n");

  /* read in 9 input files with "filename" as prefix */
  read_alloc(filename, &mData, &cData);

/*	if(mData->UnsatMode ==1)
    {
      } */
  if(mData.UnsatMode ==2)
  {
    printf("after read_alloc\n");
    printf("UnsatMode=%d\n",mData.UnsatMode);
    /* problem size */
    N = 3*mData.NumEle + 2*mData.NumRiv;
    printf("N=%d\n",N);
    printf("Trying to allocate %d bytes\n", (3*mData.NumEle+2*mData.NumRiv)*sizeof(realtype));
    assert(mData.DummyY=(realtype *)malloc((3*mData.NumEle+2*mData.NumRiv)*sizeof(realtype)));
    printf("allocated\n");
  }
  /* initial state variable depending on machine*/
  printf("We want %d variables\n", N);
//	  printf("Current process id is %#x\n", GetCurrentProcessId());
//	  while(1){printf("wait for debugger\n"); Sleep(5000);};
  CV_Y = N_VNew_Serial(N);
  printf("Hello\n");

  /* initialize mode data structure */
  initialize(filename, &mData, &cData, CV_Y);

  printf("\nSolving ODE system ... \n");

  /* allocate memory for solver */
  cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
  if(cvode_mem == NULL) {printf("CVodeMalloc failed. \n"); return(1);}

#ifdef SUNDIALS_240
  // as of CVODE 2.6 and sundials 2.4.0 some names changed // mlt
  flag = CVodeInit(cvode_mem, f, cData.StartTime, CV_Y);  //, CV_SS, );
  assert(CV_SUCCESS == CVodeSStolerances(cvode_mem, cData.reltol, cData.abstol));
  flag = CVodeSetUserData(cvode_mem, &mData);
  // set initial time step, otherwise it is estimated
  flag = CVodeSetInitStep(cvode_mem, cData.InitStep);
  flag = CVodeSetStabLimDet(cvode_mem,TRUE);
  flag = CVodeSetMaxStep(cvode_mem,cData.MaxStep);
  // Call CVSpgmr to specify the linear solver CVSPGMR
  // without preconditioning and the maximum Krylov dimension maxl
  flag = CVSpgmr(cvode_mem, PREC_NONE, 0);
#else
  flag = CVodeSetFdata(cvode_mem, &mData);
  flag = CVodeSetInitStep(cvode_mem,cData.InitStep);
  flag = CVodeSetStabLimDet(cvode_mem,TRUE);
  flag = CVodeSetMaxStep(cvode_mem,cData.MaxStep);
  flag = CVodeMalloc(cvode_mem, f, cData.StartTime, CV_Y, CV_SS, cData.reltol, &cData.abstol);
  flag = CVSpgmr(cvode_mem, PREC_NONE, 0);
//    flag = CVSpgmrSetGSType(cvode_mem, MODIFIED_GS);
#endif
  // Set modified Gram-Schmidt orthogonalization
  flag = CVSpilsSetGSType(cvode_mem, MODIFIED_GS);

  /* set start time */
  t = cData.StartTime;
  start = clock();

  /* start solver in loops */
  for(i=0; i<cData.NumSteps; i++)
  {
    /*	if (cData.Verbose != 1)
            {
              printf("  Running: %-4.1f%% ... ", (100*(i+1)/((realtype) cData.NumSteps)));
              fflush(stdout);
            } */
    /* inner loops to next output points with ET step size control */
    while(t < cData.Tout[i+1])
    {
      if (t + cData.ETStep >= cData.Tout[i+1])
      {
        NextPtr = cData.Tout[i+1];
      }
      else
      {
        NextPtr = t + cData.ETStep;
      }
      StepSize = NextPtr - t;

      /* calculate Interception Storage */
      is_sm_et(t, StepSize, &mData,CV_Y);
      printf("\n Tsteps = %f ",t);
      flag = CVode(cvode_mem, NextPtr, CV_Y, &t, CV_NORMAL);
      update(t,&mData);
    }
    PrintData(Ofile,&cData,&mData, CV_Y,t);
  }
  /* Free memory */
  N_VDestroy_Serial(CV_Y);
  /* Free integrator memory */
  CVodeFree(cvode_mem);
//    free(mData);
  return 0;
}

