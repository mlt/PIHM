#include <QtGui>
#include <QApplication>

#include "runpihm.h"
#include <qgsproject.h>

#include <iomanip>
#include <fstream>
using namespace std;
#include "../pihmLIBS/helpDialog/helpdialog.h"

runPIHMDlg::runPIHMDlg(QWidget *parent)
  : rx("Tsteps = ([\\d.,]+)[^\\d.,]"), time(0.)
{
  setAttribute(Qt::WA_DeleteOnClose);
//	QPlastiqueStyle style();
  setupUi(this);
  connect(browseButton, SIGNAL(clicked()), this, SLOT(folderBrowse()));
  connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPIHM()));

  connect( &mProcess, SIGNAL( readyReadStandardOutput() ), this, SLOT( stdoutAvailable() ) );
  connect( &mProcess, SIGNAL( readyReadStandardError() ), this, SLOT( stderrAvailable() ) );
  connect( &mProcess, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( processExit( int, QProcess::ExitStatus ) ) );

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  lineEdit->setText(projDir+"/DataModel");
  lineEditFile->setText(p->readEntry("pihm", "ID")); // 50
}

runPIHMDlg::~runPIHMDlg() {
  cancelPIHM();
}

void runPIHMDlg::cancelPIHM()
{
  mProcess.kill();
//	cout<<"\n->"<<thread;
//	if(thread != NULL )
//	thread->kill();
//thread->wait();
//	done(0);
//	close();
}
void runPIHMDlg::folderBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QDir::setCurrent(projDir);
  QString folder = QFileDialog::getExistingDirectory(this, "Choose Input Directory", projDir);
  lineEdit->setText(folder);
}

void runPIHMDlg::run()
{
  if (QProcess::Running == mProcess.state()) { // dealing with (slow) GUI, no need to be atomic
    textBrowser->append("PIHM is already running");
    return;
  }
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  static int Number=0; int folderCreated=0; int runFlag = 1;
  QString dir, file; dir=lineEdit->text(); file=lineEditFile->text();
  QDir tempDir; tempDir.cd(lineEdit->text());
  QDir dummyDir; QString source, dest, newFolder;

  if(checkBox->isChecked()==1) {
    while(folderCreated==0) {
      newFolder=Number < 10 ? "Simul0"+QString::number(Number,10) : "Simul"+QString::number(Number,10); Number++;
      if(dummyDir.exists(lineEdit->text()+"/"+newFolder) == 0) {
        tempDir.mkdir(newFolder);
        folderCreated=1;

        source=dir+"/"+file+".mesh"; runFlag=QFile::exists(source) ? runFlag : 2; dest=dir+"/"+newFolder+"/"+file+".mesh"; QFile::copy(source, dest);
        source=dir+"/"+file+".att"; runFlag=QFile::exists(source) ? runFlag : 3; dest=dir+"/"+newFolder+"/"+file+".att"; QFile::copy(source, dest);
        source=dir+"/"+file+".riv"; runFlag=QFile::exists(source) ? runFlag : 4; dest=dir+"/"+newFolder+"/"+file+".riv"; QFile::copy(source, dest);
        source=dir+"/"+file+".para"; runFlag=QFile::exists(source) ? runFlag : 5; dest=dir+"/"+newFolder+"/"+file+".para"; QFile::copy(source, dest);
        source=dir+"/"+file+".soil"; runFlag=QFile::exists(source) ? runFlag : 6; dest=dir+"/"+newFolder+"/"+file+".soil"; QFile::copy(source, dest);
        source=dir+"/"+file+".geol"; runFlag=QFile::exists(source) ? runFlag : 7; dest=dir+"/"+newFolder+"/"+file+".geol"; QFile::copy(source, dest);
        source=dir+"/"+file+".lc"; runFlag=QFile::exists(source) ? runFlag : 8; dest=dir+"/"+newFolder+"/"+file+".lc"; QFile::copy(source, dest);
        source=dir+"/"+file+".calib"; runFlag=QFile::exists(source) ? runFlag : 9; dest=dir+"/"+newFolder+"/"+file+".calib"; QFile::copy(source, dest);
        source=dir+"/"+file+".init"; runFlag=QFile::exists(source) ? runFlag : 10; dest=dir+"/"+newFolder+"/"+file+".init"; QFile::copy(source, dest);
        source=dir+"/"+file+".ibc"; runFlag=QFile::exists(source) ? runFlag : 11; dest=dir+"/"+newFolder+"/"+file+".ibc"; QFile::copy(source, dest);
        source=dir+"/"+file+".forc"; runFlag=QFile::exists(source) ? runFlag : 12; dest=dir+"/"+newFolder+"/"+file+".forc"; QFile::copy(source, dest);
      }
    }
  }

  QString id = lineEditFile->text();
  QString fileID;
  if(checkBox->isChecked()==1)
    fileID = lineEdit->text() +"/"+ newFolder + "/" + id;
  else
    fileID = lineEdit->text() + "/" + id;


  QDir hdir = QDir::home();
  QString home = hdir.homePath();
  QString logFileName(home+"/log.html");
  ofstream log;
  log.open(qPrintable(logFileName));
  log<<"<html><body><font size=3 color=black><p> Running PIHM v2.0 ...<br>";      //</font></body></html>";
  log.close();
//		textBrowser->setTextFormat(Qt::LogText);
//		textBrowser->append("Running PIHM v2.0 ...");
  textBrowser->setSource(logFileName);
//        textBrowser->setFocus();
  //textBrowser->modified=TRUE;
  QApplication::processEvents();

  //pihm(2, nfile, progressBar);
  //MyNewThread*
  if(runFlag == 1) {
    ifstream paraFile; paraFile.open(qPrintable(fileID+".para"));
    double tempNum; int tempNumInt, tempScale=999;
    if(paraFile) {
      for(int ti=0; ti<23; ti++) paraFile>>tempNum;
      for(int tj=0; tj<9; tj++) { paraFile >>tempNumInt;
                                  QString tmp("step%1");
                                  p->writeEntry("pihm", tmp.arg(tj), tempNumInt); // 101+tj
                                  tempNumInt=(tempNumInt%1440==0 ? 2 : (tempNumInt % 60==0) ? 1 : 0);
                                  tempScale=tempScale<tempNumInt ? tempScale : tempNumInt; }
      //tempNumInt=tempNumInt%1440==0 ? 2 : (tempNumInt % 60==0) ? 1 : 0;
      //tempNumInt=tempNumInt<60?0: (tempNumInt<60*24 && tempNumInt%60!=0) ? 0 : tempNumInt%1440!=0 ? 1 : 2;
      p->writeEntry("pihm", "scale", tempScale); // 110

      for(int ti=0; ti<12; ti++) paraFile >> tempNum;
      paraFile >> start; p->writeEntry("pihm", "start", start); // 111
      paraFile >> finish; p->writeEntry("pihm", "finish", finish); // 112
    }
    QString action("pihm %1");
    action = action.arg(fileID);
    textBrowser->setTextFormat(Qt::LogText);
    textBrowser->append("<h4>Running " + action + "</h4>");
    mProcess.start(action);
  }
  else {
    QString s("Error: Input file(s) missing. Flag = %1");
    textBrowser->append(s.arg(runFlag));
//		cout<<"\nError: Input file(s) missing. Flag = "<<runFlag<<"\n";
  }
/*
  log.open(qPrintable(logFileName), ios::app);
  //log<<" Done!";
        log.close();
  textBrowser->reload();
*/
}

void runPIHMDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Run PIHM", 1, "helpFiles/runpihm.html", "Help :: Run PIHM");
  hlpDlg->show();
}

void runPIHMDlg::updateLog(int progress)
{
//	static int i=0;
//	qDebug("i=%d", i++);
  textBrowser->reload();
  textBrowser->scrollToBottom();
  progressBar->setValue(progress);
//	QApplication::processEvents();
}

void runPIHMDlg::UpdateProgress(int progress)
{
  progressBar->setValue(progress);
}

void runPIHMDlg::stdoutAvailable()
{
  /*
  QList<QByteArray> lines( mProcess.readAllStandardOutput().split(13) );
  foreach(const QString& line, lines) {
    if (finish!=start && -1 != rx.indexIn(line))
      time = rx.cap(1).toDouble();
    else
      textBrowser->append(line);
  }
  */
  QString line(mProcess.readAllStandardOutput());
  if (finish != start && -1 != rx.lastIndexIn(line)) {
    time = rx.cap(1).toDouble();
    progressBar->setValue(100.*(time-start)/(finish-start));
  }
}

void runPIHMDlg::stderrAvailable()
{
  QString line( mProcess.readAllStandardError() );
  textBrowser->append(line);
}

void runPIHMDlg::processExit( int code, QProcess::ExitStatus status) {
  switch(status) {
  case QProcess::NormalExit:
    textBrowser->append(QString("<h4>pihm finished with exit code %1</h4>").arg(code));
    break;
  case QProcess::CrashExit:
    textBrowser->append("<h4>pihm crashed</h4>");
  }
  progressBar->setValue(0);
}
