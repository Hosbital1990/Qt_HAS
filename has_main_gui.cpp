
    #include "has_main_gui.h"
    #include "./ui_has_main_gui.h"
    #include "imagewidget.h" // Include the custom ImageWidget

    #include <QPushButton>
    #include <QVBoxLayout>
    #include <QHBoxLayout>
    #include <QDebug>
    #include <QComboBox>
    #include <QDir>
    #include <QSpacerItem>
    #include <QPainter>
    #include <QImage>
    #include <QtConcurrent/QtConcurrent>

    // OpenCV headers
    #include <opencv2/opencv.hpp>
    #include <opencv2/core.hpp>
    #include <opencv2/imgproc.hpp>
    #include <opencv2/highgui.hpp>
    #include <opencv2/objdetect.hpp> // For face detection

    // Standard library includes
    #include <vector>
    #include <iostream>
    #include <QTimer>
    #include <QElapsedTimer> // For measuring time intervals

    HAS_Main_GUI::HAS_Main_GUI(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::HAS_Main_GUI)
        , toggle_counter(0)
        ,cameraTimer(nullptr)

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
       // imageWidget->setMinimumSize(720,960);
        imageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QHBoxLayout* downsideLayout = new QHBoxLayout;
        downsideLayout->addWidget(imageWidget, 10);
        downsideLayout->addLayout(rightLayout, 1);

        // Create the main layout and add the image widget
        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addLayout(topLayout, 1);
        mainLayout->addLayout(downsideLayout, 50);

        ui->centralwidget->setLayout(mainLayout);

        // Initialize the available cameras
        cameraIdentifiers <<  "Camera 0" <<"Camera 2";

        // Populate the combo box with camera names
        cameraComboBox->addItems(cameraIdentifiers);
        cameraComboBox->setCurrentIndex(1);
        // Load the pre-trained Haar Cascade XML classifier
        if (!face_cascade.load("../../haarcascade_frontalface_default.xml")) {
            std::cerr << "Error loading face cascade file!" << std::endl;
            // wait for some while to next try
        }

        // Initialize the default camera
        if (!cameraIdentifiers.isEmpty()) {
            onCameraSelected(cameraComboBox->currentIndex());
        }
    }


    void HAS_Main_GUI::toggle_light() {
        // Toggle light functionality
    }

    void HAS_Main_GUI::onCameraSelected(int index) {
        if (videoCapture.isOpened()) {
            videoCapture.release(); // Release previous camera
        }
        // Stop and delete the existing timer (if any)
        if (cameraTimer) {
            cameraTimer->stop();
            delete cameraTimer;
            cameraTimer = nullptr;
        }
        // Open selected camera using OpenCV
        if (index >= 0 && index < cameraIdentifiers.size()) {
            int cameraIndex = 2*index; // Adjust this as per your camera setup
            videoCapture.open(cameraIndex);
            if (!videoCapture.isOpened()) {
                qWarning() << "Failed to open camera!";
                return;
            }

            // Start the frame processing loop using a lambda function with QtConcurrent::run
        //    QtConcurrent::run([this]() { processFrames(); });
            cameraTimer = new QTimer(this);
            connect(cameraTimer, &QTimer::timeout, [this](){

                QtConcurrent::run([this]() {
                    QMutexLocker locker(&mutex);  // Automatically locks the mutex, unlocks when out of scope

                    processFrames();

                });
            } );
            cameraTimer->start(33); // Adjust timer interval for 30 FPS
        } else {
            qWarning() << "Invalid camera index.";
        }
    }


    void HAS_Main_GUI::processFrames() {
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
         //   QImage processedImage = MatToQImage(frame);

            // Display the processed image in Qt
            imageWidget->setImage(processedImage);
        //}
            // Measure the time taken and output the result
            qint64 elapsed = timer.elapsed();
            qDebug() << "Frame processing time:" << elapsed << "milliseconds";

    }



    cv::Mat HAS_Main_GUI::QImageToMat(const QImage &image) {
        return cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine()).clone();
    }

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
        // Release the camera if it's still open
        if (videoCapture.isOpened()) {
            videoCapture.release();
        }

        // Stop and delete the camera timer if it exists
        if (cameraTimer) {
            cameraTimer->stop(); // Stop the timer before deleting
            delete cameraTimer;  // Delete the timer
            cameraTimer = nullptr;
        }

        // Clean up the UI
        delete ui;
    }
