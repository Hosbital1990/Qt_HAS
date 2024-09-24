#include <opencv2/opencv.hpp>
#include "camera.h"
#include <thread>
#include <iostream>
#include <ncurses.h>
#include <mutex>
#include <ctime>
#include <sstream>
#include <condition_variable>

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

std::string pressed_key{""} ;  //later maybe chanage to recieved command
std::mutex pressed_key_mutex;
std::mutex opencv_function_access_mutex;
std::condition_variable pressed_key_condition ;
/**
 * @brief Implementation of Camera class
 *
 * @return true
 * @return false
 */

Camera::Camera(){}



bool Camera::camera_setup()
{
    // std::jthread doorbell_CAM_thread(Camera::camera_start, "doorbell", 2);
    // std::jthread security_CAM_thread(Camera::camera_start, "security", 0);
    // std::jthread liecen_thread (listen_command, std::ref(pressed_key));


    return true;
}

bool Camera::camera_start(const std::string& camera_name, int camera_ID){

    while(true){
        std::unique_lock<std::mutex> lock(pressed_key_mutex);
        pressed_key_condition.wait(lock,[&]{  // lambda function
            bool result = pressed_key == camera_name.substr(0,1);
            return result;
        });
        pressed_key.clear();
        lock.unlock(); // lets other parts access to share data

        std::cout << "Hello from " << camera_name <<" camera :)" << std::endl;
        // Open the default camera (camera index 0)
        cv::VideoCapture cap(camera_ID);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open the camera." << std::endl;
            // wait for some while to next try
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Load the pre-trained Haar Cascade XML classifier
        cv::CascadeClassifier face_cascade;
        if (!face_cascade.load("./haarcascade_frontalface_default.xml")) {
            std::cerr << "Error loading face cascade file!" << std::endl;
            // wait for some while to next try
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Get the width and height of the frames
        int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

        cv::VideoWriter writer(create_filename(camera_name), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(frameWidth, frameHeight));
        cv::Mat frame;
        std::vector<cv::Rect> faces;  // To hold recognized dimension

        while(pressed_key != camera_name.substr(0,1)){
            // Capture a framed
            cap >> frame;
            //      cap.read(frame);

            if (frame.empty()) {
                std::cerr << "Error: Empty frame." << std::endl;
                    // wait for some while to next try
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }

            // Detect faces in the image
            face_cascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size());
            // Draw rectangles around detected faces
            for (size_t i = 0; i < faces.size(); i++) {
                cv::rectangle(frame, faces[i], cv::Scalar(255, 137, 0), 2);
            }

            //cv functions are not thread safe to preventing error use mutex lock for cv::imshow and cv::waitkey
            std::unique_lock<std::mutex> opencv_lock(opencv_function_access_mutex);
            // Display the frame
            cv::imshow(camera_name+" Camera ", frame);
            //faces.clear();
            // Write the frame to the video file
            writer.write(frame);
            cv::waitKey(25);  //30 f/sec
            opencv_function_access_mutex.unlock();


        }  // End  while(pressed_key != "c")

        // Release the VideoCapture and VideoWriter objects
        cap.release();
        writer.release();
        // Destroy all OpenCV windows
        cv::destroyAllWindows();

        {
            std::unique_lock<std::mutex> lock(pressed_key_mutex);
                //Clear keyboard last pressed key
            pressed_key.clear();
        }

    } // end of first while(true)
    return true;
}

void Camera::listen_command(std::string& listen_char ){

    listen_char= "s";

}

bool Camera::camera_frame_read()
{


    return true;
}

bool Camera::camera_frame_save(){


    return true;
}

//This function create a name for saving video based on the system time and date
std::string Camera::create_filename(const std::string& cam_name){

    std::time_t now = std::time(nullptr);
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
    std::string filename;
    // Create the filename

    if(cam_name == "Doorbell"){

        filename = "./camera_capture/"+ cam_name +"/"+ std::string(buffer) + ".avi";

    }
    else if(cam_name == "Security"){

        filename = "./camera_capture/"+ cam_name +"/"+ std::string(buffer) + ".avi";

    }

    return filename;
}


Camera::~Camera(){}






