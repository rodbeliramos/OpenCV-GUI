#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <QListWidget>
#include "opencvworker.h"
#include "dialogthreshold.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QThread *thread;        // thread to do the hole work
    std::deque<cv::Mat*> _imagesDeque;
    DialogThreshold *thresholdDialog;
    int _selectedImageIndex = -1;

    void setup();       //to create instances of opencvWorker, QThread and QTime

signals:
    void sendSetup(int device);
    void sendToggleStream();
    void sendSelectedImage(int dequeIndex);
    void sendSelectedImage(int dequeIndex, std::deque<cv::Mat*> *_imagesDeque);

private slots:
    void receiveFrame(QImage frame);
    void receiveToggleStream();
    void receiveSelectedImage(int dequeIndex);

    //MENU BAR:
    //File
    void newProject();
    //void openProject();
    //void closeProject();
    //Help
    void showAbout();

    //TOOL BAR:
    void newImage();
    //void takePhoto();
    //void startCamera();

    //CAMERA
    //void updateCameraDevice(QAction *action);
    //void updateCaptureMode();
    //void updateCameraState(QCamera::State);


    void on_functionsList_itemDoubleClicked(QListWidgetItem *item);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};



#endif // MAINWINDOW_H
