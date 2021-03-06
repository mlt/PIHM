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
#include <cvode/cvode_spgmr.h>         /* CVSPGMR linear header file                    */
#include <nvector/nvector_serial.h>  /* contains the definition of type N_Vector      */
#include <sundials/sundials_math.h>    /* contains UnitRoundoff, RSqrt, SQR functions   */
#include <sundials/sundials_dense.h>           /* generic dense solver header file              */
#include "pihm.h"            /* Data Model and Variable Declarations     */
#include "is_sm_et.h"
#include "update.h"
#include <iostream>
#include <fstream>
#include <assert.h>

#define UNIT_C 1440      /* Unit Conversions */


using namespace std;

/* Function Declarations */
int initialize(char *, Model_Data, Control_Data *, N_Vector);
/* Function to calculate right hand side of ODE systems */
int f(realtype, N_Vector, N_Vector, void *);
void read_alloc(char *, Model_Data, Control_Data *);  /* Variable definition */

void PrintData(FILE **,Control_Data *, Model_Data, N_Vector, realtype);

/* Main Function */
int main(int argc, char **argv) {
  char logFileName[] = "log.html";
  struct model_data_structure mData = {0};                 /* Model Data                */
  Control_Data cData = {0};               /* Solver Control Data       */
  N_Vector CV_Y = NULL;                    /* State Variables Vector    */
  void *cvode_mem = NULL;                  /* Model Data Pointer        */
  int flag;                         /* flag to test return value */
  FILE *Ofile[22] = {NULL};               /* Output file     */
  FILE *iproj = NULL;     /* Project File */
  int N = 0;                            /* Problem size              */
  int i,j;  //,k;                      /* loop index                */
  realtype t;           /* simulation time           */
  realtype NextPtr, StepSize;       /* stress period & step size */
//    clock_t start, end_r, end_s;    /* system clock at points    */
//    realtype cputime_r, cputime_s;  /* for duration in realtype  */
  char filename[MAX_PATH];
  char buf[MAX_PATH];

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
      int read = fscanf(iproj,"%s",filename);
      assert(1 == read);
    }
  }
  else
  {
    /* get user specified file name in command line */
    strncpy(filename,argv[1], MAX_PATH);
  }
  /* Open Output Files */
  sprintf(buf,"%s.GW.txt", filename);
  cout<<buf<<endl;
  Ofile[0]=fopen(buf, "w");
  sprintf(buf,"%s.surf.txt", filename);
  cout<<buf<<endl;
  Ofile[1]=fopen(buf,"w");
  sprintf(buf, "%s.et0.txt", filename);
  cout<<buf<<endl;
  Ofile[2] = fopen(buf, "w");
  sprintf(buf, "%s.et1.txt", filename);
  cout<<buf<<endl;
  Ofile[3]=fopen(buf, "w");
  sprintf(buf, "%s.et2.txt", filename);
  cout<<buf<<endl;
  Ofile[4]=fopen(buf, "w");
  sprintf(buf, "%s.is.txt", filename);
  cout<<buf<<endl;
  Ofile[5]=fopen(buf, "w");
  sprintf(buf, "%s.snow.txt", filename);
  cout<<buf<<endl;
  Ofile[6]=fopen(buf, "w");


  for(i=0; i<11; i++)
  {
    sprintf(buf, "%s.rivFlx%d.txt", filename, i);
    cout<<buf<<endl;
    Ofile[7+i]=fopen(buf, "w");
  }
  //cout<<ofn[17]<<"\n";
  sprintf(buf, "%s.stage.txt", filename);
  Ofile[18]=fopen(buf, "w");
  sprintf(buf, "%s.unsat.txt", filename);
  Ofile[19]=fopen(buf, "w");
  sprintf(buf, "%s.Rech.txt", filename);
  Ofile[20]=fopen(buf, "w");


  printf("\n ...  PIHM 2.0 is starting ... \n");

  /* read in 9 input files with "filename" as prefix */
  read_alloc(filename, &mData, &cData);

/*	if(mData->UnsatMode ==1)
    {
      } */
  if(mData.UnsatMode ==2)
  {
    /* problem size */
    N = 3*mData.NumEle + 2*mData.NumRiv;
    mData.DummyY=(realtype *)malloc((3*mData.NumEle+2*mData.NumRiv)*sizeof(realtype));
    memset(mData.DummyY, 0, N*sizeof(realtype));
  }
  /* initial state variable depending on machine*/
  CV_Y = N_VNew_Serial(N);

  /* initialize mode data structure */
  int BoolR = initialize(filename, &mData, &cData, CV_Y);
  ofstream log;
  if(cData.FillEleSurf==1) {
    log.open(logFileName, ios::app);
    log<<"Filling Surface Sink Elements...<br>";
    log.close();
  }
  if(cData.FillEleSub==1) {
    log.open(logFileName, ios::app);
    log<<"Filling Subsurface Sink Elements...<br>";
    log.close();
  }
  if(BoolR==1) {
    log.open(logFileName, ios::app);
    log<<"WARNING: River Elevation Correction may be needed...<br>";
    log.close();
  }

  printf("\nSolving ODE system ... \n");

  /* allocate memory for solver */
  cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
  if(cvode_mem == NULL) {printf("CVodeMalloc failed. \n"); return(1); }

#ifdef SUNDIALS_240
  // as of CVODE 2.6 and sundials 2.4.0 some names changed // mlt
  flag = CVodeInit(cvode_mem, f, cData.StartTime, CV_Y);  //, CV_SS, );
  flag = CVodeSStolerances(cvode_mem, cData.reltol, cData.abstol);
  assert(CV_SUCCESS == flag);
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
#endif
  // Set modified Gram-Schmidt orthogonalization
  flag = CVSpilsSetGSType(cvode_mem, MODIFIED_GS);

  /* set start time */
  t = cData.StartTime;
//    start = clock();

  bool error = false;
  /* start solver in loops */
  for(i=0; i<cData.NumSteps && !error; i++)
  {
    /* inner loops to next output points with ET step size control */
    while(t < cData.Tout[i+1] && !error)
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
//				assert(CV_ROOT_RETURN == flag);
      if (CV_RHSFUNC_FAIL == flag)
        error = true;
      update(t,&mData);
    }
    PrintData(Ofile,&cData,&mData, CV_Y,t);
  }
  /* Free memory */
  N_VDestroy_Serial(CV_Y);
  /* Free integrator memory */
  CVodeFree(&cvode_mem);

  free(mData.Riv_IC);
  for(i=0; i<mData.NumP; i++) {
    for(j=0; j<mData.TSD_Pressure[i].length; j++)
      free(mData.TSD_Pressure[i].TS[j]);
    free(mData.TSD_Pressure[i].TS);
  }
  free(mData.TSD_Pressure);
  for(i=0; i<mData.NumLC; i++) {
    for(j=0; j<mData.TSD_LAI[i].length; j++)
      free(mData.TSD_LAI[i].TS[j]);
    free(mData.TSD_LAI[i].TS);
    for(j=0; j<mData.TSD_RL[i].length; j++)
      free(mData.TSD_RL[i].TS[j]);
    free(mData.TSD_RL[i].TS);
  }
  free(mData.TSD_LAI);
  for(i=0; i<mData.NumRivBC; i++) {
    for(j=0; j<mData.TSD_Riv[i].length; j++)
      free(mData.TSD_Riv[i].TS[j]);
    free(mData.TSD_Riv[i].TS);
  }
  free(mData.TSD_Riv);
  for(i=0; i<mData.NumPrep; i++) {
    for(j=0; j<mData.TSD_Prep[i].length; j++)
      free(mData.TSD_Prep[i].TS[j]);
    free(mData.TSD_Prep[i].TS);
  }
  free(mData.TSD_Prep);
  for(i=0; i<mData.NumTemp; i++) {
    for(j=0; j<mData.TSD_Temp[i].length; j++)
      free(mData.TSD_Temp[i].TS[j]);
    free(mData.TSD_Temp[i].TS);
  }
  free(mData.TSD_Temp);
  for(i=0; i<mData.NumWindVel; i++) {
    for(j=0; j<mData.TSD_WindVel[i].length; j++)
      free(mData.TSD_WindVel[i].TS[j]);
    free(mData.TSD_WindVel[i].TS);
  }
  free(mData.TSD_WindVel);
  for(i=0; i<mData.NumRn; i++) {
    for(j=0; j<mData.TSD_Rn[i].length; j++)
      free(mData.TSD_Rn[i].TS[j]);
    free(mData.TSD_Rn[i].TS);
  }
  free(mData.TSD_Rn);
  for(i=0; i<mData.NumG; i++) {
    for(j=0; j<mData.TSD_G[i].length; j++)
      free(mData.TSD_G[i].TS[j]);
    free(mData.TSD_G[i].TS);
  }
  free(mData.TSD_G);
  free(mData.TSD_RL);
  for(i=0; i<mData.NumSource; i++) {
    for(j=0; j<mData.TSD_Source[i].length; j++)
      free(mData.TSD_Source[i].TS[j]);
    free(mData.TSD_Source[i].TS);
  }
  free(mData.TSD_Source);
  free(mData.ISFactor);
  free(mData.windH);
  free(mData.Riv_Mat);
  free(mData.Riv_Shape);
  free(mData.Riv);
  for(i=0; i<mData.NumMeltF; i++) {
    for(j=0; j<mData.TSD_MeltF[i].length; j++)
      free(mData.TSD_MeltF[i].TS[j]);
    free(mData.TSD_MeltF[i].TS);
  }
  free(mData.TSD_MeltF);
  free(mData.Soil);
  free(mData.Geol);
  free(mData.LandC);

  for(i=0; i<mData.NumRiv; i++)
    free(mData.FluxRiv[i]);
  free(mData.FluxRiv);
  free(mData.ElePrep);
  free(mData.EleViR);
  free(mData.Recharge);
  free(mData.EleIS);
  free(mData.EleISmax);
  free(mData.EleISsnowmax);
  free(mData.EleSnow);
  free(mData.EleSnowGrnd);
  free(mData.EleSnowCanopy);
  free(mData.EleTF);
  free(mData.EleETloss);
  free(mData.EleNetPrep);
  for(i=0; i<mData.NumEle; i++) {
    free(mData.FluxSurf[i]);
    free(mData.FluxSub[i]);
    free(mData.EleET[i]);
  }
  free(mData.FluxSurf);
  free(mData.FluxSub);
  free(mData.EleET);
  free(mData.Ele);
  free(mData.Node);
  for(i=0; i<22; i++)
    free(mData.PrintVar[i]);
  for(i=0; i<mData.NumHumidity; i++) {
    for(j=0; j<mData.TSD_Humidity[i].length; j++)
      free(mData.TSD_Humidity[i].TS[j]);
    free(mData.TSD_Humidity[i].TS);
  }
  free(mData.TSD_Humidity);
  free(mData.Ele_IC);
  if(mData.Num1BC+mData.Num2BC > 0)
    free(mData.TSD_EleBC);
  if(mData.Num1BC>0) {
    for(i=0; i<mData.Num1BC; i++) {
      for(j=0; j<mData.TSD_EleBC[i].length; j++)
        free(mData.TSD_EleBC[i].TS[j]);
      free(mData.TSD_EleBC[i].TS);
    }
  }
  free(mData.DummyY);

  free(cData.Tout);
  return 0;
}

