#ifndef DIALOGTHRESHOLD_H
#define DIALOGTHRESHOLD_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class DialogThreshold;
}

class DialogThreshold : public QDialog
{
    Q_OBJECT

private:
    Ui::DialogThreshold *ui;
    cv::Mat _imageOriginal;     // Original Data
    cv::Mat _imageGray; // CVT to Gray
    cv::Mat _imageProcessed;    // Processed Data
    cv::Mat _imageTemp;     // Temporary image
    int _thresholdLevel;
    int _thresholdMaxValue;
    int _thresholdType;
    int _analysedRow;

    bool _showOriginal;
    bool _showGray;
    bool _showProcessed;


    void setup();
    void updateImageView(cv::Mat mat);
    void updateChart(cv::Mat mat);

signals:
    void processChanged();//process changed
    void parametersChanged();    //chart changed

private slots:
    void receiveSelectedImage(int dequeIndex, std::deque<cv::Mat*> *_imagesDeque);

    void setThresholdMaxValue(int val);
    void setThresholdLevel(int val);
    void setAnalysedRow(int val);
    void setThresholdType(int val);
    void updateAll();
    void process();

public:
    explicit DialogThreshold(QWidget *parent = nullptr);
    ~DialogThreshold();
};

#endif // DIALOGTHRESHOLD_H
