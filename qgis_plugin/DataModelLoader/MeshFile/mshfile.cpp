#include <QtGui>
#include "mshfile.h"

#include <iomanip>
#include <fstream>
using namespace std;

#include "../../pihmLIBS/interpRiverNodes.h"
#include "../../pihmLIBS/pickGridValue.h"
#include "../../pihmLIBS/helpDialog/helpdialog.h"

#include <qgsproject.h>

mshFileDlg::mshFileDlg(QWidget *parent)
{
  setupUi(this);
  connect(eleFileButton, SIGNAL(clicked()), this, SLOT(eleBrowse()));
  connect(nodeFileButton, SIGNAL(clicked()), this, SLOT(nodeBrowse()));
  connect(neighFileButton, SIGNAL(clicked()), this, SLOT(neighBrowse()));
  connect(sElevFileButton, SIGNAL(clicked()), this, SLOT(sEleBrowse()));
  connect(bElevFileButton, SIGNAL(clicked()), this, SLOT(bEleBrowse()));
  connect(mshFileButton, SIGNAL(clicked()), this, SLOT(mshBrowse()));
  connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

  bElevFileLineEdit->setShown(0);
  bElevFileButton->setShown(0);

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  eleFileLineEdit->setText(p->readPath(p->readEntry("pihm", "ele"))); // 46
  nodeFileLineEdit->setText(p->readPath(p->readEntry("pihm", "node"))); // 47
  rivFileLineEdit->setText(p->readPath(p->readEntry("pihm", "strsplit")));       // 37
  QString tempStr = p->readPath(p->readEntry("pihm", "node")); // 47
  tempStr.truncate(tempStr.length()-4); tempStr.append("neigh");
  neighFileLineEdit->setText(tempStr);
  //sElevFileLineEdit->setText(readLineNumber(qPrintable(projFile), 3)); //adf
  sElevFileLineEdit->setText(p->readPath(p->readEntry("pihm", "fill"))); // 4 //asc
  mshFileLineEdit->setText(p->readPath(p->readEntry("pihm", "/model/mesh")));
}

void mshFileDlg::eleBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/DomainDecomposition","ele File(*.ele *.ELE)");
  eleFileLineEdit->setText(str);
}

void mshFileDlg::nodeBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/DomainDecomposition","node File(*.node *.NODE)");
  nodeFileLineEdit->setText(str);
}

void mshFileDlg::neighBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/DomainDecomposition","neigh File(*.neigh *.NEIGH)");
  neighFileLineEdit->setText(str);
}

void mshFileDlg::sEleBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir,"Surface Elev File(*.adf *.ADF *.asc *ASC)");
  sElevFileLineEdit->setText(str);
}

void mshFileDlg::bEleBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString str = QFileDialog::getOpenFileName(this, "Choose File", projDir,"Bed Elev File(*.adf *.ADF *.asc *ASC)");
  bElevFileLineEdit->setText(str);
}

void mshFileDlg::mshBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString temp = QFileDialog::getSaveFileName(this, "Choose File", projDir+"/DataModel","mesh File(*.mesh *.MESH)");
  QString tmp = temp;
  if(!(tmp.toLower()).endsWith(".mesh")) {
    tmp.append(".mesh");
    temp = tmp;
  }
  mshFileLineEdit->setText(temp);
}

void mshFileDlg::run()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  p->writeEntry("pihm", "/model/mesh", p->writePath(mshFileLineEdit->text())); // 49
  QString tempStr = mshFileLineEdit->text();
  tempStr.truncate(tempStr.length()-5);
  tempStr=tempStr.right(tempStr.length()-tempStr.lastIndexOf("/", -1)-1);
  p->writeEntry("pihm", "ID", p->writePath(tempStr)); // 50

  QDir dir = QDir::home();
  QString home = dir.homePath();
  QString logFileName(home+"/log.html");
  ofstream log;
  log.open(qPrintable(logFileName));
  log<<"<html><body><font size=3 color=black><p> Verifying Files...</p></font></body></html>";
  log.close();
  MessageLog->setSource(logFileName);
  MessageLog->setFocus();
  MessageLog->setModified(TRUE);

  ifstream ele, node, neigh, sele, bele;
  ofstream mesh; ofstream mesh0;

  ele.open(qPrintable((eleFileLineEdit->text())));
  node.open(qPrintable((nodeFileLineEdit->text())));
  neigh.open(qPrintable((neighFileLineEdit->text())));
  sele.open(qPrintable((sElevFileLineEdit->text())));
  bele.open(qPrintable((bElevFileLineEdit->text())));
  //mesh.open(qPrintable((mshFileLineEdit->text())), ios::out);
  QString meshFile0 =  mshFileLineEdit->text();
  QString meshFile1; meshFile1 = meshFile0 + "0";
  mesh.open(qPrintable(meshFile1), ios::out);

  int runFlag = 1;
/*
        if(ele==NULL){
          qWarning("\n%s doesn't exist!", qPrintable((eleFileLineEdit->text())));
          runFlag=0;
        }
  if(node==NULL){
                qWarning("\n%s doesn't exist!", qPrintable((nodeFileLineEdit->text())));
                runFlag=0;
        }
  if(neigh==NULL){
                qWarning("\n%s doesn't exist!", qPrintable((neighFileLineEdit->text())));
                runFlag=0;
        }
  if(sele==NULL){
                qWarning("\n%s doesn't exist!", qPrintable((sElevFileLineEdit->text())));
                runFlag=0;
        }
  if(bele==NULL){
                qWarning("\n%s doesn't exist!", qPrintable((bElevFileLineEdit->text())));
                runFlag=0;
        }
  if(mesh==NULL){
                qWarning("\n Please Enter mesh file name");
                runFlag=0;
        }
*/

  log.open(qPrintable(logFileName), ios::app);
  if((eleFileLineEdit->text()).length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input .ele Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable((eleFileLineEdit->text()))<<"... ";
    if(ele == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      //qWarning("\n%s doesn't exist!", qPrintable((inputFileLineEdit->text())));
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  MessageLog->reload();
  QApplication::processEvents();


  log.open(qPrintable(logFileName), ios::app);
  if((nodeFileLineEdit->text()).length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input .node Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable((nodeFileLineEdit->text()))<<"... ";
    if(ele == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      //qWarning("\n%s doesn't exist!", qPrintable((inputFileLineEdit->text())));
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  MessageLog->reload();
  QApplication::processEvents();


  log.open(qPrintable(logFileName), ios::app);
  if((neighFileLineEdit->text()).length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input .neigh Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable((neighFileLineEdit->text()))<<"... ";
    if(neigh == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      //qWarning("\n%s doesn't exist!", qPrintable((inputFileLineEdit->text())));
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  MessageLog->reload();
  QApplication::processEvents();


  log.open(qPrintable(logFileName), ios::app);
  if((sElevFileLineEdit->text()).length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input Surface Elev. Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable((sElevFileLineEdit->text()))<<"... ";
    if(sele == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      //qWarning("\n%s doesn't exist!", (inputFileLineEdit->text()).ascii());
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  MessageLog->reload();
  QApplication::processEvents();


  log.open(qPrintable(logFileName), ios::app);
  if((bElevFileLineEdit->text()).length()==0 && radioButtonFile->isChecked()==1) {
    log<<"<p><font size=3 color=red> Error! Please input Bed Elev. Input File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable((bElevFileLineEdit->text()))<<"... ";
    if(bele == NULL && radioButtonFile->isChecked()==1) {
      log<<"<font size=3 color=red> Error!</p>";
      //qWarning("\n%s doesn't exist!", (inputFileLineEdit->text()).ascii());
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  MessageLog->reload();
  QApplication::processEvents();


  log.open(qPrintable(logFileName), ios::app);
  if((mshFileLineEdit->text()).length()==0) {
    log<<"<p><font size=3 color=red> Error! Please input .mesh Output File</p>";
    runFlag = 0;
  }
  else{
    log<<"<p>Checking... "<<qPrintable((mshFileLineEdit->text()))<<"... ";
    if(mesh == NULL) {
      log<<"<font size=3 color=red> Error!</p>";
      qWarning("\nCan not open output file name");
      runFlag = 0;
    }
    else
      log<<"Done!</p>";
  }
  log.close();
  MessageLog->reload();
  QApplication::processEvents();


  if(runFlag == 1) {

    log.open(qPrintable(logFileName), ios::app);
    log<<"<p>Running...";
    log.close();
    MessageLog->reload();
    QApplication::processEvents();

    GDALDataset *sElev, *bElev;
    double sRanges[6], bRanges[6];
    GDALAllRegister();
    sElev = (GDALDataset *)GDALOpen(qPrintable((sElevFileLineEdit->text())), GA_ReadOnly);
    if(radioButtonFile->isChecked()==1)
      bElev = (GDALDataset *)GDALOpen(qPrintable((bElevFileLineEdit->text())), GA_ReadOnly);
    getExtent(sElev, sRanges);
    if(radioButtonFile->isChecked()==1)
      getExtent(bElev, bRanges);


    int temp;
    int numEle, numNode, numNeigh;
    ele>>numEle; ele>>temp; ele>>temp;
    node>>numNode; node>>temp; node>>temp; node>>temp;
    neigh>>numNeigh; neigh>>temp;

    mesh<<numEle<<"\t"<<numNode;
    int index;
    int node0, node1, node2, nabr0, nabr1, nabr2;
    for(int i=0; i<numEle; i++) {
      ele>>index; ele>>node0; ele>>node1; ele>>node2;
      neigh>>temp; neigh>>nabr0; neigh>>nabr1; neigh>>nabr2;
      mesh<<"\n"<<index<<"\t"<<node0<<"\t"<<node1<<"\t"<<node2<<"\t"<<(nabr0<0 ? 0 : nabr0)<<"\t"<<(nabr1<0 ? 0 : nabr1)<<"\t"<<(nabr2<0 ? 0 : nabr2);
    }
    double X, Y, Zmin, Zmax;
    for(int i=0; i<numNode; i++) {
      node>>index; node>>X; node>>Y; node>>temp;
      //Zmin = getRasterValue(bElev, 1, X, Y, bRanges);
      Zmax = getRasterValue(sElev, 1, X, Y, sRanges);

      if(radioButtonFile->isChecked()==1)
        Zmin = getRasterValue(bElev, 1, X, Y, bRanges);
      else
        Zmin = Zmax-(lineEdit->text()).toDouble();
      mesh<<"\n"<<index<<"\t"<<setprecision(20)<<X<<"\t"<<setprecision(20)<<Y<<"\t"<<setprecision(15)<<Zmin<<"\t"<<setprecision(15)<<Zmax;
    }

    log.open(qPrintable(logFileName), ios::app);
    log<<" Done!</p>";
    log.close();
    MessageLog->reload();
    QApplication::processEvents();

    ///////////////////////////////////////////////////////////////////////
    //ADDING INTERPOLATION OF ELEVATION FOR NODE INSERTED ON RIVER SEGMENTS
    ///////////////////////////////////////////////////////////////////////
    mesh.close(); ele.close(); node.close(); neigh.close();
    QString shpFile = rivFileLineEdit->text();
    QString dbfFile = shpFile; dbfFile.truncate(dbfFile.length()-3); dbfFile.append("dbf");
    QString eleFile = eleFileLineEdit->text();
    QString nodeFile= nodeFileLineEdit->text();
    QString neighFile=neighFileLineEdit->text();
    cout<<"\n"<<qPrintable(meshFile1);
    cout<<"\n"<<qPrintable(meshFile0);
    interpRiverNodes(qPrintable(shpFile), qPrintable(dbfFile), qPrintable(eleFile), qPrintable(nodeFile), qPrintable(neighFile), qPrintable(meshFile1), qPrintable(meshFile0));
  }
}

void mshFileDlg::on_rivFileButton_clicked()
{

}

void mshFileDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Mesh File", 1, "helpFiles/meshfile.html", "Help :: Mesh File");
  hlpDlg->show();
}

