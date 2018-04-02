#include "graphbuilder.h"
#include <algorithm>
#include <QGraphicsLayout>
#include <QLayout>
#include <QtCharts/QCategoryAxis>
#include <QtWidgets/QGraphicsTextItem>

GraphBuilder::GraphBuilder(QWidget *parent) :
    QChartView(nullptr, parent)
{

}

void GraphBuilder::plot(const IAudioData &data, const QString &name)
{
    //return if no data
    if (data.getData().size() == 0)
        return;

    QChart *prevChart = this->chart();

    QChart *chart = new QChart();
    QLineSeries *series = new QLineSeries(chart);


    double currX = 0.0;
    //20000 is an aproximate number of points that would be drawn
    int precision = ((int)data.getData().size()/20000)+1;
    for(size_t index =0; index < data.getData().size(); index += precision)
    {
        series->append(currX, data.getData()[index]);
        currX += data.getInterval()*precision;
    }
    series->setName("");

    //setting line style
    QPen pen(QRgb(0xaaaadd));
    pen.setWidth(1);
    series->setPen(pen);

    chart->addSeries(series);

    //setting up axis
    QCategoryAxis *axisY = new QCategoryAxis();

    QFont labelsFont;
    labelsFont.setPixelSize(12);
    axisY->setLabelsFont(labelsFont);

    axisY->setLinePen(Qt::NoPen);

    QBrush axisBrush(QRgb(0xbcbcbc));
    axisY->setLabelsBrush(axisBrush);
    axisY->setGridLineVisible(false);

    axisY->append("0", 0);

    double yMin = *std::min_element(data.getData().begin(), data.getData().end());
    double yMax = *std::max_element(data.getData().begin(), data.getData().end());
    axisY->setRange(yMin, yMax);

    chart->setAxisY(axisY, series);

    //setting up background
    QBrush brush(QRgb(0x777777));
    chart->setBackgroundBrush(brush);
    chart->setBackgroundRoundness(0);

    chart->legend()->hide();
    chart->setContentsMargins(0, 0, 0, 0);
    chart->layout()->setContentsMargins(0, 0, 0, 0);

    setChart(chart);
    setRubberBand(QChartView::HorizontalRubberBand);
    setRenderHint(QPainter::Antialiasing);

    //adding lables
    QGraphicsSimpleTextItem *nameLable = new QGraphicsSimpleTextItem(chart);
    nameLable->setPos(chart->size().width()/2, 6);

    nameLable->setBrush(axisBrush);
    nameLable->setText(name);

    delete prevChart;
}
