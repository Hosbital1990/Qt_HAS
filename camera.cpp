#include <opencv2/opencv.hpp>
#include "camera.h"

#include <QDebug>
#include <QtConcurrent/QtConcurrent>

// OpenCV headers
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp> // For face detection


/**
 * @brief Implementation of Camera class
 *
 * @return true
 * @return false
 */

Camera::Camera(){}

bool Camera::start_camera(int index, bool& face_detection, ImageWidget *imageWidget)
{

    if (videoCapture.isOpened()) {
        videoCapture.release(); // Release previous camera
    }
    // Open selected camera using OpenCV
    if (index >= 0 ) {
        int cameraIndex = 2*index; // Adjust this as per your camera setup
        videoCapture.open(cameraIndex);
        if (!videoCapture.isOpened()) {
            qWarning() << "Failed to open camera!";
            return 0;
        }

        // Start the frame processing loop using a lambda function with QtConcurrent::run
        QtConcurrent::run([&,this]() {
             this->processFrames(processFrames(imageWidget);
         });

    } else {
        qWarning() << "Invalid camera index.";
    }
}

void Camera::processFrames(ImageWidget *imageWidget) {
    // Start the timer to measure processing time
    QElapsedTimer timer;
    timer.start();

    cv::Mat frame;
    //  while (videoCapture.isOpened()) {
    videoCapture.read(frame);
    if (frame.empty()) {
        qWarning() << "Empty frame!";
        return;
    }

    // Perform face detection using OpenCV (example code)
    std::vector<cv::Rect> faces;
    cv::Mat frame_gray;

    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);

    // Detect faces
    //    face_cascade.detectMultiScale(frame_gray, faces);

    // Draw rectangles around detected faces
    for (const auto &face : faces) {
        cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);
    }

    // Convert cv::Mat to QImage
    QImage processedImage = MatToQImage(frame);

    // Display the processed image in Qt
    imageWidget->setImage(processedImage);
    //}
    // Measure the time taken and output the result
    qint64 elapsed = timer.elapsed();
    qDebug() << "Frame processing time:" << elapsed << "milliseconds";

}

QImage Camera::MatToQImage(const cv::Mat &mat) {
    switch (mat.type()) {
    case CV_8UC3: // RGB
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    case CV_8UC4: // RGBA
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888);
    case CV_8UC1: // Grayscale
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    default:
        qWarning() << "Unsupported cv::Mat format!";
        return QImage();
    }
}

Camera::~Camera(){}






