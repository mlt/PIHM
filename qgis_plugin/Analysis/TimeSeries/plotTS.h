#include <qwt_plot.h>
#include <qwt_legend.h>

class QwtPlotCurve;

class PlotTS : public QwtPlot
{
//	QwtPlotCurve *cSin;
//	int* lineColor;
//	QColor* qColor;
char *_plotTitle, *_xTitle, *_yTitle;
char **_legend;
double **_xVal, **_yVal;
int _nPts, _NumTS;
QwtLegend theLegend;

public:
PlotTS(const char* title = 0, const char* xtitle = 0, const char* ytitle = 0, char** legend = 0, double **xval= NULL, double **yval=NULL, int npts=0, int NumTS=1);
//	~PlotTS();
};

