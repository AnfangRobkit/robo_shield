#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include "sick_safetyscanners/SickSafetyscannersRos.h"
#include <vector>
#include <math.h>

#include "robo_shield_pkg/signal.h"


using namespace std;
using namespace cv;
namespace enc = sensor_msgs::image_encodings;
#define PI 3.1415926


int main(int argc, char **argv) {
    ros::init(argc, argv, "scan_image");
    ros::NodeHandle nh;
    ros::Rate loop_rate(30);
    RoboShieldListener listener;
    ros::Subscriber sub = nh.subscribe<sick_safetyscanners::OutputPathsMsg>("/sick_safetyscanners/output_paths", 1,
                                                                            &RoboShieldListener::scanValues, &listener);

//   For the image output
    ros::NodeHandle m;
    image_transport::ImageTransport it_(m);
    image_transport::Publisher image_pub_ = it_.advertise("/traj_output", 1000);
    cv_bridge::CvImagePtr cv_ptr(new cv_bridge::CvImage);


    while (ros::ok()) {

        if (RoboShieldListener.image_.rows > 60 && RoboShieldListener.image_.cols > 60) {

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

