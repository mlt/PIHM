#include <qapplication.h>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_text.h>
#include <math.h>
#include <qdialog.h>
#include <qcolor.h>
#include "plotTS.h"
#include <iostream>

using namespace std;
/*
class PlotTS : public QwtPlot
{
public:
    PlotTS(const char*, const char*, const char*, const char*, double *, double *, int);
};
*/

PlotTS::PlotTS(const char *plotTitle, const char *xTitle, const char *yTitle, char **legend, double **xVal, double **yVal, int nPts, int NumTS)
  : _legend(legend),
  _xVal(xVal), _yVal(yVal), _nPts(nPts), _NumTS(NumTS)
{
//	setAttribute(Qt::WA_DeleteOnClose);

  //qWarning("class\n");
  QColor color(255, 255, 255);
  setCanvasBackground(color);
  setTitle(plotTitle);
  insertLegend(&theLegend, QwtPlot::RightLegend);

  // Set axis titles
  setAxisTitle(xBottom, xTitle);
  setAxisTitle(yLeft, yTitle);

  // Insert new curves
//	cSin = new QwtPlotCurve[NumTS];
//	lineColor = new int[NumTS];
//	qColor = new QColor[NumTS];
//	cSin = (QwtPlotCurve **)malloc(NumTS*sizeof(QwtPlotCurve *));
//	lineColor = (int *)malloc(NumTS*sizeof(int));
//	qColor = (QColor *)malloc(NumTS*sizeof(QColor));
  for (int i=0; i<NumTS; i++) {
    QwtPlotCurve* cSin = new QwtPlotCurve(); //legend[i]);
    cSin->setTitle(legend[i]);
#if QT_VERSION >= 0x040000
    cSin->setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
    //getchar(); getchar();
    int lineColor = qrand() % 17 + 2;
    QColor qColor = QColor::fromRgb((int) qrand()%255, (int) qrand()%255, (int) qrand()%255, 255);

    cout << "color = " << lineColor << "\n";
    //cSin[i]->setPen(QPen(QColor(lineColor[i])));
    cSin->setPen(QPen(qColor));
    //cSin[i]->setPen(QPen(qrand() % 17 + 2));
    cSin->setData(xVal[i], yVal[i], nPts);
    cSin->attach(this);
//		cSin->detach();
    // Create sin and cos data
    //const int nPoints = 100;
    //cSin->setData(SimpleData(::sin, nPoints));
    //cCos->setData(SimpleData(::cos, nPoints));

  }

  // Insert markers
  /*
   //  ...a horizontal line at y = 0...
   QwtPlotMarker *mY = new QwtPlotMarker();
   mY->setLabel(QString::fromLatin1("y = 0"));
   mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
   mY->setLineStyle(QwtPlotMarker::HLine);
   mY->setYValue(0.0);
   mY->attach(this);

   //  ...a vertical line at x = 2 * pi
   QwtPlotMarker *mX = new QwtPlotMarker();
   mX->setLabel(QString::fromLatin1("x = 2 pi"));
   mX->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
   mX->setLineStyle(QwtPlotMarker::VLine);
   mX->setXValue(6.284);
   mX->attach(this);
*/
  //parent->show();
  //qWarning("Done\n");
}

//PlotTS::~PlotTS() {
/*
for (int i=0; i<_NumTS; i++) {
//		cSin[i].detach();
  free(_xVal[i]);
  free(_yVal[i]);
  free(_legend[i]);
}
//    delete[] cSin;
delete[] lineColor;
delete[] qColor;
free(_legend);
  free(_xVal);
  free(_yVal);
*/
//}
