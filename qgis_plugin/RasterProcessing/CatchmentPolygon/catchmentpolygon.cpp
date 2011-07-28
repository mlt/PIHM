#include <QtGui>
#include "catchmentpolygon.h"
#include "../../pihmRasterLIBS/lsm.h"

#include "../../pihmLIBS/helpDialog/helpdialog.h"
#include <qgsproject.h>
#include <gdal_alg.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>

#include <iostream>
#include <fstream>
using namespace std;

CatchmentPolygonDlg::CatchmentPolygonDlg(QWidget *parent)
{
  setupUi(this);
  connect(inputBrowseButton, SIGNAL(clicked()), this, SLOT(inputBrowse()));
  connect(outputBrowseButton, SIGNAL(clicked()), this, SLOT(outputBrowse()));
  connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));
  inputFileLineEdit->setText(p->readPath(p->readEntry("pihm", "catgrid"))); // 19

  int Thresh(p->readNumEntry("pihm", "facThreshold"));
  outputFileLineEdit->setText(projDir+"/RasterProcessing/cat"+ QString::number(Thresh) +".shp");
}

void CatchmentPolygonDlg::inputBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/RasterProcessing","CatchmentGrid File(*.adf *.asc)");
  inputFileLineEdit->setText(str);
}


void CatchmentPolygonDlg::outputBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString temp = QFileDialog::getSaveFileName(this, "Choose File", projDir+"/RasterProcessing","Catchment Polygon File(*.shp)");
  QString tmp = temp;
  if(!(tmp.toLower()).endsWith(".shp")) {
    tmp.append(".shp");
    temp = tmp;
  }

  outputFileLineEdit->setText(temp);
}


void CatchmentPolygonDlg::run()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));
  p->writeEntry("pihm", "catgrid", p->writePath(inputFileLineEdit->text())); // 20
  p->writeEntry("pihm", "catpoly", p->writePath(outputFileLineEdit->text())); // 21

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


  QString inputFileName((inputFileLineEdit->text()));
  QString outputShpFileName((outputFileLineEdit->text()));
  QString outputDbfFileName; QString outputShxFileName, id;
  outputDbfFileName = outputShpFileName;
  outputDbfFileName.truncate(outputDbfFileName.length()-3); outputShxFileName=outputDbfFileName;
  outputDbfFileName.append("dbf");

  outputShxFileName.truncate(outputShxFileName.length()-1);
  id=outputShxFileName;
  id=id.right(id.length()-id.lastIndexOf("/",-1)-1);
  cout << "ID = "<<qPrintable(id) <<"\n";
  outputShxFileName.append(".shx");

  QString shpFile = projDir+"/VectorProcessing/"+id+".shp";
  QString dbfFile = projDir+"/VectorProcessing/"+id+".dbf";
  QString shxFile = projDir+"/VectorProcessing/"+id+".shx";

  ifstream inFile;      inFile.open(qPrintable(inputFileLineEdit->text()));
//  ofstream outFile;    outFile.open(qPrintable(outputFileLineEdit->text()));
  int runFlag = 1;

  log.open(qPrintable(logFileName), ios::app);
  if(inputFileName.length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input Catchment Grid Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable(inputFileName)<<"... ";
    if(inFile == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      qWarning("\n%s doesn't exist!", qPrintable(inputFileLineEdit->text()));
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  messageLog->reload();
  QApplication::processEvents();

  log.open(qPrintable(logFileName), ios::app);
  if(outputShpFileName.length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input Catchment Polygon Output File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable(outputShpFileName)<<"... ";
/*    if(outFile == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      qWarning("\nCan not open output file name");
      runFlag = 0;
    }
    else*/
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

    Convert(inputFileName, outputShpFileName);

    char err = 0;
    if (QFile::exists(shpFile) && !QFile::remove(shpFile)) err |= 1;
    if (QFile::exists(dbfFile) && !QFile::remove(dbfFile)) err |= 2;
    if (QFile::exists(shxFile) && !QFile::remove(shxFile)) err |= 4;
    const char* msg[] = {" shp", " dbf", " shx"};
    QString s("Failed to remove old:");
    char idx = 0;
    for(; err; err>>=1, ++idx)
      if (err & 1) {
        s += msg[idx];
      }
    if (idx)
      QMessageBox::warning(this, "Failed to remove old files", s);

    QFile::copy(outputShpFileName, shpFile);
    QFile::copy(outputDbfFileName, dbfFile);
    QFile::copy(outputShxFileName, shxFile);

    p->writeEntry("pihm", "catpoly", shpFile); //21

    log.open(qPrintable(logFileName), ios::app);
    log<<" Done!</p>";
    log.close();
    messageLog->reload();
    QApplication::processEvents();

    if(showSG_DFrame->isChecked() == 1) {
      QString myFileNameQString = outputShpFileName;
      QFileInfo myFileInfo(myFileNameQString);
      QString myBaseNameQString = myFileInfo.baseName();
      QString provider = "OGR";
      cout<<"\n"<<myFileNameQString.ascii()<<"\n"<<myBaseNameQString.ascii()<<"\n"<<provider.ascii()<<"\n";
      //getchar(); getchar();
      applicationPointer->addVectorLayer(myFileNameQString, myBaseNameQString, "ogr");
      //QgsRasterLayer *tempLayer = new QgsRasterLayer("/backup/pihm/RasterProcessing/FillPits", "morgedem.asc");
//??			applicationPointer->addLayer(QStringList(outputShpFileName), NULL);
    }
  }
}

void CatchmentPolygonDlg::Convert(QString inputFileName, QString outputShpFileName) {
    GDALDataset* input = (GDALDataset*) GDALOpen ( inputFileName.toUtf8(), GA_ReadOnly);
    GDALRasterBandH rb = input->GetRasterBand(1);
    const char* proj = input->GetProjectionRef();
    OGRSpatialReference sr(proj); //    sr.importFromWkt(proj);
    const char pszDriverName[] = "ESRI Shapefile";
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( pszDriverName );
    if (QFile::exists(outputShpFileName))
      poDriver->DeleteDataSource(outputShpFileName.toUtf8());
    OGRDataSource *poDS = poDriver->CreateDataSource( outputShpFileName.toUtf8(), NULL );
    if (NULL != poDS) {
      OGRLayer *output = poDS->CreateLayer( "polygon_out", &sr, wkbPolygon, NULL );
      OGRFieldDefn oField( "lineNum", OFTInteger );
      output->CreateField( &oField ); // != OGRERR_NONE
      char* options[] = {"8CONNECTED=8"};
      // TODO: Move it away from static and make progress bar feedback
      GDALPolygonize( rb, rb, output, 0, options, NULL, NULL );
      OGRDataSource::DestroyDataSource( poDS );
    } else
      QMessageBox::critical(NULL, "Failed to create output shapefile", outputShpFileName);
    GDALClose(input);
}

void CatchmentPolygonDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Catchment Polygon", 1, "helpFiles/catchmentpolygon.html", "Help :: Catchment Polygon");
  hlpDlg->show();

}
/* ?? */
void CatchmentPolygonDlg::setApplicationPointer(QgisInterface* appPtr){
  applicationPointer = appPtr;
}
