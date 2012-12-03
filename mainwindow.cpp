#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/opencv.hpp"

#include <QPainter>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->capture = new cv::VideoCapture();
    this->timer = new QTimer(this);
    this->timer->setInterval(33);
    this->timerToo = new QTimer(this);
    this->timerToo->setInterval(10000);
    frames = 0;

    QObject::connect(this->ui->startButton,SIGNAL(clicked()),this->timer,SLOT(start()));
    QObject::connect(this->ui->actionOpenVideo,SIGNAL(triggered()),this,SLOT(openVideos()));
    QObject::connect(this->ui->startButton,SIGNAL(clicked()),this,SLOT(startLimitTimer()));
    QObject::connect(this->ui->stopButton,SIGNAL(clicked()),this->timer,SLOT(stop()));
    QObject::connect(this->ui->stopButton,SIGNAL(clicked()),this,SLOT(resetFrames()));
    QObject::connect(this->timer,SIGNAL(timeout()),this,SLOT(captureImage()));
    QObject::connect(this->timerToo,SIGNAL(timeout()),timer,SLOT(stop()));
    QObject::connect(this->timerToo,SIGNAL(timeout()),timerToo,SLOT(stop()));
    QObject::connect(this->timerToo,SIGNAL(timeout()),this,SLOT(resetFrames()));
}

MainWindow::~MainWindow()
{
    this->capture->release();
    delete this->capture;
    delete ui;
}

void MainWindow::resetFrames()
{
    std::cout << frames << std::endl;
    frames = 0;
}

void MainWindow::startLimitTimer()
{
    unsigned char seconds = this->ui->cameraLimit->value();
    if (seconds > 0)
        this->timerToo->start(seconds*1000);
}

void MainWindow::captureImage()
{
    this->frames = this->frames + 1;
    unsigned char noiseThreshold = this->ui->threshold->value();
    //open the user-selected camera number using OpenCV's VideoCapture
    //we reopen to get most recent image...
    if(!capture->isOpened())
    {
        if (!capture->open(0)) {
            std::cerr << "ERROR: could not open camera" << std::endl;
            return;
        }
    //an OpenCV matrix to hold the original color image
    }

    //cv::Mat color_matrix;
    //grab the image twice... this may be due to the first image being
    //garbage on some computers, but it should not be a universal condition
    //future developers should try to grab only once
    if (ui->previousCheck->isChecked())
        this->previous = this->image.clone();

    //(*capture) >> this->image;
    (*capture) >> this->image;

    if (this->previous.empty()){
        std::cerr << "Tasty" << std::endl;
        this->previous = this->image.clone();
    }

    if (this->image.empty())
    {
        std::cerr << "ERROR: camera image is empty" << std::endl;
        return;
    }

    if (this->image.type() != CV_8UC3)
    {
        std::cerr << "ERROR: camera image is not 3-channel 8-bit unsigned" << std::endl;
        return;
    }

    // Convert the color matrix to the black and white image that our point finding algorithms expect
    //cv::cvtColor(color_matrix,this->image,CV_RGB2GRAY);

    QImage qImage(this->image.size().width,
                  this->image.size().height,
                  QImage::Format_ARGB32);
    //convert each pixel from OpenCV to Qt format
    unsigned int pixels = 1;
    unsigned int sum_x = 0;
    unsigned int sum_y = 0;

    for (int y = 0; y < this->image.size().height; ++y)
    {
        QRgb* destrow = (QRgb*) qImage.scanLine(y);
        //const cv::Vec3b& bgr = image.at<cv::Vec3b>(y,0);
        //const cv::Vec3b& last = previous.at<cv::Vec3b>(y,0);
        //std::cout << "R:" << bgr[2] - last[2] << std::endl;
        //std::cout << "G:" << bgr[1] - last[1] << std::endl;
        //std::cout << "B:" << bgr[0] - last[0] << std::endl;
        for (int x = 0; x < image.size().width; ++x)
        {
            const cv::Vec3b& bgr = image.at<cv::Vec3b>(y,x);
            const cv::Vec3b& last = previous.at<cv::Vec3b>(y,x);
            bool differ[3];

            for(int i = 0; i < 3; ++i) {
                differ[i] = (abs(bgr[i] - last[i]) > noiseThreshold);

                if (differ[i]) {
                    ++pixels;
                    sum_x += x;
                    sum_y += y;
                }
            }

            destrow[x] = qRgba(255*(differ[2]),
                               255*(differ[1]),
                               255*(differ[0]),
                               255);
        }
    }
    //turn the QImage into a QPixmap
    QPixmap qPixmap = QPixmap::fromImage(qImage);
    QPen qPen;
    qPen.setWidth(3);
    qPen.setColor(QColor(0,255,0));
    QPainter qPainter(&qPixmap);
    qPainter.setPen(qPen);
    //std::cout << "X: " << sum_x/pixels <<", Y: " << sum_y/pixels << std::endl;
    QPoint point(sum_x/pixels, sum_y/pixels);
    qPainter.drawEllipse(point, 3,3);
    //put up the Pixmap on the GUI's image label widget
    ui->imageLabel->setPixmap(qPixmap);
}

void MainWindow::openVideos()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Videos"),QString(),tr("Videos (*.avi *.mp4 *.mts *.wmv)"));
    this->capture->open(fileName.toStdString().c_str());
}
