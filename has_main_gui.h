#ifndef HAS_MAIN_GUI_H
#define HAS_MAIN_GUI_H

#include <QMainWindow>
#include <QMutex>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <QComboBox>
#include "imagewidget.h" // Include the header for ImageWidget

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

    cv::Mat QImageToMat(const QImage &image);
    QImage  MatToQImage(const cv::Mat &mat);


public slots:
    void toggle_light();
    void onCameraSelected(int index);
    void processFrame(const QVideoFrame &frame);


private:
    Ui::HAS_Main_GUI *ui;
    int toggle_counter;

    QMutex mutex;
    QCamera *camera;
    QVideoWidget *viewfinder;
    QMediaCaptureSession *captureSession;
    QComboBox *cameraComboBox;

    QImage processedImage; // Store the processed image
    ImageWidget *imageWidget;
    // Store the available camera device identifiers or names
    QStringList cameraIdentifiers; // List of camera identifiers

    cv::CascadeClassifier face_cascade;

};
#endif // HAS_MAIN_GUI_H
