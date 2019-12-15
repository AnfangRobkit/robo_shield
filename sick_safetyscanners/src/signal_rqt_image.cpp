#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include "sick_safetyscanners/SickSafetyscannersRos.h"
#include <vector>
#include <math.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>


using namespace std;
using namespace cv;
namespace enc = sensor_msgs::image_encodings;
#define PI 3.1415926

class Listener {
public:
    int flag;

    void scanValues(const sick_safetyscanners::OutputPathsMsg::ConstPtr &output_paths);

    Mat image_;

};

void Listener::scanValues(const sick_safetyscanners::OutputPathsMsg::ConstPtr &output_paths) {
    cv::Mat image(1024, 1024, CV_8UC3, Scalar(0));
    image_ = image;
    vector<unsigned char> status_now = vector<unsigned char>(20);
    status_now = output_paths->status;
    flag = 0;

    if (status_now[0] == false) {
        cv::circle(image, cv::Point(512, 512), 100, CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
        flag = 2;

    } else if ((status_now[1] == false)) {
        cv::circle(image, cv::Point(512, 512), 100, CV_RGB(255, 255, 0), CV_FILLED, 8, 0);
        flag = 1;
    } else {
        cv::circle(image, cv::Point(512, 512), 100, CV_RGB(0, 255, 0), CV_FILLED, 8, 0);
        flag = 1;
    }

    image_ = image;

}


int main(int argc, char **argv) {
    ros::init(argc, argv, "scan_image");
    ros::NodeHandle nh;
    ros::Rate loop_rate(30);
    Listener listener;
    ros::Subscriber sub = nh.subscribe<sick_safetyscanners::OutputPathsMsg>("/sick_safetyscanners/output_paths", 1,
                                                                            &Listener::scanValues, &listener);

//   For the image output
    ros::NodeHandle m;
    image_transport::ImageTransport it_(m);
    image_transport::Publisher image_pub_ = it_.advertise("/traj_output", 1000);
    cv_bridge::CvImagePtr cv_ptr(new cv_bridge::CvImage);


    while (ros::ok()) {

        if (listener.image_.rows > 60 && listener.image_.cols > 60) {

            ros::Time time = ros::Time::now();
            cv_ptr->encoding = "bgr8";
            cv_ptr->header.stamp = time;
            cv_ptr->header.frame_id = "/traj_output";
            cv_ptr->image = listener.image_;
            image_pub_.publish(cv_ptr->toImageMsg());
            ros::spinOnce();
            loop_rate.sleep();
        }

        ros::spinOnce();
    }
    return 0;
}

