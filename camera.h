#ifndef CAMERA_H
#define CAMERA_H

/**
 * @brief Use namsapace for camera functions!
 * @attention consider namespace instead of creating a class due to the simplicity!
 */

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

#include <QObject>
#include "imagewidget.h" // Include the header for ImageWidget

class Camera : public QObject
{
    Q_OBJECT

public:
 explicit Camera();
    ~Camera();

 bool start_camera(int index, bool& face_detection, ImageWidget *imageWidget );
 void processFrames() ;
 QImage MatToQImage(const cv::Mat &mat) ;

 protected:



 private:
     cv::VideoCapture videoCapture;

};



#endif   // CAMERA_H
