#include "opencvworker.h"

OpenCvWorker::OpenCvWorker(QObject *parent) :
    QObject(parent),
    status(false),
    toggleStream(false),
    binaryThresholdEnable(false),
    binaryThreshold(127)
{
    cap = new VideoCapture();
}

//Destructor - ** Release memory **
OpenCvWorker::~OpenCvWorker(){
    if(cap->isOpened()) cap->release();
    delete cap;
}

void OpenCvWorker::checkIfDeviceAlreadyOpened(const int device)
{
    if(cap->isOpened()) cap->release();
    cap->open(device);
}

void OpenCvWorker::process()
{
    cvtColor(_frameOriginal, _frameProcessed, COLOR_BGR2GRAY);
    if(binaryThresholdEnable){
        cv::threshold(_frameProcessed, _frameProcessed,
                      binaryThreshold, 255, cv::THRESH_BINARY);

    }
}

void OpenCvWorker::receiveGrabFrame()
{
    if(!toggleStream) return;

    (*cap) >> _frameOriginal;
    if(_frameOriginal.empty()) return; //! maybe create a signal to show that something goes wrong.

    //! ** Important **
    //! The image data that comes from the camera, USUALLY,
    //! is of type BGR.
    //! After the processig using opencv you can modify the type
    //! BUT REMEMBER the QImage also need to use this parameter
    //! for images in gray(CV_8U) -> QImage::Format_Indexed8
    //! for images in bgr() -> uses ...

    process();

    QImage output(static_cast<const unsigned char*>(_frameProcessed.data),
                  _frameProcessed.cols, _frameProcessed.rows,
                  static_cast<const int>(_frameProcessed.step),
                  QImage::Format_Indexed8);
    //output.scaledToWidth()  //
    emit sendFrame(output);
}

void OpenCvWorker::receiveSetup(const int device)
{
    checkIfDeviceAlreadyOpened(device);
    if(!cap->isOpened()){
        status = false;
        return;
    }
    status = true;
}

void OpenCvWorker::receiveToggleStream()
{
    toggleStream = !toggleStream;
}

void OpenCvWorker::receiveEnableBinaryThreshold()
{
    binaryThresholdEnable = !binaryThresholdEnable;
}

void OpenCvWorker::receiveBinaryThreshold(int threshold)
{
    binaryThreshold = threshold;
}














