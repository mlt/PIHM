#include "dissolve.h"
#include "ui_dissolve.h"
#include <QFileDialog>
#include <QString>
#include <QtGui>

#include <qgsproject.h>
#include "../../pihmLIBS/helpDialog/helpdialog.h"

#include "dissolve_h.h"

Dissolve::Dissolve(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dissolve)
{
  ui->setupUi(this);

  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));
  QString s;
  s = p->readPath(p->readEntry("pihm", "catpoly"));
  ui->lineEditPolygonInput->setText(s);
  s.truncate(s.length()-4); //truncate(s,4);
  s.append("_Dissolve.shp");
  ui->lineEditPolygonOutput->setText(s);

}

Dissolve::~Dissolve()
{
  delete ui;
}

void Dissolve::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void Dissolve::on_pushButtonInput_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString s = QFileDialog::getOpenFileName(this, "Choose Polygon Shape File File", projDir+"/VectorProcessing", "Shape (*.shp *.SHP)");
  ui->lineEditPolygonInput->setText(s);
  //QString ss=
  s.truncate(s.length()-4);          //truncate(s,4);
  s.append("_Dissolve.shp");
  ui->lineEditPolygonOutput->setText(s);
}

void Dissolve::on_pushButtonOutput_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString s = QFileDialog::getSaveFileName(this, "Choose Dissolve File Name", projDir+"/VectorProcessing", "Shape file (*.shp)");
  if(!s.endsWith(".shp"))
    s.append(".shp");
  ui->lineEditPolygonOutput->setText(s);
}

void Dissolve::on_pushButtonClose_clicked()
{
  close();
}

void Dissolve::on_pushButtonHelp_clicked()
{
  helpDialog* hlpDlg = new helpDialog(this, "Dissolve", 1, "helpFiles/dissolve.html", "Help :: Dissolve");
  hlpDlg->show();
}

void Dissolve::on_pushButtonRun_clicked()
{
  QDir dir = QDir::home();
  QString home = dir.homePath();
  QString logFileName(home+"/log.html");
  ofstream log;
  log.open(qPrintable(logFileName), ios::out);
  log<<"<html><body><br><font size=3 color=black><p> Running...";      //</font></body></html>";
  log.close();
  ui->textBrowser->setSource(logFileName);
  ui->textBrowser->setFocus();
  ui->textBrowser->setModified(TRUE);
  QApplication::processEvents();

  QgsProject *p = QgsProject::instance();
//	QString projDir = p->readEntry("pihm", "projDir");
  p->writeEntry("pihm", "catpoly", p->writePath(ui->lineEditPolygonInput->text())); // 22
  p->writeEntry("pihm", "catdiss", p->writePath(ui->lineEditPolygonOutput->text())); // 23

  QString inshp, indbf, outshp, outdbf;
  inshp =ui->lineEditPolygonInput->text();
  indbf =ui->lineEditPolygonInput->text();  indbf.truncate(indbf.length()-3); indbf.append("dbf");
  outshp=ui->lineEditPolygonOutput->text();
  outdbf=ui->lineEditPolygonOutput->text(); outdbf.truncate(outdbf.length()-3); outdbf.append("dbf");
  dissolve(qPrintable(inshp), qPrintable(indbf), qPrintable(outshp), qPrintable(outdbf));

  QString myFileNameQString = outshp;
  QFileInfo myFileInfo(myFileNameQString);
  QString myBaseNameQString = myFileInfo.baseName();
  QString provider = "OGR";
  applicationPointer->addVectorLayer(myFileNameQString, myBaseNameQString, "ogr");

  log.open(qPrintable(logFileName), ios::app);
  log<<" Done!";
  log.close();
  ui->textBrowser->reload();
  QApplication::processEvents();
}

void Dissolve::setApplicationPointer(QgisInterface* appPtr){
  applicationPointer = appPtr;
}
