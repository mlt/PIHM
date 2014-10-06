/***************************************************************************
  pihmgis.cpp
  A Tightly Coupled GIS Interface to Penn State Integrated hydrologic Model (PIHM)
  -------------------
         begin                : [PluginDate]
         copyright            : [(C) Your Name and Date]
         email                : [Your Email]

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*  $Id: pihmgis.cpp 283 2011-07-18 06:15:42Z mlt $ */

//
// QGIS Specific includes
//
//#include "../../src/app/qgsabout.h"

#include <qgisinterface.h>
#include <qgisgui.h>
#include <qgsproject.h>

#include "pihmgis.h"

//
// Qt4 Related Includes
//
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QThread>
#include <QDesktopServices>
#include <QFileDialog>
#include <QUrl>
#include <QTextStream>

#include "Project/DefineProject/defineproject.h"

#include "RasterProcessing/RunAllRaster/runallraster.h"
#include "RasterProcessing/FillPits/fillpits.h"
#include "RasterProcessing/FlowGrid/flowgrid.h"
#include "RasterProcessing/StreamGrid/streamgrid.h"
#include "RasterProcessing/LinkGrid/linkgrid.h"
#include "RasterProcessing/StreamPolyLine/streampolyline.h"
#include "RasterProcessing/CatchmentGrid/catchmentgrid.h"
#include "RasterProcessing/CatchmentPolygon/catchmentpolygon.h"

#include "VectorProcessing/Dissolve/dissolve.h"
#include "VectorProcessing/PolygonToPolyLine/polygontopolylinedialog.h"
#include "VectorProcessing/SimplifyLine/simplifylinedialog.h"
#include "VectorProcessing/SplitLine/splitlinedialog.h"
#include "VectorProcessing/MergeFeatures/mergefeaturesdialog.h"

#include "DomainDecomposition/RunAllDomain/runnalldomain.h"
#include "DomainDecomposition/GenerateShapeTopology/generateshapetopology.h"
#include "DomainDecomposition/RunTriangle/runtriangle.h"
#include "DomainDecomposition/CreateTINs/createtins.h"

#include "DataModelLoader/MeshFile/mshfile.h"
#include "DataModelLoader/AttFile/attfile.h"
#include "DataModelLoader/RivFile/rivfile.h"
#include "DataModelLoader/ParaFile/parafile.h"
#include "DataModelLoader/SoilFile/soilfile.h"
#include "DataModelLoader/GeolFile/geolfile.h"
#include "DataModelLoader/LCFile/lcfile.h"
#include "DataModelLoader/CalibFile/calibfile.h"
#include "DataModelLoader/InitFile/initfile.h"
#include "DataModelLoader/IbcFile/ibcfile.h"

#include "RunPIHM/runpihm.h"
#include "Analysis/SpatialPlot/spatialplot.h"
#include "Analysis/TimeSeries/timeseries.h"

#include "Help/About/about.h"

#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif

static const char * const sIdent = "$Id: pihmgis.cpp 283 2011-07-18 06:15:42Z mlt $";
static const QString sName = QObject::tr("PIHMgis");
static const QString sDescription = QObject::tr("A Tightly Coupled GIS Interface to Penn State Integrated hydrologic Model (PIHM)");
static const QString sPluginVersion = QObject::tr("Version 0.1");
static const QgisPlugin::PLUGINTYPE sPluginType = QgisPlugin::UI;

//////////////////////////////////////////////////////////////////////
//
// THE FOLLOWING METHODS ARE MANDATORY FOR ALL PLUGINS
//
//////////////////////////////////////////////////////////////////////

/**
 * Constructor for the plugin. The plugin is passed a pointer
 * an interface object that provides access to exposed functions in QGIS.
 * @param theQGisInterface - Pointer to the QGIS interface object
 */
PIHMgis::PIHMgis(QgisInterface * theQgisInterface) :
  QgisPlugin(sName,sDescription,QString(),sPluginVersion,sPluginType),
  mQGisIface(theQgisInterface)
{
}

PIHMgis::~PIHMgis()
{

}

/*
 * Initialize the GUI interface for the plugin - this is only called once when the plugin is
 * added to the plugin registry in the QGIS application.
 */
void PIHMgis::initGui()
{
  QAction *projAction = new QAction("&New", this);
  connect(projAction, SIGNAL(triggered()), this, SLOT(runDefineProject()));
  QMenu* projMenu = new QMenu("&Import");
  projMenu->addAction("Import *.pihmgis", this, SLOT(runImportProject()));
  projAction->setMenu(projMenu);
  mQGisIface->addToolBarIcon(projAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QMenu* rastMenu = new QMenu("&Raster-Processing"); //, mQGisIface->app());
  rastMenu->addAction("Run All", this, SLOT(runRunAllRaster()));
  rastMenu->addSeparator();
//      rastMenu->addAction(QIcon(":/pihmgis/pihmgis.png"), "Fill Pits", this, SLOT(runFillPits()));
  rastMenu->addAction( "Fill Pits", this, SLOT(runFillPits()));
  rastMenu->addAction( "Flow Grid", this, SLOT(runFlowGrid()));
  rastMenu->addAction( "Stream Grid", this, SLOT(runStreamGrid()));
  rastMenu->addAction( "Link Grid", this, SLOT(runLinkGrid()));
  rastMenu->addAction( "Stream Polyline", this, SLOT(runStreamPolyLine()));
  rastMenu->addAction( "Catchment Grid", this, SLOT(runCatchmentGrid()));
  rastMenu->addAction( "Catchment Polygon", this, SLOT(runCatchmentPolygon()));

  QAction *rastAction = new QAction(" Raster Processing ", this);
  rastAction->setMenu(rastMenu);
  mQGisIface->addToolBarIcon(rastAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QMenu* vectMenu = new QMenu("&Vector-Processing"); //, mQGisIface->app());
  QAction* vp0 = vectMenu->addAction( "Run All", this, SLOT(runDoNothing())); vp0->setDisabled(1);
  vectMenu->addSeparator();
  //vectMenu->addAction( "Input Files", this, SLOT(runShpFileInput()));
  vectMenu->addAction( "Dissolve Polygon", this, SLOT(runDissolve()));
  vectMenu->addAction( "Polygon To Line", this, SLOT(runPolygonToPolyline()));
  vectMenu->addAction( "Simplify Line", this, SLOT(runSimplifyLine()));
  vectMenu->addAction( "Split Line", this, SLOT(runSplitLine()));
  vectMenu->addAction( "Vector Merge", this, SLOT(runVectorMerge()));

  QAction *vectAction = new QAction(" Vector Processing ", this);
  vectAction->setMenu(vectMenu);
  mQGisIface->addToolBarIcon(vectAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QMenu* domainMenu = new QMenu("&Domain-Decomposition"); //??, mQGisIface->app());
  domainMenu->addAction( "Run All", this, SLOT(runRunAllDomain()));
  domainMenu->addSeparator();
  domainMenu->addAction( "Read ShapeTopology", this, SLOT(runGenerateShapeTopology()));
  domainMenu->addAction( "Run Triangle", this, SLOT(runRunTriangle()));
  domainMenu->addAction( "TIN Generation", this, SLOT(runCreateTINs()));

  QAction *domainAction = new QAction("Domain Decomposition", this);
  domainAction->setMenu(domainMenu);
  mQGisIface->addToolBarIcon(domainAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QMenu* xtractMenu = new QMenu("&Extract-Data"); //??, mQGisIface->app());
  QAction* xt0 = xtractMenu->addAction( "Run All", this, SLOT(runMshFile())); xt0->setDisabled(1);
  xtractMenu->addSeparator();
  xtractMenu->addAction( "Generate MeshFile", this, SLOT(runMshFile()));
  xtractMenu->addAction( "Generate AttFile", this, SLOT(runAttFile()));
  xtractMenu->addAction( "Generate RivFile", this, SLOT(runRivFile()));
  xtractMenu->addAction( "Generate ParaFile", this, SLOT(runParaFile()));
  xtractMenu->addAction( "Generate SoilFile", this, SLOT(runSoilFile()));
  xtractMenu->addAction( "Generate GeolFile", this, SLOT(runGeolFile()));
  xtractMenu->addAction( "Generate LCFile",   this, SLOT(runLCFile()));
  xtractMenu->addAction( "Generate CalibFile", this,SLOT(runCalibFile()));
  xtractMenu->addAction( "Generate InitFile", this, SLOT(runInitFile()));
  xtractMenu->addAction( "Generate IBCFile", this, SLOT(runIBCFile()));
  QAction* forc = xtractMenu->addAction( "Generate ForcFile", this, SLOT(runForcFile())); forc->setDisabled(1);
  QAction *xtractAction = new QAction("&DataModel Loader", this);
  xtractAction->setMenu(xtractMenu);
  mQGisIface->addToolBarIcon(xtractAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QAction *pihmAction = new QAction("Run &PIHM...", this);
  connect(pihmAction, SIGNAL(triggered()), this, SLOT(runPIHM()));
  mQGisIface->addToolBarIcon(pihmAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QMenu* analysisMenu = new QMenu("&Analysis"); //??, mQGisIface->app());
  analysisMenu->addAction("TimeSeries Plots", this, SLOT(runTimeSeriesPlots()));
  analysisMenu->addAction("Spatial Plots", this, SLOT(runSpatialPlots()));

  QAction *analysisAction = new QAction("      InfoViz      ", this);
  analysisAction->setMenu(analysisMenu);
  mQGisIface->addToolBarIcon(analysisAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  QMenu* helpMenu = new QMenu("&HELP"); //??, mQGisIface->app());
  helpMenu->addAction( "Online Help", this, SLOT(runOnlineHelp()));
  helpMenu->addAction( "PIHMgis Homepage", this, SLOT(runHomePage()));
  helpMenu->addAction( "About", this, SLOT(runAbout()));
  QAction *helpAction = new QAction("      Help      ", this);
  helpAction->setMenu(helpMenu);
  mQGisIface->addToolBarIcon(helpAction);
/////////////////////////////////////////////////////////////////////////////////////////////////////////



}
//method defined in interface
void PIHMgis::help()
{
  //implement me!
}

// Unload the plugin by cleaning up the GUI
void PIHMgis::unload()
{
  // remove the GUI
  //mQGisIface->removePluginMenu("&PIHMgis",mQActionPointer);
  //mQGisIface->removeToolBarIcon(mQActionPointer);
  //delete mQActionPointer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PIHMgis::runDefineProject(){
  DefineProject *Dlg = new DefineProject;
  //Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runImportProject(){
  QString projFile = QFileDialog::getOpenFileName(NULL, "Choose pihmgis project file to import", QString::null, "PIHM GIS project (*.pihmgis)");
  if (!projFile.isEmpty()) {
  typedef enum {Skip, Vector, Raster, Path, String, Int, Double} What;
  typedef struct {
    What what;
    const char* prop;
  } Action;
  Action lines[] = {  //{Skip, "/model/mesh" }; // Excel semi-generated
    {Skip, NULL},   // date
    { Skip, "projDir"},   // line 2,
    { Raster, "dem" },   // line 3, source DEM
    { Raster, "fill" },   // line 4, sink filled DEM
    { Skip, "fill" },   // line 5,
    { Raster, "fdr" },   // line 6,
    { Raster, "fac" },   // line 7,
    { Skip, "fac" },   // line 8, See 7
    { Raster, "strgrid" },   // line 9, Streams raster
    { Int, "facThreshold" },   // line 10, Threshold for stream acc
    { Skip, "strgrid" },   // line 11, See 9
    { Skip, "fdr" },   // line 12, See 6
    { Raster, "lnk" },   // line 13,
    { Skip, "strgrid" },   // line 14, See 11
    { Skip, "fdr" },   // line 15, See 6
    { Vector, "strline" },   // line 16, Streams shapefile
    { Skip, "lnk" },   // line 17,
    { Skip, "fdr" },   // line 18,
    { Raster, "catgrid" },   // line 19,
    { Skip, "catgrid" },   // line 20,
    { Vector, "catpoly" },   // line 21,
    { Skip, "catpoly" },   // line 22, See 21
    { Vector, "catdiss" },   // line 23,
    { Skip, "catdiss" },   // line 24, See 23
    { Vector, "catline" },   // line 25,
    { Skip, "catline" },   // line 26,
    { Vector, "catsimple" },   // line 27,
    { Double, "cattol" },   // line 28, line simplification tolerance for diss polylines?
    { Vector, "strline" },   // line 29,
    { Vector, "strsimple" },   // line 30,
    { Double, "strtol" },   // line 31, line simplification tolerance for streams?
    { Skip, "catsimple" },   // line 32,
    { Vector, "catsplit" },   // line 33,
    { Skip, "strsimple" },   // line 34,
    { Vector, "strsplit" },   // line 35,
    { Skip, "catsplit" },   // line 36, See 33
    { Skip, "strsplit" },   // line 37, See 35
    { Vector, "merge" },   // line 38,
    { Skip, "merge" },   // line 39,
    { Skip, "merge" },   // line 40,
    { Path, "pslg" },   // line 41, Planar Straight Line Graph
    { Skip, "pslg" },   // line 42,
    { Double, "triangle/angle" },   // line 43, triangulation parameter
    { Double, "triangle/area" },   // line 44, triangulation parameter
    { String, "triangle/other" },   // line 45, triangulation parameter
    { Path, "ele" },   // line 46,
    { Path, "node" },   // line 47,
    { Vector, "TIN" },   // line 48, TIN/ele shp file
    { Path, "/model/mesh" },   // line 49,
    { String, "ID" },   // line 50, file ID
    { Skip, NULL },   // line 51,
    { Skip, NULL },   // line 52,
    { Skip, NULL },   // line 53,
    { Path, "Precip" },   // line 54,
    { Path, "Temp" },   // line 55,
    { Path, "Humid" },   // line 56,
    { Path, "Wind" },   // line 57,
    { Path, "G" },   // line 58,
    { Path, "Rn" },   // line 59,
    { Path, "P" },   // line 60,
    { Path, "Soil" },   // line 61,
    { Path, "Geol" },   // line 62,
    { Path, "LC" },   // line 63,
    { Path, "MF" },   // line 64,
    { Path, "MP" },   // line 65,
    { Path, "ISIC" },   // line 66,
    { Path, "SnowIC" },   // line 67,
    { Path, "OverlandIC" },   // line 68,
    { Path, "UnSatIC" },   // line 69,
    { Path, "SatIC" },   // line 70,
    { Path, "BC" },   // line 71,
    { Path, "Source" },   // line 72,
    { Skip, NULL },   // line 73,
    { Path, "/model/att" },   // line 74,
    { Skip, "strsplit" },   // line 75, input stream
    { Skip, "ele" },   // line 76,
    { Skip, "node" },   // line 77,
    { Path, "neigh" },   // line 78,
    { Skip, NULL },   // line 79,
    { Path, "/model/riv" },   // line 80, .riv file
    { Path, "rivdec" },   // line 81, For spatial plots
    { Path, "/model/para" },   // line 82,
    { Skip, NULL },   // line 83,
    { Skip, NULL },   // line 84,
    { Skip, NULL },   // line 85,
    { Skip, NULL },   // line 86,
    { Path, "soil" },   // line 87, in
    { Path, "/model/soil" },   // line 88, out
    { Path, "geol" },   // line 89, in
    { Path, "/model/geol" },   // line 90, out
    { Path, "nlcd" },   // line 91, NLCD source?
    { Path, "/model/lc" },   // line 92,
    { Path, "/model/calib" },   // line 93,
    { Path, "/model/init" },   // line 94,
    { Skip, NULL },   // line 95,
    { Skip, NULL },   // line 96,
    { Skip, NULL },   // line 97,
    { Skip, NULL },   // line 98,
    { Skip, NULL },   // line 99,
    { Double, "res" },   // line 100, DEM resolution
    { Double, "step0" },   // line 101, prep steps, adjustments
    { Double, "step1" },   // line 102,
    { Double, "step2" },   // line 103,
    { Double, "step3" },   // line 104,
    { Double, "step4" },   // line 105,
    { Double, "step5" },   // line 106,
    { Double, "step6" },   // line 107,
    { Double, "step7" },   // line 108,
    { Double, "step8" },   // line 109, adjustments
    { Double, "scale" },   // line 110,
    { Double, "start" },   // line 111, from .para file
    { Double, "finish" },   // line 112, from .para file
    { Skip, NULL },   // line 113,
    { Skip, NULL },   // line 114,
    { Skip, NULL },   // line 115,
    { Skip, NULL },   // line 116,
    { Skip, NULL },   // line 117,
    { Skip, NULL },   // line 118,
    { Skip, NULL },   // line 119,
    { Skip, NULL },   // line 120,
    { Skip, NULL },   // line 121,
  };
  int lines_size = sizeof(lines)/sizeof(lines[0]);
  QFile file(projFile);
  if (file.open(QFile::ReadOnly)) {
    QTextStream in(&file);
    in.readLine();   // date
    QString oldDir = in.readLine();
    QRegExp rx("[\\/]"); // file could have been created on other platform, can't rely on QFileInfo
    qDebug("oldDir = %s", qPrintable(oldDir));
    oldDir = oldDir.left(oldDir.lastIndexOf(rx));
    int oldDirLength = oldDir.length();
    qDebug("oldDir = %s, oldDirLength = %d", qPrintable(oldDir), oldDirLength);
    QFileInfo fi(projFile);
    QString projDir = fi.absoluteDir().absPath();
    qDebug("new projDir is %s", qPrintable(projDir));
    QgsProject *p = QgsProject::instance();
    QString s = projFile.left(projFile.length() - 7) + "qgs";
    p->setFileName(s);   // instead of pihmgis
    p->writeEntry("Paths", "/Absolute", false);
    p->writeEntry("pihm", "projDir", QString("."));
    bool ok;
    double d;
    int i;
    for( int idx=2; idx<lines_size && !in.atEnd(); ++idx) {
      s = in.readLine();
      switch (lines[idx].what) {
      case Int:
        i = s.toInt(&ok);
        if (ok)
          p->writeEntry("pihm", lines[idx].prop, i);
        break;
      case Double:
        d = s.toDouble(&ok);
        if (ok)
          p->writeEntry("pihm", lines[idx].prop, d);
        break;
      case Path:
      {
        qDebug("Path Was %s", qPrintable(s));
        s = ".." + s.mid(oldDirLength);
        qDebug("Truncated version is %s", qPrintable(s));
        QFileInfo fi(projDir, s);
        s = p->writePath(fi.canonicalFilePath());
        qDebug("Became %s", qPrintable(s));
      }       // no break!! Don't move
      case String:
        p->writeEntry("pihm", lines[idx].prop, s);
        break;
      case Raster:
      {
        qDebug("Raster Was %s", qPrintable(s));
        s = ".." + s.mid(oldDirLength);
        qDebug("Truncated version is %s", qPrintable(s));
        QFileInfo fi(projDir, s);
        s = fi.canonicalFilePath();
        qDebug("Became %s", qPrintable(s));
      }
        p->writeEntry("pihm", lines[idx].prop, p->writePath(s));
        mQGisIface->addRasterLayer(s);
        break;
      case Vector:
      {
        qDebug("Vector Was %s", qPrintable(s));
        QFileInfo fi(projDir, ".." + s.mid(oldDirLength));
        s = fi.canonicalFilePath();
        qDebug("Became %s", qPrintable(s));
      }
        p->writeEntry("pihm", lines[idx].prop, p->writePath(s));
        mQGisIface->addVectorLayer(s, QString::null, "ogr");
        break;
      }
    }
  }
  }
}

void PIHMgis::runRunAllRaster(){
  RunAllRaster* Dlg = new RunAllRaster;
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runFillPits(){
  fillpitsDlg* Dlg = new fillpitsDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
  //??Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runFlowGrid(){
  FlowGridDlg* Dlg = new FlowGridDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
  //??Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runStreamGrid(){
  StreamGridDlg* Dlg = new StreamGridDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
  //??Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runLinkGrid(){
  LinkGridDlg* Dlg = new LinkGridDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
//??        Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runStreamPolyLine(){
  StreamPolyLineDlg* Dlg = new StreamPolyLineDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
//??        Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runCatchmentGrid(){
  CatchmentGridDlg* Dlg = new CatchmentGridDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
//??        Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runCatchmentPolygon(){
  CatchmentPolygonDlg* Dlg = new CatchmentPolygonDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  Dlg->setApplicationPointer(mQGisIface);      //->app());
//??        Dlg->setApplicationPointer(mQGisApp);
  Dlg->show();
}

void PIHMgis::runDissolve(){
  Dissolve* Dlg = new Dissolve;
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runPolygonToPolyline(){
  polygonToPolyLineDialogDlg* Dlg = new polygonToPolyLineDialogDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runSimplifyLine(){
  simplifyLineDialogDlg* Dlg = new simplifyLineDialogDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runSplitLine(){
  splitLineDialogDlg* Dlg = new splitLineDialogDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runVectorMerge(){
  mergeFeaturesDialogDlg* Dlg = new mergeFeaturesDialogDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runRunAllDomain(){
  RunnAllDomain* Dlg = new RunnAllDomain;
  Dlg->show();
}

void PIHMgis::runGenerateShapeTopology(){
  generateShapeTopologyDlg* Dlg = new generateShapeTopologyDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->show();
}

void PIHMgis::runRunTriangle(){
  runTriangleDlg* Dlg = new runTriangleDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->show();
}

void PIHMgis::runCreateTINs(){
  createTINsDlg* Dlg = new createTINsDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runMshFile(){
  mshFileDlg* Dlg = new mshFileDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->show();
}

void PIHMgis::runAttFile(){
  attFileDlg* Dlg = new attFileDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->show();
}

void PIHMgis::runRivFile(){
  rivFileDlg* Dlg = new rivFileDlg;
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runParaFile(){
  paraFileDlg* Dlg = new paraFileDlg;       //(NULL,NULL,TRUE,0); //,"/opt3/helpDialog/poly.html","Help Caption");
  //Dlg->setApplicationPointer(mQGisIface->app());
  Dlg->show();
}

void PIHMgis::runSoilFile(){
  SoilFile* Dlg = new SoilFile;
  Dlg->show();
}

void PIHMgis::runGeolFile(){
  GeolFile* Dlg = new GeolFile;
  Dlg->show();
}

void PIHMgis::runLCFile(){
  LCFile* Dlg = new LCFile;
  Dlg->show();
}

void PIHMgis::runCalibFile(){
  CalibFile* Dlg = new CalibFile;
  Dlg->show();
}

void PIHMgis::runInitFile(){
  InitFile* Dlg = new InitFile;
  Dlg->show();
}

void PIHMgis::runIBCFile(){
  IbcFile* Dlg = new IbcFile;
  Dlg->show();
}

void PIHMgis::runForcFile(){

}


void PIHMgis::runPIHM(){
  runPIHMDlg* Dlg = new runPIHMDlg;
  Dlg->show();
  //MyNewThread* thread = new MyNewThread;
  //thread->start();
}

void PIHMgis::runSpatialPlots(){
  spatialPlotDlg* Dlg = new spatialPlotDlg;
//??        Dlg->setApplicationPointer(mQGisApp);
  Dlg->setApplicationPointer(mQGisIface);
  Dlg->show();
}

void PIHMgis::runTimeSeriesPlots(){
  timeSeriesDlg *Dlg = new timeSeriesDlg;
  Dlg->show();
}

void PIHMgis::runAbout(){
  AboutDlg* Dlg = new AboutDlg;
  Dlg->show();
}

void PIHMgis::runHomePage(){
  QUrl url("http://www.pihm.psu.edu");
  QDesktopServices::openUrl( url);
}

void PIHMgis::runOnlineHelp(){
  QUrl url("http://www.pihm.psu.edu/pihmgis_documents.html");
  QDesktopServices::openUrl( url);
}
//////////////////////////////////////////////////////////////////////////
//
//
//  THE FOLLOWING CODE IS AUTOGENERATED BY THE PLUGIN BUILDER SCRIPT
//    YOU WOULD NORMALLY NOT NEED TO MODIFY THIS, AND YOUR PLUGIN
//      MAY NOT WORK PROPERLY IF YOU MODIFY THIS INCORRECTLY
//
//
//////////////////////////////////////////////////////////////////////////


/**
 * Required extern functions needed  for every plugin
 * These functions can be called prior to creating an instance
 * of the plugin class
 */
// Class factory to return a new instance of the plugin class
QGISEXTERN QgisPlugin * classFactory(QgisInterface * theQgisInterfacePointer)
{
  return new PIHMgis(theQgisInterfacePointer);
}
// Return the name of the plugin - note that we do not user class members as
// the class may not yet be insantiated when this method is called.
QGISEXTERN QString name()
{
  return sName;
}

// Return the description
QGISEXTERN QString description()
{
  return sDescription;
}

// Return the type (either UI or MapLayer plugin)
QGISEXTERN int type()
{
  return sPluginType;
}

// Return the version number for the plugin
QGISEXTERN QString version()
{
  return sPluginVersion;
}

// Delete ourself
QGISEXTERN void unload(QgisPlugin * thePluginPointer)
{
  delete thePluginPointer;
}
