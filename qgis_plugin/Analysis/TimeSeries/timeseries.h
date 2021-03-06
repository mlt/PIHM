#ifndef TIMESERIES
#define TIMESERIES

#include "ui_timeseries.h"

class PlotTS;
class timeSeriesDlg : public QDialog, private Ui::timeSeriesDialog
{
Q_OBJECT

PlotTS *plotts;
QRegExp rx;   //!< Regular expression for parsing id string

public:
timeSeriesDlg();

public slots:
void browse();
void plot();
void savePlot();
void help();


private slots:
void on_pushButtonClose_clicked();
void on_comboBoxRivFeature_currentIndexChanged(int index);
void on_comboBoxEleFeature_currentIndexChanged(int index);
void on_comboBoxRivVariable_currentIndexChanged(int index);
void on_comboBoxEleVariable_currentIndexChanged(int index);
};

#endif
