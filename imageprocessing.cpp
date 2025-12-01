#include "imageprocessing.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QtMath>
#include <random>

// OpenCV includes
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/photo.hpp> 

// Helper: Convert Mat to QImage
QImage matToQImage(const cv::Mat &mat) {
    if(mat.empty()) return QImage();
    if(mat.type() == CV_8UC3) {
        QImage img((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped(); 
    }
    else if(mat.type() == CV_8UC1) {
        QImage img((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return img.copy(); 
    }
    return QImage();
}

ImageProcessing::ImageProcessing(QWidget *parent) : QMainWindow(parent) {
    setupUI(); // Dựng giao diện
    
    // Kết nối tín hiệu cập nhật UI khi đổi loại thuật toán
    connect(cboNoise, &QComboBox::currentTextChanged, this, &ImageProcessing::updateUIState);
    connect(cboDenoise, &QComboBox::currentTextChanged, this, &ImageProcessing::updateUIState);
    
    // Kết nối Slider -> Cập nhật số -> Chạy xử lý
    QList<QSlider*> sliders = {hsMean, hsVar, hsAmount, hsWeight, hsSigma};
    for(auto s : sliders) {
        connect(s, &QSlider::valueChanged, this, &ImageProcessing::updateLabelValues);
        connect(s, &QSlider::sliderReleased, this, &ImageProcessing::applyProcessing); 
    }
    
    // Kết nối SpinBox -> Chạy xử lý
    QList<QSpinBox*> spins = {sbH, sbPatch, sbSearch};
    for(auto s : spins) {
        connect(s, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageProcessing::applyProcessing);
    }

    // Kết nối ComboBox -> Chạy xử lý
    connect(cboNoise, &QComboBox::currentTextChanged, this, &ImageProcessing::applyProcessing);
    connect(cboDenoise, &QComboBox::currentTextChanged, this, &ImageProcessing::applyProcessing);
    
    // Khởi tạo trạng thái ban đầu
    updateUIState();
    updateLabelValues();
}

void ImageProcessing::setupUI() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    
    // 1. Top Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnOpen = new QPushButton("Open Image", this);
    QPushButton *btnSave = new QPushButton("Save Result", this);
    connect(btnOpen, &QPushButton::clicked, this, &ImageProcessing::openFile);
    connect(btnSave, &QPushButton::clicked, this, &ImageProcessing::saveFile);
    btnLayout->addWidget(btnOpen);
    btnLayout->addWidget(btnSave);

    // 2. Image Display
    QHBoxLayout *imgLayout = new QHBoxLayout();
    labelImage = new QLabel("Original", this);
    labelFilter = new QLabel("Result", this);
    QString labelStyle = "border: 2px dashed #555; background-color: #222; color: #AAA; qproperty-alignment: AlignCenter;";
    labelImage->setStyleSheet(labelStyle); labelFilter->setStyleSheet(labelStyle);
    labelImage->setMinimumSize(400, 300); labelFilter->setMinimumSize(400, 300);
    imgLayout->addWidget(labelImage);
    imgLayout->addWidget(labelFilter);

    // 3. Controls Area
    QHBoxLayout *controlsLayout = new QHBoxLayout();

    // --- Group: Noise Parameters ---
    QGroupBox *gbNoise = new QGroupBox("Noise Parameters", this);
    gbNoise->setStyleSheet("QGroupBox { font-weight: bold; color: red; }");
    QVBoxLayout *noiseLayout = new QVBoxLayout(gbNoise);
    
    cboNoise = new QComboBox();
    cboNoise->addItems({"None", "Gaussian", "Salt & Pepper", "Speckle", "Poisson"});
    
    noiseLayout->addWidget(new QLabel("Choose Noise Type"));
    noiseLayout->addWidget(cboNoise);

    // Grid cho các Slider Noise
    QGridLayout *noiseGrid = new QGridLayout();
    hsMean = new QSlider(Qt::Horizontal); hsMean->setRange(0, 50);
    hsVar = new QSlider(Qt::Horizontal); hsVar->setRange(0, 100);
    hsAmount = new QSlider(Qt::Horizontal); hsAmount->setRange(0, 50);
    
    lblMeanVal = new QLabel("0.0"); lblMeanVal->setFixedWidth(30);
    lblVarVal = new QLabel("0.0"); lblVarVal->setFixedWidth(30);
    lblAmountVal = new QLabel("0.0"); lblAmountVal->setFixedWidth(30);

    noiseGrid->addWidget(new QLabel("Mean"), 0, 0);
    noiseGrid->addWidget(hsMean, 1, 0); noiseGrid->addWidget(lblMeanVal, 1, 1);
    
    noiseGrid->addWidget(new QLabel("Variance"), 2, 0);
    noiseGrid->addWidget(hsVar, 3, 0); noiseGrid->addWidget(lblVarVal, 3, 1);
    
    noiseGrid->addWidget(new QLabel("Amount/Peak"), 4, 0);
    noiseGrid->addWidget(hsAmount, 5, 0); noiseGrid->addWidget(lblAmountVal, 5, 1);
    
    noiseLayout->addLayout(noiseGrid);
    noiseLayout->addStretch();

    // --- Group: Denoising ---
    QGroupBox *gbDenoise = new QGroupBox("Denoising", this);
    gbDenoise->setStyleSheet("QGroupBox { font-weight: bold; color: red; }");
    QVBoxLayout *denoiseLayout = new QVBoxLayout(gbDenoise);
    
    cboDenoise = new QComboBox();
    cboDenoise->addItems({"None", "Non-Local Means Denoising", "Bilateral Filter", "Gaussian Blur", "Median Blur"});
    
    denoiseLayout->addWidget(new QLabel("Choose DeNoise Type"));
    denoiseLayout->addWidget(cboDenoise);

    // Dòng chứa h, patch, search (cho NL Means)
    QHBoxLayout *nlParamsLayout = new QHBoxLayout();
    sbH = new QSpinBox(); sbH->setRange(1, 50); sbH->setValue(10);
    sbPatch = new QSpinBox(); sbPatch->setRange(3, 21); sbPatch->setValue(7); sbPatch->setSingleStep(2);
    sbSearch = new QSpinBox(); sbSearch->setRange(3, 50); sbSearch->setValue(21); sbSearch->setSingleStep(2);
    
    nlParamsLayout->addWidget(new QLabel("h")); nlParamsLayout->addWidget(sbH);
    nlParamsLayout->addWidget(new QLabel("patch")); nlParamsLayout->addWidget(sbPatch);
    nlParamsLayout->addWidget(new QLabel("search")); nlParamsLayout->addWidget(sbSearch);
    denoiseLayout->addLayout(nlParamsLayout);

    // Grid cho Weight/Sigma
    QGridLayout *denoiseGrid = new QGridLayout();
    hsWeight = new QSlider(Qt::Horizontal); hsWeight->setRange(1, 100); hsWeight->setValue(10);
    hsSigma = new QSlider(Qt::Horizontal); hsSigma->setRange(1, 100); hsSigma->setValue(10);
    
    lblWeightVal = new QLabel("1.0"); lblWeightVal->setFixedWidth(30);
    lblSigmaVal = new QLabel("1.0"); lblSigmaVal->setFixedWidth(30);

    denoiseGrid->addWidget(new QLabel("weight / d / kernel"), 0, 0);
    denoiseGrid->addWidget(hsWeight, 1, 0); denoiseGrid->addWidget(lblWeightVal, 1, 1);
    
    denoiseGrid->addWidget(new QLabel("sigma / sigmaColor"), 2, 0);
    denoiseGrid->addWidget(hsSigma, 3, 0); denoiseGrid->addWidget(lblSigmaVal, 3, 1);
    
    denoiseLayout->addLayout(denoiseGrid);
    
    // [ĐÃ XÓA] GroupBox Edge Detection

    // Add groups to controls layout
    controlsLayout->addWidget(gbNoise);
    controlsLayout->addWidget(gbDenoise);

    // Finalize Layout
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(imgLayout);
    mainLayout->addLayout(controlsLayout);
    setCentralWidget(central);
    
    setWindowTitle("Noise & Denoise Lab - Optimized");
    resize(1000, 600);
}

// ------------------- UI Logic Functions -------------------

void ImageProcessing::updateUIState() {
    QString noiseType = cboNoise->currentText();
    
    // Logic bật tắt noise params
    bool isGaussian = (noiseType == "Gaussian" || noiseType == "Speckle");
    bool isSalt = (noiseType == "Salt & Pepper");
    
    hsMean->setEnabled(isGaussian);
    hsVar->setEnabled(isGaussian);
    hsAmount->setEnabled(isSalt);
    
    // Logic bật tắt denoise params
    QString denoiseType = cboDenoise->currentText();
    bool isNLMeans = (denoiseType == "Non-Local Means Denoising");
    bool isBilateral = (denoiseType == "Bilateral Filter");
    
    sbH->setEnabled(isNLMeans);
    sbPatch->setEnabled(isNLMeans);
    sbSearch->setEnabled(isNLMeans);
    
    hsWeight->setEnabled(!isNLMeans && denoiseType != "None"); 
    hsSigma->setEnabled(isBilateral); 
}

void ImageProcessing::updateLabelValues() {
    lblMeanVal->setText(QString::number(hsMean->value()));
    lblVarVal->setText(QString::number(hsVar->value()));
    lblAmountVal->setText(QString::number(hsAmount->value() / 100.0, 'f', 2));
    
    lblWeightVal->setText(QString::number(hsWeight->value()));
    lblSigmaVal->setText(QString::number(hsSigma->value()));
}

// ------------------- Core Processing -------------------

void ImageProcessing::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp)");
    if(fileName.isEmpty()) return;

    cv::Mat temp = cv::imread(fileName.toStdString());
    if(temp.empty()) return;
    
    // Resize nếu ảnh quá to
    if(temp.cols > 1024) {
        double scale = 1024.0 / temp.cols;
        cv::resize(temp, imgOriginal, cv::Size(), scale, scale);
    } else {
        imgOriginal = temp;
    }
    
    imgNoisy = imgOriginal.clone();
    imgProcessed = imgOriginal.clone();
    displayImage(imgOriginal, labelImage);
    applyProcessing();
}

void ImageProcessing::saveFile() {
    if(imgProcessed.empty()) return;
    QString fileName = QFileDialog::getSaveFileName(this, "Save", "", "PNG (*.png);;JPG (*.jpg)");
    if(!fileName.isEmpty()) cv::imwrite(fileName.toStdString(), imgProcessed);
}

void ImageProcessing::applyProcessing() {
    if(imgOriginal.empty()) return;
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // 1. Apply Noise
    imgNoisy = computeNoise(imgOriginal);
    
    // 2. Apply Denoise (input is Noisy Image)
    imgProcessed = computeDenoise(imgNoisy);
    
    // [ĐÃ XÓA] Bước 3 Edge Detection -> Giờ ảnh kết quả chính là ảnh đã lọc nhiễu
    
    displayImage(imgProcessed, labelFilter);
    QApplication::restoreOverrideCursor();
}

void ImageProcessing::displayImage(const cv::Mat &mat, QLabel *label) {
    if(mat.empty()) return;
    QImage qimg = matToQImage(mat);
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// ------------------- Algorithms Implementation -------------------

cv::Mat ImageProcessing::computeNoise(const cv::Mat &src) {
    QString type = cboNoise->currentText();
    
    double mean = hsMean->value();          
    double var = hsVar->value();            
    double amount = hsAmount->value() / 100.0; 
    
    if (type == "Gaussian") return gaussianNoise(src, mean, var);
    if (type == "Salt & Pepper") return saltPepperNoise(src, amount);
    if (type == "Speckle") return speckleNoise(src, mean, var);
    if (type == "Poisson") {
        cv::Mat dst;
        cv::Mat noise(src.size(), src.type());
        cv::randn(noise, 0, 10); 
        cv::add(src, noise, dst);
        return dst;
    }
    return src.clone();
}

cv::Mat ImageProcessing::gaussianNoise(const cv::Mat &src, double mean, double sigma) {
    cv::Mat noise(src.size(), src.type());
    cv::randn(noise, mean, sigma); 
    cv::Mat dst;
    cv::add(src, noise, dst);
    return dst;
}

cv::Mat ImageProcessing::saltPepperNoise(const cv::Mat &src, double amount) {
    cv::Mat dst = src.clone();
    int num = (int)(amount * src.total());
    for(int i=0; i<num; i++) {
        int r = rand() % src.rows;
        int c = rand() % src.cols;
        if(src.channels()==3) dst.at<cv::Vec3b>(r,c) = cv::Vec3b(255,255,255);
        else dst.at<uchar>(r,c) = 255;
        
        r = rand() % src.rows;
        c = rand() % src.cols;
        if(src.channels()==3) dst.at<cv::Vec3b>(r,c) = cv::Vec3b(0,0,0);
        else dst.at<uchar>(r,c) = 0;
    }
    return dst;
}

cv::Mat ImageProcessing::speckleNoise(const cv::Mat &src, double mean, double sigma) {
    cv::Mat dst;
    src.convertTo(dst, CV_64F);
    cv::Mat noise(src.size(), dst.type());
    cv::randn(noise, 0, sigma/255.0);
    cv::multiply(dst, noise, noise);
    cv::add(dst, noise, dst);
    dst.convertTo(dst, src.type());
    return dst;
}

cv::Mat ImageProcessing::computeDenoise(const cv::Mat &src) {
    QString type = cboDenoise->currentText();
    cv::Mat dst;
    
    // Lấy tham số từ GUI
    int h = sbH->value();
    int patch = sbPatch->value();
    int search = sbSearch->value();
    
    int param1 = hsWeight->value(); // Kernel size hoặc d
    int param2 = hsSigma->value();  // Sigma
    
    if (type == "Non-Local Means Denoising") {
        if(src.channels()==3)
            cv::fastNlMeansDenoisingColored(src, dst, h, h, patch, search);
        else
            cv::fastNlMeansDenoising(src, dst, h, patch, search);
    }
    else if (type == "Bilateral Filter") {
        int d = (param1 > 15) ? 15 : param1; 
        if(d <= 0) d = 5;
        cv::bilateralFilter(src, dst, d, param2*2, param2*2);
    }
    else if (type == "Gaussian Blur") {
        int k = param1;
        if(k % 2 == 0) k++;
        cv::GaussianBlur(src, dst, cv::Size(k, k), 0);
    }
    else if (type == "Median Blur") {
        int k = param1;
        if(k % 2 == 0) k++;
        cv::medianBlur(src, dst, k);
    }
    else {
        dst = src.clone();
    }
    
    return dst;
}