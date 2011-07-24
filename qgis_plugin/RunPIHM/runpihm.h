#ifndef RUNPIHM
#define RUNPIHM

#include <QProcess>
#include "ui_runpihm.h"
//#include "../MyNewThread.h"

class runPIHMDlg : public QDialog, private Ui::runPIHM
{
Q_OBJECT

QProcess mProcess;
QRegExp rx;
double start, finish, time;
public:
runPIHMDlg(QWidget *parent = 0);
~runPIHMDlg();
//	MyNewThread* thread;// = new MyNewThread;
public slots:
void folderBrowse();
void run();
void help();
void cancelPIHM();
void updateLog(int progress);
void UpdateProgress(int progress);
private slots:
void stdoutAvailable();
void stderrAvailable();
void processExit( int, QProcess::ExitStatus );
};

#endif
