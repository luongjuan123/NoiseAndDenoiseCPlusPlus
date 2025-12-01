#pragma once

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis> // Cần thêm cái này để chỉnh trục
#include <QVBoxLayout>
#include <vector>

// Namespace macro cho QtCharts
QT_CHARTS_USE_NAMESPACE

class ImFilter : public QWidget
{
    Q_OBJECT
public:
    explicit ImFilter(QWidget *parent = nullptr);
    void plotHistogram(const std::vector<float>& hist, const QString& title);

private:
    QChart *chart;
    QChartView *chartView;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
};