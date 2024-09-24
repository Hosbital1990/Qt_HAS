#include "has_main_gui.h"
#include "./ui_has_main_gui.h"
#include "imagewidget.h" // Include the custom ImageWidget

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QComboBox>
#include <QDir>
#include <QMediaDevices>
#include <QSpacerItem>
#include <QVideoSink>
#include <QPainter>
#include <QtConcurrent/QtConcurrent>

HAS_Main_GUI::HAS_Main_GUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HAS_Main_GUI)
    , toggle_counter(0)
    , camera(nullptr)
{
    ui->setupUi(this);

    // Create and configure the button
    QPushButton *button = new QPushButton("Toggle Light", this);
    button->setMinimumHeight(40);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(button, &QPushButton::clicked, this, &HAS_Main_GUI::toggle_light);

    // Create and configure the combo box
    cameraComboBox = new QComboBox(this);
    cameraComboBox->setMinimumHeight(40);
    cameraComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(cameraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HAS_Main_GUI::onCameraSelected);

    // Create a layout for the buttons and combo box
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addSpacerItem(new QSpacerItem(100, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    topLayout->addWidget(button);
    topLayout->addSpacerItem(new QSpacerItem(100, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    topLayout->addWidget(cameraComboBox);
    topLayout->addSpacerItem(new QSpacerItem(100, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Create and configure the buttons
    QPushButton *recordButton = new QPushButton("Record", this);
    QPushButton *face_detectionButton = new QPushButton("Face Detection", this);
    QPushButton *snapshot_Button = new QPushButton("Snapshot", this);
    QPushButton *video_playback = new QPushButton("Playback", this);

    // Create a layout for the right buttons
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(face_detectionButton);
    rightLayout->addWidget(recordButton);
    rightLayout->addWidget(snapshot_Button);
    rightLayout->addWidget(video_playback);

    // Create and configure the image widget
    imageWidget = new ImageWidget(this);
    imageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout* downsideLayout = new QHBoxLayout;
    downsideLayout->addWidget(imageWidget, 5);
    downsideLayout->addLayout(rightLayout, 1);

    // Create the main layout and add the image widget
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout, 1);
    mainLayout->addLayout(downsideLayout, 50);

    ui->centralwidget->setLayout(mainLayout);

    // Initialize the available cameras
    cameraIdentifiers << "Camera 1" << "Camera 2";

    // Populate the combo box with camera names
    cameraComboBox->addItems(cameraIdentifiers);

    // Load the pre-trained Haar Cascade XML classifier
    if (!face_cascade.load("../../haarcascade_frontalface_default.xml")) {
        std::cerr << "Error loading face cascade file!" << std::endl;
        // wait for some while to next try
    }

    // Initialize the default camera
    if (!cameraIdentifiers.isEmpty()) {
        onCameraSelected(0);
    }
}

void HAS_Main_GUI::toggle_light() {
    // Toggle light functionality
}

void HAS_Main_GUI::onCameraSelected(int index) {
    if (camera) {
        camera->stop();
        delete camera;
        camera = nullptr; // Reset camera
    }

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();

    if (index >= 0 && index < cameras.size()) {
        QCameraDevice selectedCameraDevice = cameras[index];
        camera = new QCamera(selectedCameraDevice, this);
        captureSession = new QMediaCaptureSession(this);
        captureSession->setCamera(camera);

        QVideoSink *videoSink = new QVideoSink(this);
        captureSession->setVideoOutput(videoSink);
        connect(videoSink, &QVideoSink::videoFrameChanged, this, &HAS_Main_GUI::processFrame);
        camera->start();
    } else {
        qWarning() << "Invalid camera index.";
    }
}


// In processFrame function
void HAS_Main_GUI::processFrame(const QVideoFrame &frame) {
    if (!frame.isValid()) return;

    // Convert QVideoFrame to QImage
    QImage processedImage = frame.toImage();

    // Perform face detection using OpenCV (example code)
    std::vector<cv::Rect> faces;

    cv::Mat mat = QImageToMat(processedImage); // Convert to cv::Mat
    face_cascade.detectMultiScale(mat, faces);

    // Draw rectangles around detected faces
    for (const auto &face : faces) {
        cv::rectangle(mat, face, cv::Scalar(255, 0, 0), 2);
    }

    cv::imshow("test",mat);
    cv::waitKey(1);
    // Convert cv::Mat back to QImage
    processedImage = MatToQImage(mat);

    // Update the image widget to display the processed image
    imageWidget->setImage(processedImage);
}



cv::Mat HAS_Main_GUI::QImageToMat(const QImage &image) {
    return cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.bits(), image.bytesPerLine()).clone();
}

// QImage HAS_Main_GUI::MatToQImage(const cv::Mat &mat) {
//     switch (mat.type()) {
//     case CV_8UC3: // RGB
//         return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
//     case CV_8UC4: // RGBA
//         return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888);
//     case CV_8UC1: // Grayscale
//         return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
//     default:
//         qWarning() << "Unsupported cv::Mat format!";
//         return QImage();
//     }
// }


QImage HAS_Main_GUI::MatToQImage(const cv::Mat &mat) {

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


HAS_Main_GUI::~HAS_Main_GUI() {
    if (camera) {
        camera->stop();
        delete camera;
    }

    delete ui;
}


