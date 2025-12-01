#include "imfilter.h"

ImFilter::ImFilter(QWidget *parent) : QWidget(parent) {
    chart = new QChart();
    chart->setBackgroundBrush(QBrush(Qt::black));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->hide(); // Ẩn chú thích nếu không cần

    series = new QLineSeries();
    chart->addSeries(series);

    // Thiết lập trục
    axisX = new QValueAxis();
    axisX->setRange(0, 255);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Intensity");
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleBrush(Qt::white);

    axisY = new QValueAxis();
    axisY->setTitleText("Frequency");
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(Qt::white);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    layout->setContentsMargins(0,0,0,0); // Xóa lề
    setLayout(layout);
}

void ImFilter::plotHistogram(const std::vector<float>& hist, const QString& title) {
    series->clear();
    
    float maxVal = 0;
    for (size_t i = 0; i < hist.size(); ++i) {
        series->append(i, hist[i]);
        if (hist[i] > maxVal) maxVal = hist[i];
    }
    
    axisY->setRange(0, maxVal); // Cập nhật trục Y theo dữ liệu mới
    chart->setTitle(title);
}