#include "runnalldomain.h"
#include "ui_runnalldomain.h"
#include <QtGui>

#include <qgsproject.h>
#include "../../pihmLIBS/generatePolyFile.h"
#include "../../pihmLIBS/createTinShapeFile.h"

#include "../../pihmLIBS/helpDialog/helpdialog.h"

//QString projDir = "/Users/bhattgopal/Documents";

RunnAllDomain::RunnAllDomain(QWidget *parent)
  : QDialog(parent), ui(new Ui::RunnAllDomain)
{
  ui->setupUi(this);

  connect( &mProcess, SIGNAL( readyReadStandardOutput() ), this, SLOT( stdoutAvailable() ) );
  connect( &mProcess, SIGNAL( readyReadStandardError() ), this, SLOT( stderrAvailable() ) );

  QgsProject *p = QgsProject::instance();
  ui->lineEditConstrain->setText(p->readPath(p->readEntry("pihm", "merge"))); //40
  double tmp;
  bool ok;
  tmp = p->readDoubleEntry("pihm", "/triangle/angle", 0, &ok); //43
  if (ok) {
    ui->lineEditAngle->setText(QString::number(tmp));
    ui->checkBoxAngle->setChecked(true);
  } else
    ui->lineEditAngle->hide();
  tmp = p->readDoubleEntry("pihm", "/triangle/area", 0, &ok); //44
  if (ok) {
    ui->lineEditArea->setText(QString::number(tmp));
    ui->checkBoxArea->setChecked(true);
  } else
    ui->lineEditArea->hide();
  QString s = p->readEntry("pihm", "/triangle/other");
  if (!s.isEmpty())
    ui->lineEditOthers->setText(s);  // 45
  else
    ui->lineEditOthers->hide();
}

RunnAllDomain::~RunnAllDomain()
{
  delete ui;
}

void RunnAllDomain::on_pushButtonConstrain_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  //tout<<ui->lineEditFolder->text()<<"\n";
  //tout<<ui->lineEditFile->text()<<"\n";

  QString s = QFileDialog::getOpenFileName(this, "Choose Constrain Shape File File", projDir, "Shape (*.shp *.SHP)");
  if (!s.isNull()) {
    ui->lineEditConstrain->setText(s);

    QString mesh=projDir+"/DomainDecomposition/mesh.shp";
    ui->lineEditMesh->setText(mesh);
  }
}

void RunnAllDomain::on_pushButtonMesh_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  QString s = QFileDialog::getSaveFileName(this, "Save Mesh File to...", projDir, "Shape file (*.shp)");
  if (!s.isNull()) {
    if(!s.endsWith(".shp"))
      s.append(".shp");
    ui->lineEditMesh->setText(s);
  }
}

void RunnAllDomain::on_pushButtonRun_clicked()
{
  QgsProject *p = QgsProject::instance();
  QString projDir = p->readPath(p->readEntry("pihm", "projDir"));

  //TODO
  //QString cmd = "cd "+projDir+";"+"mkdir DomainDecomposition";
  //system(qPrintable(cmd));
  QString polyStr = ui->lineEditConstrain->text();
  cout<<"here: "<<qPrintable(polyStr)<<"\n";
  polyStr=polyStr.right(polyStr.length()-polyStr.lastIndexOf("/",-1));
  polyStr.truncate(polyStr.length()-4);
  cout<<"here: >"<<qPrintable(polyStr)<<"<\n";
  QString polyFile = projDir+"/DomainDecomposition/"+polyStr+".poly";
  QDir dir = QDir::home();
  QString home = dir.homePath();
  QString logFileName(qPrintable(home+"/log.html"));
  generatePolyFile(qPrintable(ui->lineEditConstrain->text()), qPrintable(polyFile), qPrintable(logFileName));

  p->writeEntry("pihm", "merge", p->writePath(ui->lineEditConstrain->text())); // 40
  p->writeEntry("pihm", "pslg", p->writePath(polyFile)); // 41

  QString exeString;  exeString = "triangle";
  QString optString;  optString = "-nV";
  QString fileString; fileString= polyFile;
  if(ui->checkBoxAngle->isChecked()) {optString.append("q"); optString.append(ui->lineEditAngle->text()); }
  if(ui->checkBoxArea->isChecked()) {optString.append("a"); optString.append(ui->lineEditArea->text()); }
  if(ui->checkBoxOthers->isChecked()) {optString.append(ui->lineEditOthers->text()); }

  QString action("%1 %2 %3");
  action = action.arg(exeString, optString, polyFile);
  ui->textBrowser->setTextFormat(Qt::LogText);
  ui->textBrowser->append("<h4>Running " + action + "</h4>");
  mProcess.start(action);
  while (!mProcess.waitForFinished(1000)) {
    QApplication::processEvents();
  }
  //mProcess.waitForFinished(-1); // GUI will freeze, but triangle should work fast
  ui->textBrowser->append(QString("<h4>Finished with exit code %1</h4>").arg(mProcess.exitCode()));

  p->writeEntry("pihm", "pslg", p->writePath(polyFile)); // 42
  double tmp;
  bool ok;
  tmp = ui->lineEditAngle->text().toDouble(&ok);
  if (ui->checkBoxAngle->isChecked() && ok)
    p->writeEntry("pihm", "/triangle/angle", tmp);  // 43
  else
    p->removeEntry("pihm", "/triangle/angle");
  tmp = ui->lineEditArea->text().toDouble(&ok);
  if (ui->checkBoxArea->isChecked() && ok)
    p->writeEntry("pihm", "/triangle/area", tmp);  // 44
  else
    p->removeEntry("pihm", "/triangle/area");
  p->writeEntry("pihm", "/triangle/other", ui->lineEditOthers->text()); // 45

  QString eleFile;  eleFile  = projDir+"/DomainDecomposition/"+polyStr+".1.ele";
  QString nodeFile; nodeFile = projDir+"/DomainDecomposition/"+polyStr+".1.node";
  QString meshFileShp; meshFileShp = ui->lineEditMesh->text();
  QString meshFileDbf; meshFileDbf = meshFileShp;
  meshFileDbf.truncate(meshFileDbf.length()-3);
  meshFileDbf.append("dbf");
  createTinShapeFile(qPrintable(eleFile), qPrintable(nodeFile), qPrintable(meshFileShp), qPrintable(meshFileDbf), qPrintable(logFileName));

  p->writeEntry("pihm", "ele", p->writePath(eleFile)); // 46
  p->writeEntry("pihm", "node", p->writePath(nodeFile)); // 47
  p->writeEntry("pihm", "TIN", p->writePath(meshFileShp)); // 48
}

void RunnAllDomain::on_pushButtonClose_clicked()
{
  close();
}

void RunnAllDomain::on_pushButtonHelp_clicked()
{
  helpDialog* hlpDlg = new helpDialog(this, "Run All DomainDecomposition", 1, "helpFiles/runalldomain.html", "Help :: Run All Domain Decomposition");
  hlpDlg->show();
}

void RunnAllDomain::stdoutAvailable()
{
  QString line( mProcess.readAllStandardOutput() );
  ui->textBrowser->append(line);
}

void RunnAllDomain::stderrAvailable()
{
  QString line( mProcess.readAllStandardError() );
  ui->textBrowser->append(line);
}
