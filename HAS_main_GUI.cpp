#include "HAS_main_GUI.h"
#include "./ui_HAS_main_GUI.h"
#include "imagewidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QComboBox>
#include <QElapsedTimer>
#include <QThread>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QPixmap>

HAS_Main_GUI::HAS_Main_GUI(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::HAS_Main_GUI),
    toggle_counter(0),
    face_detection(false),
    cameraThread(nullptr),
    cameraTimer(nullptr),
    camera(nullptr)
{
    ui->setupUi(this);

    // Setup UI components
    QPushButton *button = new QPushButton("Toggle Light", this);
    button->setMinimumHeight(40);
    connect(button, &QPushButton::clicked, this, &HAS_Main_GUI::toggle_light);

    cameraComboBox = new QComboBox(this);
    cameraComboBox->setMinimumHeight(40);
    // Initialize available cameras
    cameraIdentifiers << "Camera 0" << "Camera 2";
    cameraComboBox->addItems(cameraIdentifiers);
    cameraComboBox->setCurrentIndex(1);
    connect(cameraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HAS_Main_GUI::onCameraSelected);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addSpacerItem(new QSpacerItem(100, 20));
    topLayout->addWidget(button);
    topLayout->addSpacerItem(new QSpacerItem(100, 20));
    topLayout->addWidget(cameraComboBox);
    topLayout->addSpacerItem(new QSpacerItem(100, 20));

    // Create the button and set its fixed size
    QPushButton *recordButton = new QPushButton("", nullptr);

    recordButton->setFixedSize(100, 30);  // Set desired button size

    // Load the original icon and check its size
    QPixmap originalPixmap("../../Media/Icon/rec-Icon-croped.png");
    qDebug() << "Original Icon Size:" << originalPixmap.size();

    // Scale the icon to cover the button's width
    QPixmap scaledPixmap = originalPixmap.scaled(80, 22, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    qDebug() << "Scaled Icon Size:" << scaledPixmap.size();  // Should be (100, 30)

    // Set the scaled icon to the button
    recordButton->setIcon(QIcon(scaledPixmap));
    recordButton->setIconSize(scaledPixmap.size());  // Set icon size to match the scaled pixmap

    // Print the icon size after setting
    qDebug() << "Button Icon Size:" << recordButton->iconSize();  // Should be (100, 30)

    connect(recordButton, &QPushButton::clicked, this, [this](){
        this->camera_record^=true;
        qDebug()<< "Face detection boolean is: " <<face_detection ;
    }
            );



    QPushButton *face_detectionButton = new QPushButton(QIcon("../../Media/Icon/face_recog_icon.png"),"Face Detection", this);


    connect(face_detectionButton, &QPushButton::clicked, this, [this](){
      this->face_detection^=true;
        qDebug()<< "Face detection boolean is: " <<face_detection ;
    }
            );

    QPushButton *snapshot_Button = new QPushButton("Snapshot", this);
    QPushButton *video_playback = new QPushButton("Playback", this);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(face_detectionButton);
    rightLayout->addWidget(recordButton);
    rightLayout->addWidget(snapshot_Button);
    rightLayout->addWidget(video_playback);

    imageWidget = new ImageWidget(this);
    imageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *downsideLayout = new QHBoxLayout();
    downsideLayout->addWidget(imageWidget, 10);
    downsideLayout->addLayout(rightLayout, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(downsideLayout);

    ui->centralwidget->setLayout(mainLayout);


    // Create and start a new camera thread
    camera = new Camera(cameraComboBox->currentIndex(), face_detection, camera_record, imageWidget);
    cameraThread = new QThread();
    // Move camera to the new thread
    camera->moveToThread(cameraThread);
    connect(cameraThread, &QThread::started, camera, &Camera::start_camera);
    // connect(cameraThread, &QThread::finished, cameraThread, &QObject::deleteLater);
    cameraThread->start();

}

void HAS_Main_GUI::toggle_light() {
    // Toggle light functionality
}

void HAS_Main_GUI::onCameraSelected(int index) {
    if (cameraThread) {
        // Gracefully stop the camera and thread
        camera->stop();
        cameraThread->quit();  // Gracefully request thread termination
        cameraThread->wait();  // Wait for the thread to finish

        // Clean up camera and thread using deleteLater()
       camera->deleteLater();  // Use deleteLater to ensure it is deleted after the event loop finishes
       cameraThread->deleteLater();

       cameraThread = nullptr;
        camera = nullptr;
    }
    // Create and start a new camera thread
    camera = new Camera(index, face_detection, camera_record, imageWidget);
    cameraThread = new QThread();
    // Move camera to the new thread
    camera->moveToThread(cameraThread);
    connect(cameraThread, &QThread::started, camera, &Camera::start_camera);
    cameraThread->start();

}


HAS_Main_GUI::~HAS_Main_GUI() {


    if (camera) {
        camera->stop();
        delete camera;
    }

    if (cameraThread) {
        cameraThread->quit();
        cameraThread->wait();
        delete cameraThread;
    }

    // Clean up UI
    delete ui;
}
