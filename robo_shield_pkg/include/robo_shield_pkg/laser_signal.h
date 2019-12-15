#ifndef SIGNAL_INCLUDE_H
#define SIGNAL_INCLUDE_H

#include "sick_safetyscanners/sick_safetyscanners.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>

/**
 * @brief This node receives the laser output from the topic
 * and after calculate the distance between robot and human to give the signal output in image
 * Red indicates dangerous, Yello indicates critical, Green indicates Safety
 **/
class RoboShieldListener {

public:
    RoboShieldListener();

    int flag;

    void scanValues(const sick_safetyscanners::OutputPathsMsg::ConstPtr &output_paths);

    Mat image_;

};

RoboShieldListener::RoboShieldListener() {
    flag = 0;
    cv::Mat image(1024, 1024, CV_8UC3, Scalar(0));
    image_ = image;
}

void RoboShieldListener::scanValues(const sick_safetyscanners::OutputPathsMsg::ConstPtr &output_paths) {

    vector<unsigned char> status_now = vector<unsigned char>(20);
    status_now = output_paths->status;
    flag = 0;

    if (status_now[0] == false) {
        cv::circle(image_, cv::Point(512, 512), 100, CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
        flag = 2;
    } else if ((status_now[1] == false)) {
        cv::circle(image_, cv::Point(512, 512), 100, CV_RGB(255, 255, 0), CV_FILLED, 8, 0);
        flag = 1;
    } else {
        cv::circle(image_, cv::Point(512, 512), 100, CV_RGB(0, 255, 0), CV_FILLED, 8, 0);
        flag = 1;
    }
}

#endif
