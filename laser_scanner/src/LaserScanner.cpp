#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
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
int Samples = 720;
#define PI 3.1415926

cv::Mat image_new(1024, 1024, CV_8UC3, Scalar(0));

class Listener {
public:
    int flag;

    void scanValues(const sensor_msgs::LaserScan::ConstPtr &scan);

    Mat image_;

};

void Listener::scanValues(const sensor_msgs::LaserScan::ConstPtr &scan) {
    cv::Mat image(1024, 1024, CV_8UC3, Scalar(0));
    image_ = image;
    std::vector<float> range = std::vector<float>(Samples, 0);
    range = scan->ranges;
    double angle, x, y;
    float safe_d = 3.5;
    float warn_d = 1.5;
    float distance_now;
    flag = 0;
    //cout<<range[100]<<endl

    for (int i = 1; i <= Samples; i++) {

        if (abs(range[i]) < 100 && abs(range[i]) > 0.1) {
            distance_now = range[i];


            if (distance_now < (warn_d)) {
//                 ROS_INFO("Dangerous!");
                cv::circle(image, cv::Point(512, 512), 100, CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
                flag = 2;

                break;
            } else if (distance_now > warn_d && distance_now < safe_d) {

                cv::circle(image, cv::Point(512, 512), 100, CV_RGB(255, 255, 0), CV_FILLED, 8, 0);
//                 ROS_INFO("Critical!");      
                flag = 1;
                break;
            } else if (distance_now > safe_d) {
                cv::circle(image, cv::Point(512, 512), 100, CV_RGB(0, 255, 0), CV_FILLED, 8, 0);
//                 ROS_INFO("Critical!");      
                flag = 1;
            }
        }
    }
    image_ = image;

}


int main(int argc, char **argv) {
    ros::init(argc, argv, "scanner");
    ros::NodeHandle nh;
    ros::Rate loop_rate(30);
    Listener listener;
    ros::Subscriber sub = nh.subscribe<sensor_msgs::LaserScan>("/mybot/laser/scan_right", 1, &Listener::scanValues,
                                                               &listener);

//   For the image output
    ros::NodeHandle m;
    image_transport::ImageTransport it_(m);
    image_transport::Publisher image_pub_ = it_.advertise("/traj_output", 1000);
    cv_bridge::CvImagePtr cv_ptr(new cv_bridge::CvImage);


    while (ros::ok()) {

        if (listener.image_.rows > 60 && listener.image_.cols > 60) {

            ROS_INFO("Image Here");
            ros::Time time = ros::Time::now();
            cv_ptr->encoding = "bgr8";
            cv_ptr->header.stamp = time;
            cv_ptr->header.frame_id = "/traj_output";

            cv_ptr->image = listener.image_;
            image_pub_.publish(cv_ptr->toImageMsg());
            ros::spinOnce();
            loop_rate.sleep();
        }
        //cv::imshow("image_window", image_new);

        ros::spinOnce();
    }
//         cv::destroyWindow("image_window"); 

    return 0;
}

