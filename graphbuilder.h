#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QString>
#include <QDebug>
#include <vector>
#include "iaudiodata.h"

QT_CHARTS_USE_NAMESPACE

class GraphBuilder : public QChartView
{
    Q_OBJECT
public:
    explicit GraphBuilder(QWidget *parent = 0);

    void plot(const IAudioData &data, const QString &name);
    //TODO: zoom options other than rubberband

private:
};

#endif // GRAPHBUILDER_H
