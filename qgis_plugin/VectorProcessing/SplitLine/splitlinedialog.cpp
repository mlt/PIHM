#include <QtGui>
#include "splitlinedialog.h"
#include "../../pihmLIBS/helpDialog/helpdialog.h"
#include "../../pihmLIBS/splitLineAtVertices.h"
#include <shapefil.h>

#include <qgsproject.h>

#include <fstream>
using namespace std;

splitLineDialogDlg::splitLineDialogDlg(QWidget *parent)
{
  setupUi(this);
  connect(addFileButton, SIGNAL(clicked()), this, SLOT(addBrowse()));
  connect(removeFileButton, SIGNAL(clicked()), this, SLOT(removeBrowse()));
  connect(removeAllButton, SIGNAL(clicked()), this, SLOT(removeAllBrowse()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

  QStringList labels;
  labels << "Input (simplified) Polylines" << "Output Split Lines";
  inputOutputTable->setColumnLabels(labels);

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));
  // TODO: worth trying p->readListEntry()
  QString tempStr; tempStr=p->readPath(p->readEntry("pihm", "catsimple"));       // 27
  if(tempStr.length()>1) {
    int rows = inputOutputTable->numRows();
    inputOutputTable->insertRows(rows);
    Q3TableItem *tempItem;
    tempItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, tempStr);
    inputOutputTable->setItem(rows, 0, tempItem);
    tempStr.truncate(tempStr.length()-4); tempStr.append("_Split.shp");
    tempItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, tempStr);
    inputOutputTable->setItem(rows, 1, tempItem);
  }
  tempStr; tempStr=p->readPath(p->readEntry("pihm", "strsimple")); // 30
  if(tempStr.length()>1) {
    int rows = inputOutputTable->numRows();
    inputOutputTable->insertRows(rows);
    Q3TableItem *tempItem;
    tempItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, tempStr);
    inputOutputTable->setItem(rows, 0, tempItem);
    tempStr.truncate(tempStr.length()-4); tempStr.append("_Split.shp");
    tempItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, tempStr);
    inputOutputTable->setItem(rows, 1, tempItem);
  }
}

void splitLineDialogDlg::addBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QStringList temp = QFileDialog::getOpenFileNames(this, "Choose File", projDir+"/VectorProcessing","Shape Files(*.shp *.SHP)");
  QString str = "";
  QString str1 = "";

  unsigned int i;

  int rows = inputOutputTable->numRows();

  for(i=0; i< temp.count(); i++)
  {
    //std::cout<<"\n"<<qPrintable(temp[i]);
    str = temp[i];
    str1 = temp[i];
    str1.truncate(str1.length()-4);
    str1.append("_Split.shp");
    inputOutputTable->insertRows(rows + i);
    Q3TableItem *tmpItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, str );
    inputOutputTable->setItem(rows+i,0,tmpItem);
    tmpItem = new Q3TableItem(inputOutputTable, Q3TableItem::Always, str1 );
    inputOutputTable->setItem(rows+i,1,tmpItem);
  }
}

void splitLineDialogDlg::removeBrowse()
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

void splitLineDialogDlg::removeAllBrowse()
{
  int tmp = inputOutputTable->numRows();
  for(int i=0; i<= tmp; i++) {
    inputOutputTable->removeRow(0);
  }
}


void splitLineDialogDlg::run()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  p->writeEntry("pihm", "catsimple", p->writePath(inputOutputTable->text(0,0))); // 32
  p->writeEntry("pihm", "catsplit", p->writePath(inputOutputTable->text(0,1))); // 33
  if(inputOutputTable->numRows()>0) {
    p->writeEntry("pihm", "strsimple", p->writePath(inputOutputTable->text(1,0))); // 34
    p->writeEntry("pihm", "strsplit", p->writePath(inputOutputTable->text(1,1))); // 35
  }

  QDir dir = QDir::home();
  QString home = dir.homePath();
  QString logFileName(qPrintable(home+"/log.html"));
  ofstream log;
  log.open(qPrintable(logFileName));
  log<<"<html><body><font size=3 color=black><p> Verifying Files...</p></font></body></html>";
  log.close();
  MessageLog->setSource(logFileName);
  MessageLog->setFocus();
  MessageLog->setModified(TRUE);

  if(inputOutputTable->numRows() < 1) {
    log.open(qPrintable(logFileName), ios::app);
    log<<"<p><font size=3 color=red> First Please input Files</p>";
    log.close();
    MessageLog->reload();
    QApplication::processEvents();
  }
  else{
    int rows = inputOutputTable->numRows();
    QString shpFileName, dbfFileName, shpFileNameSplit, dbfFileNameSplit;
    for(int i=0; i<rows; i++) {
      shpFileName = inputOutputTable->text(i, 0);
      dbfFileName = shpFileName;
      dbfFileName.truncate(dbfFileName.length()-3);
      dbfFileName.append("dbf");

      shpFileNameSplit = inputOutputTable->text(i, 1);
      dbfFileNameSplit = shpFileNameSplit;
      dbfFileNameSplit.truncate(dbfFileNameSplit.length()-3);
      dbfFileNameSplit.append("dbf");

      ifstream in;
      in.open(qPrintable(shpFileName));
      ofstream out;
      out.open(qPrintable(shpFileNameSplit));

      int runFlag = 1;
      log.open(qPrintable(logFileName), ios::app);
      log<<"<p>Checking... "<<qPrintable(shpFileName)<<"... ";
      if(in == NULL) {
        log<<"<font size=3 color=red> Error!</p>";
        runFlag = 0;
        qWarning("\n %s doesn't exist!", qPrintable(shpFileName));
      }
      else
        log<<"Done!</p>";
      log<<"<p>Checking... "<<qPrintable(shpFileNameSplit)<<"... ";
      if(out == NULL) {
        log<<"<font size=3 color=red> Error!</p>";
        runFlag = 0;
        qWarning("\n %s doesn't exist!", qPrintable(shpFileNameSplit));
      }
      else
        log<<"Done!</p>";
      log.close();
      MessageLog->reload();
      QApplication::processEvents();

      if(runFlag == 1) {
        log.open(qPrintable(logFileName), ios::app);
        log<<"<p>Running... "<<qPrintable(shpFileName);
        log.close();
        MessageLog->reload();
        QApplication::processEvents();

        splitLineAtVertices(qPrintable(shpFileName), qPrintable(dbfFileName), qPrintable(shpFileNameSplit), qPrintable(dbfFileNameSplit));

        QString myFileNameQString = shpFileNameSplit;
        QFileInfo myFileInfo(myFileNameQString);
        QString myBaseNameQString = myFileInfo.baseName();
        QString provider = "OGR";
        applicationPointer->addVectorLayer(myFileNameQString, myBaseNameQString, "ogr");

        log.open(qPrintable(logFileName), ios::app);
        log<<" Done!</p>";
        log.close();
        MessageLog->reload();
        QApplication::processEvents();
      }
      in.close();
      out.close();
    }
  }
}

void splitLineDialogDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Split Line", 1, "helpFiles/splitLineDialog.html", "Help :: Split Line");
  hlpDlg->show();
}

void splitLineDialogDlg::setApplicationPointer(QgisInterface* appPtr){
  applicationPointer = appPtr;
}
