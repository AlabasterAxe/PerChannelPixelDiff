#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <cv.h>

namespace Ui {
class MainWindow;
}

namespace cv {
class VideoCapture;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void captureImage();
    void resetFrames();
    void startLimitTimer();
    void openVideos();

private:
    Ui::MainWindow *ui;
    cv::VideoCapture *capture;
    QTimer* timer;
    QTimer* timerToo;
    cv::Mat image;
    cv::Mat previous;
    unsigned int frames;
};

#endif // MAINWINDOW_H
