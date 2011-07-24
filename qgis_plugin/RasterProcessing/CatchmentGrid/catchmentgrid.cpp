#include <QtGui>
#include "catchmentgrid.h"
#include "../../pihmRasterLIBS/lsm.h"
#include "../../pihmRasterLIBS/flood.h"
#include "../../pihmRasterLIBS/setdir.h"
#include "../../pihmRasterLIBS/aread8.h"
#include "../../pihmRasterLIBS/catProcessing.h"

#include "../../pihmLIBS/helpDialog/helpdialog.h"
#include <qgsproject.h>

#include <iostream>
#include <fstream>
using namespace std;

CatchmentGridDlg::CatchmentGridDlg(QWidget *parent)
{
  setupUi(this);
  connect(inputLNKBrowseButton, SIGNAL(clicked()), this, SLOT(inputLNKBrowse()));
  connect(inputFDRBrowseButton, SIGNAL(clicked()), this, SLOT(inputFDRBrowse()));
  connect(outputBrowseButton, SIGNAL(clicked()), this, SLOT(outputBrowse()));
  connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));
  inputLNKFileLineEdit->setText(p->readPath(p->readEntry("pihm", "lnk")));
  inputFDRFileLineEdit->setText(p->readPath(p->readEntry("pihm", "fdr")));

  int Thresh(p->readNumEntry("pihm", "facThreshold"));
  outputFileLineEdit->setText(projDir+"/RasterProcessing/cat"+ QString::number(Thresh) +".asc");
}

void CatchmentGridDlg::inputLNKBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/RasterProcessing","Link Grid File(*.adf *.asc)");
  inputLNKFileLineEdit->setText(str);
}

void CatchmentGridDlg::inputFDRBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/RasterProcessing","Flow Dir Grid File(*.adf *.asc)");
  inputFDRFileLineEdit->setText(str);
}

void CatchmentGridDlg::outputBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString temp = QFileDialog::getSaveFileName(this, "Choose File", projDir+"/RasterProcessing","Catchment Grid File(*.adf *.asc)");
  QString tmp = temp;
  if(!(tmp.toLower()).endsWith(".asc")) {
    tmp.append(".asc");
    temp = tmp;
  }

  outputFileLineEdit->setText(temp);
}


void CatchmentGridDlg::run()
{
  QgsProject *p = QgsProject::instance();
  p->writeEntry("pihm", "lnk", p->writePath(inputLNKFileLineEdit->text())); // 17
  p->writeEntry("pihm", "fdr", p->writePath(inputFDRFileLineEdit->text())); // 18
  p->writeEntry("pihm", "catgrid", p->writePath(outputFileLineEdit->text())); // 19

  QDir dir = QDir::home();
  QString home = dir.homePath();
  QString logFileName(qPrintable(home+"/log.html"));
  ofstream log;
  log.open(qPrintable(logFileName));
  log<<"<html><body><font size=3 color=black><p> Verifying Files...</p></font></body></html>";
  log.close();
  messageLog->setSource(logFileName);
  messageLog->setFocus();
  messageLog->setModified(TRUE);

  QString inputLNKFileName((inputLNKFileLineEdit->text()));
  QString inputFDRFileName((inputFDRFileLineEdit->text()));
  QString outputFileName((outputFileLineEdit->text()));

  ifstream LNKinFile;      LNKinFile.open(qPrintable(inputLNKFileLineEdit->text()));
  ifstream FDRinFile;      FDRinFile.open(qPrintable(inputFDRFileLineEdit->text()));
  ofstream outFile;    outFile.open(qPrintable(outputFileLineEdit->text()));
  int runFlag = 1;

  log.open(qPrintable(logFileName), ios::app);
  if(inputLNKFileName.length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input Link Grid Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable(inputLNKFileName)<<"... ";
    if(LNKinFile == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      qWarning("\n%s doesn't exist!", qPrintable(inputLNKFileLineEdit->text()));
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  messageLog->reload();
  QApplication::processEvents();

  log.open(qPrintable(logFileName), ios::app);
  if(inputFDRFileName.length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input Flow Dir. Grid Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable(inputFDRFileName)<<"... ";
    if(FDRinFile == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      qWarning("\n%s doesn't exist!", qPrintable(inputFDRFileLineEdit->text()));
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  messageLog->reload();
  QApplication::processEvents();


  log.open(qPrintable(logFileName), ios::app);
  if(outputFileName.length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input Catchment Grid Output File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable(outputFileName)<<"... ";
    if(outFile == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      qWarning("\nCan not open output file name");
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  messageLog->reload();
  QApplication::processEvents();


  if(runFlag == 1) {
    log.open(qPrintable(logFileName), ios::app);
    log<<"<p>Running...";
    log.close();
    messageLog->reload();
    QApplication::processEvents();

    int err = catchmentGrid((char *)qPrintable(inputLNKFileName), (char *)qPrintable(inputFDRFileName), (char *)qPrintable(outputFileName) );

    log.open(qPrintable(logFileName), ios::app);
    log<<" Done!</p>";
    log.close();
    messageLog->reload();
    QApplication::processEvents();

    if(showSG_DFrame->isChecked() == 1) {
      //QgsRasterLayer *tempLayer = new QgsRasterLayer("/backup/pihm/RasterProcessing/FillPits", "morgedem.asc");
      //??
      applicationPointer->addRasterLayer(outputFileName);
    }
  }
}

void CatchmentGridDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Catchment Grid", 1, "helpFiles/catchmentgrid.html", "Help :: Catchment Grid");
  hlpDlg->show();

}
/* ??*/
void CatchmentGridDlg::setApplicationPointer(QgisInterface* appPtr){
  applicationPointer = appPtr;
}

