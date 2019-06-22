#include "dialogthreshold.h"
#include "ui_dialogthreshold.h"
#include <QtWidgets>

using namespace std;
using namespace cv;

DialogThreshold::DialogThreshold(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogThreshold),
    _thresholdLevel(127),
    _thresholdMaxValue(255)
{
    ui->setupUi(this);
}

DialogThreshold::~DialogThreshold()
{
    delete ui;
}

void DialogThreshold::receiveSelectedImage(int dequeIndex, std::deque<cv::Mat*> *_imagesDeque)
{
    if(!_imagesDeque->empty() && dequeIndex != -1)
    {
        _imageOriginal = *(_imagesDeque->at(static_cast<unsigned long>(dequeIndex)));
        setup();
        updateImageView(_imageOriginal);
    }
    else {
        qDebug() << "Error, no image on queue. This functions should not be called.";
    }
}

void DialogThreshold::setup()
{
    ui->comboBox->setEnabled(true);

    ui->showOriginal->setEnabled(true);
    ui->showOriginal->setChecked(true);
    ui->showGray->setEnabled(true);
    ui->showProcessed->setEnabled(true);

    ui->imageLineSlider->setEnabled(true);
    ui->imageLineSlider->setMaximumHeight(_imageOriginal.rows);
    ui->imageLineSlider->setMaximum(_imageOriginal.rows-1);
    ui->imageLineSlider->setValue(_imageOriginal.rows/2);
    connect(ui->imageLineSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setAnalysedRow(int)));

    ui->maxLevelSlider->setEnabled(true);
    ui->maxLevelSpinBox->setEnabled(true);
    connect(ui->maxLevelSlider, SIGNAL(valueChanged(int)),
            ui->maxLevelSpinBox, SLOT(setValue(int)));
    connect(ui->maxLevelSpinBox, SIGNAL(valueChanged(int)),
            ui->maxLevelSlider, SLOT(setValue(int)));
    connect(ui->maxLevelSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setThresholdMaxValue(int)));

    ui->thresholdLevelSlider->setEnabled(true);
    ui->thresholdLevelSpinBox->setEnabled(true);
    connect(ui->thresholdLevelSlider, SIGNAL(valueChanged(int)),
            ui->thresholdLevelSpinBox, SLOT(setValue(int)));
    connect(ui->thresholdLevelSpinBox, SIGNAL(valueChanged(int)),
            ui->thresholdLevelSlider, SLOT(setValue(int)));
    connect(ui->thresholdLevelSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setThresholdLevel(int)));

    connect(ui->showOriginal, SIGNAL(clicked()), this, SLOT(selectImage()));
    connect(ui->showGray, SIGNAL(clicked()), this, SLOT(selectImage()));
    connect(ui->showProcessed, SIGNAL(clicked()), this, SLOT(selectImage()));

    connect(this, SIGNAL(chartChanged()), this, SLOT(updateChart()));

    ui->chartView->setFixedSize(300,300);
    /**
    0    255
            |
            |
            |
            |
            |__________
    300    0
        0               300
    */
    emit chartChanged();
    cvtColor(_imageOriginal, _imageGray, CV_BGR2GRAY);
    threshold(_imageGray,_imageProcessed,_thresholdLevel,_thresholdMaxValue,_thresholdType);


}


void DialogThreshold::updateImageView(cv::Mat mat)
{
    Mat _mat = mat.clone();
    QImage image;
    _imageTemp = _mat.clone();

    int _imageDepth = _mat.depth();
        /** CV_8U - 8-bit unsigned integers ( 0..255 )*/

    int _imageChannels = _mat.channels();
        /** Number of matrix channels - gray = 1 BGR = 3*/

    if(_imageDepth == CV_8U && _imageChannels == 1){
        line(_mat, CvPoint(0, mat.rows-_analysedRow), CvPoint(_mat.cols-1, _mat.rows-_analysedRow),
             255, 1, CV_AA, 0);
        image = QImage(static_cast<const unsigned char*>(_mat.data),
                        _mat.cols, _mat.rows,
                        static_cast<const int>(_mat.step),
                        QImage::Format_Grayscale8);

    }else if (_imageDepth == CV_8U && _imageChannels == 3) {
        line(_mat, CvPoint(0, _mat.rows-_analysedRow), CvPoint(_mat.cols-1, _mat.rows-_analysedRow),
             cvScalar(255,0,0), 1, CV_AA, 0);
        image = QImage(static_cast<const unsigned char*>(_mat.data),
                        _mat.cols, _mat.rows,
                        static_cast<const int>(_mat.step),
                        QImage::Format_RGB888);
    }    
    ui->imageView->setPixmap(QPixmap::fromImage(image));
}

void DialogThreshold::updateChart(){

    qDebug() << "updateChart()";
    Mat chart = Mat(ui->chartView->width(),ui->chartView->height(),
                    CV_8UC3, cvScalar(255,255,255));
    int fontFace = FONT_HERSHEY_PLAIN;
    double fontScale = 1;
    int thickness = 2;

    // Chart's X Axis
    line(chart, CvPoint(20, 280), CvPoint(280, 280), cvScalar(0,0,0), 1, CV_AA, 0);
    // Chart's Y Axis
    line(chart, CvPoint(20, 280), CvPoint(20, 20), cvScalar(0,0,0), 1, CV_AA, 0);

    // Chart's value
    putText(chart, " 0", CvPoint(5,295), fontFace, fontScale, cvScalar(0,0,0), thickness, 8);
    putText(chart, "255", CvPoint(5,15), fontFace, fontScale, cvScalar(0,0,0), thickness, 8);
    putText(chart, "end", CvPoint(290,295), fontFace, fontScale, cvScalar(0,0,0), thickness, 8);

    // Histograms


    // Threshold Limits
    line(chart, CvPoint(21, 280-_thresholdMaxValue), CvPoint(279, 280-_thresholdMaxValue),
         cvScalar(255,0,0), 1, CV_AA, 0);

    line(chart, CvPoint(21, 280-_thresholdLevel), CvPoint(279, 280-_thresholdLevel),
            cvScalar(0,0,255), 1, CV_AA, 0);



    cvtColor(chart, chart, CV_BGR2RGB);

    QImage image;
    image = QImage(static_cast<const unsigned char*>(chart.data),
                    chart.cols, chart.rows,
                    static_cast<const int>(chart.step),
                    QImage::Format_RGB888);
    ui->chartView->setPixmap(QPixmap::fromImage(image));

}

void DialogThreshold::setThresholdMaxValue(int val)
{
    if(_thresholdMaxValue != val){
        _thresholdMaxValue = val;
        emit chartChanged();
    }
}

void DialogThreshold::setThresholdLevel(int val)
{
    if(_thresholdLevel != val){
        _thresholdLevel = val;
        emit chartChanged();
    }
}

void DialogThreshold::setAnalysedRow(int val)
{
    if(_analysedRow != val){
        _analysedRow = val;
        emit selectImage(); //emit imageChanged();  imageChanged() -> updateImageView
    }
}

void DialogThreshold::selectImage(){
    if(ui->showGray->isChecked()){
        updateImageView(_imageGray);

    } else if(ui->showOriginal->isChecked()){
        updateImageView(_imageOriginal);

    } else if (ui->showProcessed->isChecked()){
        updateImageView(_imageProcessed);
    } else {
        qDebug() << "Erro @selectImage()";
    }
}







