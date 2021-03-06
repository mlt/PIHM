#ifndef SIMPLIFYLINEDIALOG
#define SIMPLIFYLINEDIALOG

#include "ui_simplifylinedialog.h"
#include <qgisinterface.h>

class simplifyLineDialogDlg : public QDialog, private Ui::simplifyLineDialog
{
Q_OBJECT
public:
simplifyLineDialogDlg(QWidget *parent = 0);
void setApplicationPointer(QgisInterface *);
QgisInterface* applicationPointer;

public slots:
void inputBrowse();
void addBrowse();
void removeBrowse();
void removeAllBrowse();
void editTolerance();
void run();
void help();
};

#endif

