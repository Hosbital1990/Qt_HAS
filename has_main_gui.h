#ifndef HAS_MAIN_GUI_H
#define HAS_MAIN_GUI_H

#include <QMainWindow>
#include <QMutex>
#include <QImage>
#include <QComboBox>
#include <opencv2/opencv.hpp>
#include "imagewidget.h" // Include the header for ImageWidget
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui {
class HAS_Main_GUI;
}
QT_END_NAMESPACE

class HAS_Main_GUI : public QMainWindow
{
    Q_OBJECT

public:
    HAS_Main_GUI(QWidget *parent = nullptr);
    ~HAS_Main_GUI();

    // Conversion between QImage and cv::Mat
    cv::Mat QImageToMat(const QImage &image);
    QImage MatToQImage(const cv::Mat &mat);


public slots:
    void toggle_light();
    void onCameraSelected(int index);
    void processFrames(); // Updated to match the cpp file's processFrames method

private:
    Ui::HAS_Main_GUI *ui;
    int toggle_counter;

    QMutex mutex;

    // OpenCV camera and face detection components
    cv::VideoCapture videoCapture;      // Renamed from 'cap' to 'videoCapture' to match cpp
    cv::CascadeClassifier face_cascade; // Haar cascade for face detection

    QComboBox *cameraComboBox;          // Camera selection combo box

    QImage processedImage;              // Store the processed image
    ImageWidget *imageWidget;           // Custom widget for displaying images

    // Store the available camera device identifiers or names
    QStringList cameraIdentifiers;      // List of camera identifiers
    QTimer* cameraTimer;
   // QMutex mutex; // Mutex for controlling access to processFrames

    // Function to start OpenCV camera
    void startOpenCVCamera();           // Kept this in case you want to initialize OpenCV camera setup
};

#endif // HAS_MAIN_GUI_H
