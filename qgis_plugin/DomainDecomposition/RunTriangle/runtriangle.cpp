#include <iostream>
#include <QtGui>
#include "runtriangle.h"
#include <fstream>
#include "../../pihmLIBS/helpDialog/helpdialog.h"

#include <qgsproject.h>
#include <qgsrunprocess.h>

#include <fstream>

using namespace std;

runTriangleDlg::runTriangleDlg(QWidget *parent)
{
  setupUi(this);
  connect(polyFileBrowseButton, SIGNAL(clicked()), this, SLOT(inputBrowse()));
  connect(showOptionsButton, SIGNAL(clicked()), this, SLOT(options()));
  connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
  connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  nOptionCheckBox->setCheckState(Qt::Checked);
  VOptionsCheckBox->setCheckState(Qt::Checked);

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  polyFileLineEdit->setText(p->readPath(p->readEntry("pihm", "pslg"))); // 41
  bool ok;
  QString tmp;
  tmp = QString::number(p->readDoubleEntry("pihm", "/triangle/angle", 0, &ok)); // 43
  if(ok) {
    qOptionsCheckBox->setCheckState(Qt::Checked);
    qOptionLineEdit->setText(tmp);
  }
  tmp = QString::number(p->readDoubleEntry("pihm", "/triangle/area", 0, &ok)); // 44
  if(ok) {
    aOptionsCheckBox->setCheckState(Qt::Checked);
    aOptionLineEdit->setText(tmp);
  }
  tmp = p->readEntry("pihm", "/triangle/other"); // 45
  if(!tmp.isEmpty()) {
    otherOptionLineEdit->setText(tmp);
  }

}


void runTriangleDlg::inputBrowse()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString temp = QFileDialog::getOpenFileName(this, "Choose File", projDir+"/DomainDecomposition","Poly File(*.poly)");

  polyFileLineEdit->setText(temp);
}

void runTriangleDlg::options()
{
  helpDialog* hlpDlg = new helpDialog(this, "TRIANGLE Options", 1, "helpFiles/triangleoptions.html", "Help :: TRIANGLE Options");
  hlpDlg->show();
}

void runTriangleDlg::run()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  p->writeEntry("pihm", "pslg", p->writePath(polyFileLineEdit->text())); // 42
  double tmp;
  bool ok;
  tmp = qOptionLineEdit->text().toDouble(&ok);
  if (qOptionsCheckBox->isChecked() && ok)
    p->writeEntry("pihm", "/triangle/angle", tmp);  // 43
  else
    p->removeEntry("pihm", "/triangle/angle");
  tmp = aOptionLineEdit->text().toDouble(&ok);
  if (aOptionsCheckBox->isChecked() && ok)
    p->writeEntry("pihm", "/triangle/area", tmp);  // 44
  else
    p->removeEntry("pihm", "/triangle/area");
  p->writeEntry("pihm", "/triangle/other", otherOptionLineEdit->text()); // 45

  if (!QFile::exists(polyFileLineEdit->text()))
    QMessageBox::critical(this, "Input file doesn't exist", "Please specify input PSLG file");
  else {
    QString exeString("triangle");
    QString optString("-");
    if(nOptionCheckBox->isChecked()) {optString.append("n"); }
    if(VOptionsCheckBox->isChecked()) {optString.append("V"); }
    if(qOptionsCheckBox->isChecked()) {optString.append("q"); optString.append(qOptionLineEdit->text()); }
    if(aOptionsCheckBox->isChecked()) {optString.append("a"); optString.append(aOptionLineEdit->text()); }
    if(otherOptionCheckBox->isChecked()) {optString.append(otherOptionLineEdit->text()); }
    QString action("%1 %2 %3");
    action = action.arg(exeString, optString, polyFileLineEdit->text());
    QgsRunProcess *rp = QgsRunProcess::create(action, true);
    close();
  }
}

void runTriangleDlg::help()
{
  helpDialog* hlpDlg = new helpDialog(this, "Run TRIANGLE", 1, "helpFiles/runtriangle.html", "Help :: Run TRIANGLE");
  hlpDlg->show();
}
