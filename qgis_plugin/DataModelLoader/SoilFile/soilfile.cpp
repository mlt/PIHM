#include "soilfile.h"
//#include "ui_soilfile.h"
#include <QFileDialog>
#include <fstream>
#include <math.h>
using namespace std;

#include "../../pihmLIBS/helpDialog/helpdialog.h"
#include <qgsproject.h>

SoilFile::SoilFile(QWidget *parent)
  : QDialog(parent), ui(new Ui::SoilFile)
{
  ui->setupUi(this);
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString tempStr=p->readPath(p->readEntry("pihm", "/model/mesh")); tempStr.truncate(tempStr.length()-4);
  ui->lineEditSoilFile->setText(tempStr+"soil");

  tempStr = p->readEntry("pihm", "soil");
  if (!tempStr.isEmpty()) {
    ui->lineEditSoilTexture->setText(p->readPath(tempStr)); // 87
  }
}

SoilFile::~SoilFile()
{
  delete ui;
}

void SoilFile::on_pushButtonClose_clicked()
{
  this->close();
}

void SoilFile::on_pushButtonSoilTexture_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString s = QFileDialog::getOpenFileName(this, "Choose Soil Texture File", projDir, "Texture (*.txt *.TXT)");
  ui->lineEditSoilTexture->setText(s);
}

void SoilFile::on_pushButtonSoilFile_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString s = QFileDialog::getSaveFileName(this, "Choose Soil File Name", projDir+"/DataModel", "SOIL file (*.soil)");
  if(!s.endsWith(".soil"))
    s.append(".soil");
  ui->lineEditSoilFile->setText(s);
}

void SoilFile::on_pushButtonRun_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  p->writeEntry("pihm", "soil", p->writePath(ui->lineEditSoilTexture->text())); // 87
  p->writeEntry("pihm", "/model/soil", p->writePath(ui->lineEditSoilFile->text())); // 88

  int RunFlag=1;
  ifstream inFile;
  ofstream outFile;
  inFile.open((ui->lineEditSoilTexture->text()).toAscii());
  outFile.open((ui->lineEditSoilFile->text()).toAscii());

  ofstream logFile;
  QDir dir = QDir::home();
  QString home = dir.homePath();
  QString logFileName(home+"/log.html");
  logFile.open(qPrintable(logFileName));
  logFile<<"<html><body><font size=3 color=black> Verifying Files...<br>";
  logFile.close();
  ui->textBrowser->setSource(logFileName);
  ui->textBrowser->setFocus();
  ui->textBrowser->setModified(TRUE);

  logFile.open(qPrintable(logFileName), ios::app);
  logFile<<"Checking Soil Texture File... ";
  if (inFile == NULL) {
    logFile<<"could NOT open file.  ERROR<br>";
    RunFlag = 0;
  }
  else
    logFile<<"Done!<br>";
  logFile<<"Checking Soil File... ";
  if (outFile == NULL) {
    logFile<<"could NOT open file to write.  ERROR<br>";
    RunFlag = 0;
  }
  else
    logFile<<"Done!<br>";
  logFile.close();
  ui->textBrowser->reload();

  if(RunFlag == 1) {

    logFile.open(qPrintable(logFileName), ios::app);
    logFile<<"Generating SOIL file... ";

    char tempS[500];
    int count=0;
    double data[1000][6];
    // (1) MUKEY (2) SILT (3) CLAY (4) ORGANIC_MATTER (5) BULK_DENSITY //(6) TOPSOIL = 1
    //inFile>>tempS; inFile>>tempS; inFile>>tempS; inFile>>tempS; inFile>>tempS; inFile>>tempS;
    inFile.getline(tempS, 500);
    while(inFile) {
      inFile>>data[count][0];       //MUKEY
      inFile>>data[count][1];       //SILT
      inFile>>data[count][2];       //CLAY
      inFile>>data[count][3]; if(data[count][3]<0) data[count][3]=2.5;       //ORGANIC MATTER
      inFile>>data[count][4]; if(data[count][4]<0) data[count][4]=1.3;      // BULK DENSITY (g/cm3)
      //inFile>>data[count][5]; // TOP SOIL
      data[count][5] = 1;
      //? outFile<<"-$ "<<data[count][0]<<"\t"<<data[count][1]<<"\t"<<data[count][2]<<"\t"<<data[count][3]<<"\t"<<data[count][4]<<"\t"<<data[count][5]<<" $-  \n";
      count++;
    }
    count--;
    outFile<<count<<"\n";

    double S, C, OM, D;     //S=SILT, C=CLAY, OM=ORGANIC MATTER, D=BULK DENSITY,
    int topsoil;     //TOPSOIL
    double **outData;
    outData = (double **)malloc(count*sizeof(double *));
    for(int i=0; i<count; i++) {
      outData[i] = (double *)malloc(9*sizeof(double));
      for(int j=0; j<9; j++)
        outData[i][j]=0.0;

      S = data[i][1];
      C = data[i][2];
      OM = data[i][3];
      D = data[i][4];
      topsoil = (int) data[i][5];
      //? outFile<<"-> "<<S<<"\t"<<C<<"\t"<<OM<<"\t"<<D<<"\t"<<topsoil<<" <-  \n";

      //Index
      outData[i][0]=i+1;
      outFile<<outData[i][0]<<"\t";
      //KsatV
      //outData[i][1]= (7.755+0.03252*S+0.93*topsoil-0.967*D*D-0.000484*C*C-0.000322*S*S+0.001/S-0.0748/OM-0.643*log(S)-0.01398*D*C-0.1673*D*OM+0.02986*topsoil*C-0.03305*topsoil*S);
      outData[i][1]=exp(7.755+0.03252*S+0.93*topsoil-0.967*D*D-0.000484*C*C-0.000322*S*S+0.001/S-0.0748/OM-0.643*log(S)-0.01398*D*C-0.1673*D*OM+0.02986*topsoil*C-0.03305*topsoil*S);
      outData[i][1]=outData[i][1]/100;       //UNIT CONVERSION cm/d to m/day
      outFile<<outData[i][1]<<"\t";
      //ThetaS
      //outData[i][2]=(0.7919+0.001691*C-0.29619*D-0.000001491*S*S+0.0000821*OM*OM+0.02427/C+0.01113/S+0.01472*log(S)-0.0000733*OM*C-0.000619*D*C-0.001183*D*OM-0.0001664*topsoil*S);
      outData[i][2]=  (0.7919+0.001691*C-0.29619*D-0.000001491*S*S+0.0000821*OM*OM+0.02427/C+0.01113/S+0.01472*log(S)-0.0000733*OM*C-0.000619*D*C-0.001183*D*OM-0.0001664*topsoil*S);
      outFile<<outData[i][2]<<"\t";
      //ThetaR
      outData[i][3]=0.01;
      outFile<<outData[i][3]<<"\t";
      //InfD
      outData[i][4]=0.10;
      outFile<<outData[i][4]<<"\t";
      //Alpha
      //outData[i][5]=log(-14.96+0.03135*C+0.0351*S+0.646*OM+15.29*D-0.192*topsoil-4.671*D*D-0.000781*C*C-0.00687*OM*OM+0.0449/OM+0.0663*log(S)+0.1482*log(OM)-0.04546*D*S-0.4852*D*OM+0.00673*topsoil*C);
      outData[i][5]=  exp(-14.96+0.03135*C+0.0351*S+0.646*OM+15.29*D-0.192*topsoil-4.671*D*D-0.000781*C*C-0.00687*OM*OM+0.0449/OM+0.0663*log(S)+0.1482*log(OM)-0.04546*D*S-0.4852*D*OM+0.00673*topsoil*C);
      outData[i][5]= 100*outData[i][5]; //UNIT CONVERSION 1/cm TO 1/m
      outFile<<outData[i][5]<<"\t";
      //Beta
      outData[i][6]=1+exp(-25.23-0.02195*C+0.0074*S-0.1940*OM+45.5*D-7.24*D*D+0.0003658*C*C+0.002885*OM*OM-12.81/D-0.1524/S-0.01958/OM-0.2876*log(S)-0.0709*log(OM)-44.6*log(D)-0.02264*D*C+0.0896*D*OM+0.00718*topsoil*C);
      outFile<<outData[i][6]<<"\t";
      //hAreaF
      outData[i][7]=0.01;
      outFile<<outData[i][7]<<"\t";
      //macKsatV
      outData[i][8]=100*outData[i][1];
      outFile<<outData[i][8]<<"\n";
    }
    logFile<<"Done!<br>";
    logFile<<"SoilFile Module - COMPLETE";
    logFile.close();
    ui->textBrowser->reload();
    QApplication::processEvents();
  }
}

void SoilFile::on_pushButtonHelp_clicked()
{
  helpDialog* hlpDlg = new helpDialog(this, "Soil File", 1, "helpFiles/soilfile.html", "Help :: Soil File");
  hlpDlg->show();
}
