#ifndef CAMERA_H
#define CAMERA_H

/**
 * @brief Use namsapace for camera functions!
 * @attention consider namespace instead of creating a class due to the simplicity!
 */
#include    <iostream>
#include <string>

#include <qobject.h>

class Camera : public QObject
{
    Q_OBJECT

 explicit Camera();
    ~Camera();

bool camera_setup();
bool camera_frame_read();

bool camera_frame_show();
bool camera_frame_save();

void listen_command(std::string& listen_char );

bool camera_start(const std::string& camera_name, int camera_ID);

//This function creat a name for saving video based on the system time and date
std::string create_filename(const std::string& cam_name);



};



#endif   // CAMERA_H
