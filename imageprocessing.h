#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <opencv2/opencv.hpp>

class ImageProcessing : public QMainWindow
{
    Q_OBJECT
public:
    explicit ImageProcessing(QWidget *parent = nullptr);

private slots:
    void openFile();
    void saveFile();
    
    // Slot xử lý chính
    void applyProcessing();
    
    // Slot cập nhật giao diện (Enable/Disable control)
    void updateUIState();
    
    // Slot hiển thị giá trị số bên cạnh slider
    void updateLabelValues();

private:
    // --- GUI Elements ---
    QLabel *labelImage;
    QLabel *labelFilter;
    
    // Noise Controls
    QComboBox *cboNoise;
    QSlider *hsMean;
    QSlider *hsVar;    // Variance
    QSlider *hsAmount; // Amount/Peak
    QLabel *lblMeanVal, *lblVarVal, *lblAmountVal;

    // Denoise Controls
    QComboBox *cboDenoise;
    QSpinBox *sbH;      // Tham số h (NL Means)
    QSpinBox *sbPatch;  // Template Window Size
    QSpinBox *sbSearch; // Search Window Size
    QSlider *hsWeight;  // Weight (hoặc Kernel Size)
    QSlider *hsSigma;   // Sigma
    QLabel *lblWeightVal, *lblSigmaVal;

    // [ĐÃ XÓA] cboEdge

    // --- Image Data ---
    cv::Mat imgOriginal;
    cv::Mat imgNoisy;
    cv::Mat imgProcessed;

    // --- Functions ---
    void setupUI(); 
    void displayImage(const cv::Mat &mat, QLabel *label);

    // Noise Algorithms
    cv::Mat computeNoise(const cv::Mat &src);
    cv::Mat gaussianNoise(const cv::Mat &src, double mean, double variance);
    cv::Mat saltPepperNoise(const cv::Mat &src, double amount);
    cv::Mat speckleNoise(const cv::Mat &src, double mean, double variance);

    // Denoise Algorithms
    cv::Mat computeDenoise(const cv::Mat &src);
};