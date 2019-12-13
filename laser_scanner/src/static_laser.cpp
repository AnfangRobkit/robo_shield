
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include <vector>
#include <math.h>

using namespace std;
int Samples = 720;
#define PI 3.1415926
// The Sensor angle range is from -2/3 PI to 2/3PI, and the angle increment is  0.00672220578417 Deg
std::vector<float> old_range = std::vector<float>(Samples, 0);
std::vector<float> range = std::vector<float>(Samples, 0);
std::vector<float> diff = std::vector<float>(Samples, 0);

void scanValues(const sensor_msgs::LaserScan::ConstPtr &scan) {
    range = scan->ranges;
    double angle, x, y;
    float safe_d = 0.5;
    float warn_d = 0.1;
    float distance_now;
    for (int i = 1; i <= Samples; i++) {
        diff[i] = range[i] - old_range[i];
        if (abs(diff[i]) > 0.1 && abs(diff[i]) < 100) {
            distance_now = diff[i];
            if (distance_now > safe_d) {
                ROS_INFO("Safety!");
            } else if (distance_now > warn_d && distance_now < safe_d) {
                ROS_INFO("Critical!");
            } else if (distance_now < (warn_d)) {
                ROS_INFO("Dangerous!");
            }
        }
    }
    old_range = range;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "static");
    ros::NodeHandle n;
    ros::Subscriber sub;
    sub = n.subscribe("/sick_safetyscanners/scan", 50, scanValues);
    ros::spin();
    return 0;
}

