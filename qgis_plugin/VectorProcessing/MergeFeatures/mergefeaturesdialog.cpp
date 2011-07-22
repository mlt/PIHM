#include <QtGui>
#include "mergefeaturesdialog.h"
#include "../../pihmLIBS/helpDialog/helpdialog.h"
#include "../../pihmLIBS/mergeFeatures.h"
#include "../../pihmLIBS/shapefil.h"

#include "../../pihmLIBS/fileStruct.h"

#include <fstream>
using namespace std;

mergeFeaturesDialogDlg::mergeFeaturesDialogDlg(QWidget *parent)
{
  setupUi(this);
  connect(addFileButton, SIGNAL(clicked()), this, SLOT(addBrowse()));
  connect(removeFileButton, SIGNAL(clicked()), this, SLOT(removeBrowse()));
  connect(removeAllButton, SIGNAL(clicked()), this, SLOT(removeAllBrowse()));
  connect(outputMergedFileButton, SIGNAL(clicked()), this, SLOT(outputBrowse()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

  clusterToleranceLineEdit->setDisabled(1);

  QStringList labels;
  labels << "Split Line Features (Constraining layer)";
  inputOutputTable->setColumnLabels(labels);

  QString projDir, projFile;
  QFile tFile(QDir::homePath()+"/project.txt");
  tFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream tin(&tFile);
  projDir  = tin.readLine();
  projFile = tin.readLine();
  tFile.close();
  cout << qPrintable(projDir);

  QString tempStr; tempStr=readLineNumber(qPrintable(projFile), 33);
  if(tempStr.length()>1) {
    int rows = inputOutputTable->numRows();
    inputOutputTable->insertRows(rows);
    Q3TableItem *tempItem;
    tempItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, tempStr);
    inputOutputTable->setItem(rows, 0, tempItem);
  }
  tempStr=readLineNumber(qPrintable(projFile), 35);
  if(tempStr.length()>1) {
    int rows = inputOutputTable->numRows();
    inputOutputTable->insertRows(rows);
    Q3TableItem *tempItem;
    tempItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, tempStr);
    inputOutputTable->setItem(rows, 0, tempItem);
  }
  outputMergedFileLineEdit->setText(projDir+"/VectorProcessing/MergeFeatures.shp");
}

void mergeFeaturesDialogDlg::addBrowse()
{
  QString projDir, projFile;
  QFile tFile(QDir::homePath()+"/project.txt");
  tFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream tin(&tFile);
  projDir  = tin.readLine();
  projFile = tin.readLine();
  tFile.close();
  cout << qPrintable(projDir);

  QStringList temp = QFileDialog::getOpenFileNames(this, "Choose File", projDir+"/VectorProcessing","Shape Files(*.shp *.SHP)");
  QString str = "";
  //QString str1 = "";

  unsigned int i;

  int rows = inputOutputTable->numRows();

  for(i=0; i< temp.count(); i++)
  {
    //std::cout<<"\n"<<temp[i].ascii();
    str = temp[i];
    //str1 = temp[i];
    //str1.truncate(str1.length()-4);
    //str1.append("_Split.shp");
    inputOutputTable->insertRows(rows + i);
    Q3TableItem *tmpItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, str );
    inputOutputTable->setItem(rows+i,0,tmpItem);
    //tmpItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, str1 );
    //inputOutputTable->setItem(rows+i,1,tmpItem);
  }
}

void mergeFeaturesDialogDlg::removeBrowse()
{
  int tmp = inputOutputTable->numRows();
  //qWarning("\n %d",tmp);
  Q3MemArray <int> rowArray(tmp);
  int j=0;
  for(int i=0; i<tmp; i++) {
    if(inputOutputTable->isRowSelected(i))
    {
      rowArray[j]=i;
      qWarning("\n %d",rowArray[j]);
      j++;
    }
  }
  rowArray.truncate(j);
  inputOutputTable->removeRows(rowArray);
}

void mergeFeaturesDialogDlg::removeAllBrowse()
{
  int tmp = inputOutputTable->numRows();
  for(int i=0; i<= tmp; i++) {
    inputOutputTable->removeRow(0);
  }
}

void mergeFeaturesDialogDlg::outputBrowse()
{
  QString projDir, projFile;
  QFile tFile(QDir::homePath()+"/project.txt");
  tFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream tin(&tFile);
  projDir  = tin.readLine();
  projFile = tin.readLine();
  tFile.close();
  cout << qPrintable(projDir);

  QString temp = QFileDialog::getSaveFileName(this, "Choose File", projDir+"/VectorProcessing","Shape File(*.shp *.SHP)");
  QString tmp = temp;
  if(!(tmp.toLower()).endsWith(".shp")) {
    tmp.append(".shp");
    temp = tmp;
  }
  outputMergedFileLineEdit->setText(temp);
}

void mergeFeaturesDialogDlg::run()
{
  QString projDir, projFile;
  QFile tFile(QDir::homePath()+"/project.txt");
  tFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream tin(&tFile);
  projDir  = tin.readLine();
  projFile = tin.readLine();
  tFile.close();
  cout << qPrintable(projDir);

  writeLineNumber(qPrintable(projFile), 36, qPrintable(inputOutputTable->text(0,0)));
  if(inputOutputTable->numRows()>0) {
    writeLineNumber(qPrintable(projFile), 37, qPrintable(inputOutputTable->text(1,0)));
  }
  writeLineNumber(qPrintable(projFile), 38, qPrintable(outputMergedFileLineEdit->text()));
  //writeLineNumber(qPrintable(projFile), 39, qPrintable(projDir+"/DomainDecomposition/"+id+".shp"));

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



  int rows = inputOutputTable->numRows();
  qWarning("Number of rows = %d", rows);
  const char **shpFileNamesChar = new const char*[rows];
  const char **dbfFileNamesChar = new const char*[rows];
  QString *shpFileNames = new QString[rows];
  QString *dbfFileNames = new QString[rows];
  int fileCount = 0;

  QString shpFileName, dbfFileName, shpFileNameMerge, dbfFileNameMerge, shxFileNameMerge;
  //double tol;
  int runFlag = 1;
  if(inputOutputTable->numRows() < 2) {
    log.open(qPrintable(logFileName), ios::app);
    log<<"<p><font size=3 color=red> First Please input Files</p>";
    log.close();
    messageLog->reload();
    QApplication::processEvents();
  }
  else{
    for(int i=0; i<rows; i++) {
      shpFileName = inputOutputTable->text(i, 0);
      dbfFileName = shpFileName;
      dbfFileName.truncate(dbfFileName.length()-3);
      dbfFileName.append("dbf");

      ifstream file;
      file.open(qPrintable(shpFileName), ios::in);
      log.open(qPrintable(logFileName), ios::app);

      qWarning("\n %s", qPrintable(shpFileName));

      log<<"<p>Checking... "<<qPrintable(shpFileName)<<"... ";
      if(file == NULL) {
        log<<"<font size=3 color=red> Error!</p>";
        runFlag = 0;
      }
      else{
        log<<"Done!</p>";
        shpFileNamesChar[fileCount] = new char[200];
        dbfFileNamesChar[fileCount] = new char[200];
        shpFileNamesChar[fileCount] = qPrintable(shpFileName);
        dbfFileNamesChar[fileCount] = qPrintable(dbfFileName);

        shpFileNames[fileCount] = shpFileName; //.ascii();
        dbfFileNames[fileCount] = dbfFileName;
        fileCount++;
      }
      log.close();
      messageLog->reload();
      QApplication::processEvents();
    }
  }

  ofstream out;
  out.open(qPrintable(outputMergedFileLineEdit->text()));

  log.open(qPrintable(logFileName), ios::app);
  if((outputMergedFileLineEdit->text()).length()==0) {
    runFlag = 0;
    log<<"<p><font size=3 color=red> Error! Please input Merge Output File</p>";
    qWarning("\nPlease enter output file name");
  }
  else{
    log<<"<p>Checking... "<< qPrintable(outputMergedFileLineEdit->text())<<"... ";
    if(out == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      //qWarning("\n%s doesn't exist!", (inputFileLineEdit->text()).ascii());
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  messageLog->reload();
  QApplication::processEvents();


  if(runFlag == 1) {
    shpFileNameMerge = outputMergedFileLineEdit->text();
    dbfFileNameMerge = shpFileNameMerge;
    dbfFileNameMerge.truncate(dbfFileNameMerge.length()-3);
    shxFileNameMerge=dbfFileNameMerge;
    dbfFileNameMerge.append("dbf");

    log.open(qPrintable(logFileName), ios::app);
    log<<"<p>Running...";
    log.close();
    messageLog->reload();
    QApplication::processEvents();



    mergeFeatures(shpFileNamesChar, dbfFileNamesChar, fileCount, qPrintable(shpFileNameMerge), qPrintable(dbfFileNameMerge));

    QString myFileNameQString = shpFileNameMerge;
    QFileInfo myFileInfo(myFileNameQString);
    QString myBaseNameQString = myFileInfo.baseName();
    QString provider = "OGR";
    applicationPointer->addVectorLayer(myFileNameQString, myBaseNameQString, "ogr");

    shxFileNameMerge.truncate(shxFileNameMerge.length()-1);
    QString id=shxFileNameMerge;
    id=id.right(id.length()-id.lastIndexOf("/",-1)-1);
    cout << "ID = "<<qPrintable(id) <<"\n";
    shxFileNameMerge.append(".shx");

    QString shpFile = projDir+"/DomainDecomposition/"+id+".shp";
    QString dbfFile = projDir+"/DomainDecomposition/"+id+".dbf";
    QString shxFile = projDir+"/DomainDecomposition/"+id+".shx";

    QFile::copy(shpFileNameMerge, shpFile);
    QFile::copy(dbfFileNameMerge, dbfFile);
    QFile::copy(shxFileNameMerge, shxFile);

    writeLineNumber(qPrintable(projFile), 39, qPrintable(shpFile));

    log.open(qPrintable(logFileName), ios::app);
    log<<" Done!</p>";
    log.close();
    messageLog->reload();
    QApplication::processEvents();
  }
}

void mergeFeaturesDialogDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Vecto Merge", 1, "helpFiles/mergeFeaturesDialog.html", "Help :: Vector Merge");
  hlpDlg->show();
}

void mergeFeaturesDialogDlg::setApplicationPointer(QgisInterface* appPtr){
  applicationPointer = appPtr;
}
