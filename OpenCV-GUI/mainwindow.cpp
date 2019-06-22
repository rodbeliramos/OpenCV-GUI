#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QTimer>
#include <QCameraInfo>

#include <opencv2/opencv.hpp>

Q_DECLARE_METATYPE(QCameraInfo)

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);




    setup();

    //MENU BAR:
    //FILE
    connect(ui->actionNew_Project, &QAction::triggered, this, &MainWindow::newProject);
    //openProject
    //closeProject

    //DEVICES
    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            videoDeviceAction->setChecked(true);

        ui->menuDevices->addAction(videoDeviceAction);
    }
    //connect(videoDevicesGroup, &QActionGroup::triggered, this, &MainWindow::updateCameraDevice);

    //HELP
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);

    //TOOL BAR:
    //newImage
    connect(ui->actionNew_Image, &QAction::triggered, this, &MainWindow::newImage);
    //takePhoto
    //startCamera
}

MainWindow::~MainWindow()
{
    thread->quit();
    while (!thread->isFinished());
    //_imagesDeque.clear();

    delete thread;
    delete ui;
}

void MainWindow::setup()
{
    thread = new QThread();
    OpenCvWorker *worker = new OpenCvWorker();
    //! if we create worker as class member of MainWindow
    //! we cannot move it to another thread because a Qobject can only be passed
    //! onto thread that is not part of the current thread that is running in
    //! sencond, only dynamic allocation of *worker, its instance will basically
    //! do the trick. because is we have the worker on the stack it will disapper
    //! but if we pass the chunk of memory that is allocated this line of code
    //! the thread will continue to live on


    QTimer *workerTrigger = new QTimer();
    //! Qtimer will be use to trigger the grabbing of the frame and the
    //! processing everytime its times out.

    workerTrigger->setInterval(1);

    // Signals that are ok:
    connect(workerTrigger, SIGNAL(timeout()), worker, SLOT(receiveGrabFrame()));
    connect(this, SIGNAL(sendSetup(int)), worker, SLOT(receiveSetup(int)));
    connect(this, SIGNAL(sendToggleStream()), worker, SLOT(receiveToggleStream()));
    connect(ui->actionStart_Camera, SIGNAL(triggered(bool)), this, SLOT(receiveToggleStream()));
    connect(ui->checkBoxEnableBinaryThreshold, SIGNAL(toggled(bool)), worker, SLOT(receiveEnableBinaryThreshold()));
    connect(ui->spiBoxEnableBinaryThreshold, SIGNAL(valueChanged(int)), worker, SLOT(receiveBinaryThreshold(int)));
    connect(worker, SIGNAL(sendFrame(QImage)), this, SLOT(receiveFrame(QImage)));
    // Testing Signals:
    connect(this, SIGNAL(sendSelectedImage(int)), this, SLOT(receiveSelectedImage(int)));

    workerTrigger->start();

    worker->moveToThread(thread);
    workerTrigger->moveToThread(thread);

    thread->start();
    emit sendSetup(0);
}

void MainWindow::receiveFrame(QImage frame)
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->camImageLabel->setPixmap(QPixmap::fromImage(frame));
}

void MainWindow::receiveToggleStream()
{
    //if(!ui->pushButtonPlay->text().compare(">")) ui->pushButtonPlay->setText("||");
    //else ui->pushButtonPlay->setText(">");
    emit sendToggleStream();
}

void MainWindow::newProject()
{
    printf("teste");
}

void MainWindow::showAbout()
{
   QMessageBox::about(this, tr("About"),
            tr("The <b>OpenCV-GUI</b> demonstrates functions and some "
               "image processing algorithms using the OpenCV library."
               " "));
}

void MainWindow::receiveSelectedImage(int dequeIndex)
{
        Mat mat = *(_imagesDeque.at(static_cast<unsigned long>(dequeIndex)));

        //! PROBLEM FOUND! - Image get distorted when send to the QImage
        //!
        //! Solution at:
        //! https://stackoverflow.com/questions/39057168/qwidget-draws-distorted-angled-qimage
        // imshow("Image", mat); // Debugging
        QImage image(static_cast<const unsigned char*>(mat.data),
                        mat.cols, mat.rows,
                        static_cast<const int>(mat.step),
                        QImage::Format_RGB888);

        ui->stackedWidget->setCurrentIndex(1);
        ui->singleCamImgLabel->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::newImage()
{
    //! Opening single image

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home", tr("Image Files (*.png *.jpg)"));
    if (!fileName.isEmpty()){
        statusBar()->showMessage(fileName);
    }

    Mat *_imageData = new Mat();
    *_imageData = imread(fileName.toStdString());
    if(!_imageData->empty()){
        cvtColor(*_imageData, *_imageData, CV_BGR2RGB);

        _imagesDeque.push_back(_imageData);
        qDebug() << "ImageData pointer: " << _imageData ;
        qDebug() << "_imagesDeque.size: " << _imagesDeque.size();

        _selectedImageIndex = static_cast<int>(_imagesDeque.size()-1);
        qDebug() << "SelectedImageIndex: " << _selectedImageIndex ;

        emit sendSelectedImage(_selectedImageIndex);
    }

    //! Add new image to the image list


    //cvtColor(*_imageData, *_imageData, CV_BGR2RGB);

    /*
    QImage image(static_cast<const unsigned char*>(_imageData->data),
                  _imageData->cols, _imageData->rows,
                  QImage::Format_RGB888);

    ui->singleCamImgLabel->setPixmap(QPixmap::fromImage(image));
    */

    //! TODO: Opening multiple files
/*
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.png *.jpg)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    QString selectedFiles;

    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (!fileNames.isEmpty()){
        for(QString fileName : fileNames){


        selectedFiles.operator+=(fileName);
        selectedFiles.operator+=(" ");
        }
    }
    statusBar()->showMessage(selectedFiles);
*/
}

void MainWindow::on_functionsList_itemDoubleClicked(QListWidgetItem *item)
{
    if(item == nullptr){
        qDebug() << "nullptr on selected item";
        return;
    }
    qDebug() << "Selected item: " << item->text() ;

    // Open Functions Dialogs :
    if(item->text() == "cvThreshold()"){
        thresholdDialog = new DialogThreshold(this);
        thresholdDialog->setModal(true);


        if(!_imagesDeque.empty() && _selectedImageIndex!=-1){
            connect(this, SIGNAL(sendSelectedImage(int, std::deque<cv::Mat*>*)),
                thresholdDialog, SLOT(receiveSelectedImage(int, std::deque<cv::Mat*>*)));
            emit sendSelectedImage(_selectedImageIndex, &_imagesDeque);
        }


        thresholdDialog->show();
    }
}











