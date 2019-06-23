#ifndef OPENCVWORKER_H
#define OPENCVWORKER_H

#include <QObject>
#include <QImage>   //to convert cvMats to Q friendly format

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class OpenCvWorker : public QObject
{
    Q_OBJECT

private:
    Mat _frameOriginal;     // Original data
    Mat _frameProcessed;    // Data Processed
    VideoCapture *cap;      // Video Capture

    bool status;            // To check if the device was opened properly
    bool toggleStream;      // To control the capturing

    //threshold:
    bool binaryThresholdEnable; // Enable/Disable status
    int binaryThreshold;    // Threshold level

    void checkIfDeviceAlreadyOpened(const int device);
    void process();         //opencv processings (filters, threshold and else)

public:
    explicit OpenCvWorker(QObject *parent = nullptr);
    ~OpenCvWorker();


signals:
    void sendFrame(QImage frameProcessed);  // Signal to send QImage to slot in our UI


public slots:
    void receiveGrabFrame();                //will be trigged when grab a frame from capture device
    void receiveSetup(const int device);    //way to opendevice to capture a video capture
    void receiveToggleStream();         //toggle toggleStream act on frame grabbing and process image data
    void receiveEnableBinaryThreshold();
    void receiveBinaryThreshold(int threshold);




};

#endif // OPENCVWORKER_H
