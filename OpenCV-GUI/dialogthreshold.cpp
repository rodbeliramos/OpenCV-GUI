#include "dialogthreshold.h"
#include "ui_dialogthreshold.h"
#include <QtWidgets>

using namespace std;
using namespace cv;

DialogThreshold::DialogThreshold(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogThreshold),
    _thresholdLevel(127),
    _thresholdMaxValue(255),
    _thresholdType(0),
    _analysedRow(0),
    _showOriginal(true),
    _showGray(false),
    _showProcessed(false)
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
    ui->thresholdTypeComboBox->setEnabled(true);

    ui->showOriginal->setEnabled(true);
    ui->showOriginal->setChecked(true);
    ui->showGray->setEnabled(true);
    ui->showProcessed->setEnabled(true);

    ui->imageLineSlider->setEnabled(true);
    ui->imageLineSlider->setMaximumHeight(_imageOriginal.rows);
    ui->imageLineSlider->setMaximum(_imageOriginal.rows-1);
    ui->imageLineSlider->setValue(_imageOriginal.rows/2);
    _analysedRow = _imageOriginal.rows/2;
    connect(ui->imageLineSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setAnalysedRow(int)));

    connect(ui->thresholdTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setThresholdType(int)));

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

    connect(ui->showOriginal, SIGNAL(clicked()), this, SLOT(updateAll()));
    connect(ui->showGray, SIGNAL(clicked()), this, SLOT(updateAll()));
    connect(ui->showProcessed, SIGNAL(clicked()), this, SLOT(updateAll()));

    connect(this, SIGNAL(parametersChanged()), this, SLOT(updateAll()));
    connect(this, SIGNAL(processChanged()), this, SLOT(process()));

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
    emit parametersChanged();
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
        line(_mat, CvPoint(0, _analysedRow), CvPoint(_mat.cols-1, _analysedRow),
             255, 1, CV_AA, 0);
        image = QImage(static_cast<const unsigned char*>(_mat.data),
                        _mat.cols, _mat.rows,
                        static_cast<const int>(_mat.step),
                        QImage::Format_Grayscale8);

    }else if (_imageDepth == CV_8U && _imageChannels == 3) {
        line(_mat, CvPoint(0, _analysedRow), CvPoint(_mat.cols-1, _analysedRow),
             cvScalar(255,0,0), 1, CV_AA, 0);
        image = QImage(static_cast<const unsigned char*>(_mat.data),
                        _mat.cols, _mat.rows,
                        static_cast<const int>(_mat.step),
                        QImage::Format_RGB888);
    }    
    ui->imageView->setPixmap(QPixmap::fromImage(image));
}

void DialogThreshold::updateChart(cv::Mat mat){

    //qDebug() << "updateChart()";
    Mat chart = Mat(ui->chartView->width(),ui->chartView->height(),
                    CV_8UC3, cvScalar(255,255,255));
    int fontFace = FONT_HERSHEY_PLAIN;
    double fontScale = 1;
    int thickness = 1;

    // Chart's X Axis
    line(chart, CvPoint(20, 280), CvPoint(280, 280), cvScalar(30,30,30), 1, CV_AA, 0);
    // Chart's Y Axis
    line(chart, CvPoint(20, 280), CvPoint(20, 20), cvScalar(30,30,30), 1, CV_AA, 0);

    // Chart's value
    putText(chart, " 0", CvPoint(5,295), fontFace, fontScale, cvScalar(30,30,30), thickness, CV_AA);
    putText(chart, "255", CvPoint(5,15), fontFace, fontScale, cvScalar(30,30,30), thickness, CV_AA);
    putText(chart, "col", CvPoint(270,295), fontFace, fontScale, cvScalar(30,30,30), thickness, CV_AA);

    // Histograms ( depends of the image selected )
    if(mat.channels()==1){

        int matCols = mat.cols;    //200
        int chartW = 260;
        int binW = 1;
        int chartCol = 0;

        if(matCols > chartW)
            binW = matCols/chartW;

        vector<Point> chartPoints;
        chartPoints.push_back(Point(21,281));

        for (int matCol = 0; matCol<matCols; matCol+=binW) {
            int sum = 0;

            for (int binCol = 0; binCol<binW; binCol++) {
                sum += mat.at<uchar>(_analysedRow, matCol+binCol); //pixel value on imageGray
            }
            int binVal = sum/binW;
            Point point = Point(21+chartCol,280-binVal);
            chartPoints.push_back(point);
            chartCol++;
        }
        chartPoints.push_back(Point(21+chartCol, 281));
        vector<vector<Point>> allCharts;
        allCharts.push_back(chartPoints);
        if(_showProcessed)
            fillPoly(chart, allCharts, Scalar(100,100,255));
        polylines(chart,allCharts,true,Scalar(0,0,0),1,CV_AA,0);

        if(_showProcessed){
            chartCol = 0;

            vector<Point> grayPoints;
            grayPoints.push_back(Point(21,281));

            for (int matCol = 0; matCol<matCols; matCol+=binW) {
                int sum = 0;

                for (int binCol = 0; binCol<binW; binCol++) {
                    sum += _imageGray.at<uchar>(_analysedRow, matCol+binCol); //pixel value on imageGray
                }
                int binVal = sum/binW;
                Point point = Point(21+chartCol,280-binVal);
                grayPoints.push_back(point);
                chartCol++;
            }
            grayPoints.push_back(Point(21+chartCol, 281));
            vector<vector<Point>> grayChart;
            grayChart.push_back(grayPoints);

            //fillPoly(chart, grayChart, Scalar(100,100,255));
            polylines(chart,grayChart,true,Scalar(0,0,0),1,CV_AA,0);
        }


    }

    if(mat.channels()==3){
        Vec3b intensity;
        Point lastPoint;
        vector<Scalar> colors = {Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};  //BGR
        int matCols = mat.cols;    //200
        int chartW = 260;
        int binW = 1;
        int chartCol = 0;

        if(matCols > chartW)
            binW = matCols/chartW;

        for (uint color=0; color<3; color++) {
            chartCol = 0;
            for (int matCol = 0; matCol<matCols; matCol+=binW) {
                int sum = 0;
                for (int binCol = 0; binCol<binW; binCol++) {
                    intensity = mat.at<Vec3b>(_analysedRow, matCol+binCol); //valor no pixel
                    sum+= intensity.val[color];
                }
                int binVal = sum/binW;
                Point point = Point(21+chartCol,280-binVal);
                if(chartCol)
                    line(chart, lastPoint, point, colors[color], 1, CV_AA, 0);
                lastPoint = point;

                chartCol++;
            }
        }
    }

    // Threshold Limits
    line(chart, CvPoint(21, 280-_thresholdMaxValue), CvPoint(279, 280-_thresholdMaxValue),
         cvScalar(50,255,50), 1, CV_AA, 0);

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
        emit processChanged();
        emit parametersChanged();
    }
}

void DialogThreshold::setThresholdLevel(int val)
{
    if(_thresholdLevel != val){
        _thresholdLevel = val;
        emit processChanged();
        emit parametersChanged();
    }
}

void DialogThreshold::setAnalysedRow(int val)
{
    if(_analysedRow != val){
        _analysedRow = val;
        emit parametersChanged(); //emit imageChanged();  imageChanged() -> updateImageView
    }
}

void DialogThreshold::setThresholdType(int val){
    if(_thresholdType != val){
        _thresholdType = val;
        emit processChanged();
        emit parametersChanged();
    }
}

void DialogThreshold::updateAll(){
    if(ui->showGray->isChecked()){
        _showOriginal = false;
        _showGray = true;
        _showProcessed = false;
        updateImageView(_imageGray);
        updateChart(_imageGray);

    } else if(ui->showOriginal->isChecked()){
        _showOriginal = true;
        _showGray = false;
        _showProcessed = false;
        updateImageView(_imageOriginal);
        updateChart(_imageOriginal);

    } else if (ui->showProcessed->isChecked()){
        _showOriginal = false;
        _showGray = false;
        _showProcessed = true;
        updateImageView(_imageProcessed);
        updateChart(_imageProcessed);

    } else {
        qDebug() << "Erro @selectImage()";
    }
}

void DialogThreshold::process(){
    threshold(_imageGray,_imageProcessed,_thresholdLevel,_thresholdMaxValue,_thresholdType);
}






