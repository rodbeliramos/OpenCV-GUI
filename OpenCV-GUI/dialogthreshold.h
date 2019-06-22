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
    enum processImg_t {  IMG_ORIGINAL  = 0,
                IMG_GRAY      = 1,
                IMG_PROCESSED = 2,
                IMG_TEMP      = 3
                } _images;
    bool _showOriginal;
    bool _showGray;
    bool _showProcessed;
    int _thresholdType;
    int _analysedRow;

    void setup();
    void updateImageView(cv::Mat mat);

    //void process();
    //void connectThresholControlGroup();
    //void disconnectThresholControlGroup();
signals:
        //process changed
    void chartChanged();    //chart changed
        //image changed

private slots:
    void receiveSelectedImage(int dequeIndex, std::deque<cv::Mat*> *_imagesDeque);
    void updateChart();
    void setThresholdMaxValue(int val);
    void setThresholdLevel(int val);
    void setAnalysedRow(int val);
    void selectImage();

    //void onComboBoxIndexChanged(int);
    //void onThresholdValueChanged(int value);
    //void showImage(int img);

    //void on_verticalSlider_valueChanged(int value);

    //void on_showOriginal_toggled(bool checked);

    //void on_showGray_toggled(bool checked);

    //void on_showProcessed_toggled(bool checked);

public:
    explicit DialogThreshold(QWidget *parent = nullptr);
    ~DialogThreshold();
};

#endif // DIALOGTHRESHOLD_H
