#include "ros/ros.h"
#include <iostream>
#include "visualization_msgs/Marker.h"
#include "visualization_msgs/MarkerArray.h"
#include <cmath>
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"


ros::Publisher g_marker_pub;
double zone_radius = 1.8;
double corner_x = 0.425;
double corner_y = -1.05;
float dangerous_state = 0.1, safe_state = 0.1, warning_state = 0.1;

int Samples = 720;
#define PI 3.1415926

std::vector<float> old_range = std::vector<float>(Samples, 0);
std::vector<float> range = std::vector<float>(Samples, 0);
std::vector<float> diff = std::vector<float>(Samples, 0);


void emitLineStrip(const std::string type_name, int32_t x_pos, float r, float g, float b, int k,
                   ros::Publisher &pub, float a = 1.0, std::string frame_id = std::string("/world")) {
    visualization_msgs::Marker marker;
    marker.header.frame_id = frame_id;
    ros::Time ros_time = ros::Time::now();
    marker.header.stamp = ros::Time::now();
    marker.ns = "marker_" + type_name;
    marker.id = 0;
    marker.type = visualization_msgs::Marker::LINE_STRIP;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = 0.0;
    marker.pose.position.y = 0.0;
    marker.pose.position.z = 0.0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 0.0;
    marker.pose.position.x = x_pos;
    marker.scale.x = 0.05;
    marker.color.a = a;
    marker.color.r = r;
    marker.color.g = g;
    marker.color.b = b;
    marker.frame_locked = false;

    double points_list_x[4] = {corner_x + k * zone_radius, corner_x + k * zone_radius, corner_x, -corner_x};
    double points_list_y[4] = {-corner_y, corner_y, corner_y - k * zone_radius, corner_y - k * zone_radius};

    for (int i = 0; i < 2; ++i) {
        geometry_msgs::Point p;
        p.x = points_list_x[i];
        p.y = points_list_y[i];
        p.z = 0;
        marker.points.push_back(p);
    }

    for (double temp_y = points_list_y[1] - 0.1; temp_y >= points_list_y[2];) {
        geometry_msgs::Point p;
        p.y = temp_y;
        double temp_x = sqrt(k * k * zone_radius * zone_radius - (temp_y - corner_y) * (temp_y - corner_y)) + corner_x;
        if (!isnan(temp_x)) {
            p.x = temp_x;
        } else {
            ROS_INFO("NaN");
        }
        p.z = 0;
        marker.points.push_back(p);
        temp_y -= 0.1;
    }

    for (int i = 2; i < 4; ++i) {
        geometry_msgs::Point p;
        p.x = points_list_x[i];
        p.y = points_list_y[i];
        p.z = 0;
        marker.points.push_back(p);
    }

    pub.publish(marker);
}

void emitTableLine(int x_pos) {

    visualization_msgs::Marker marker;
    marker.header.frame_id = "/world";
    marker.header.stamp = ros::Time::now();
    marker.ns = "marker_table";
    marker.id = 0;
    marker.type = visualization_msgs::Marker::LINE_STRIP;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = 0.0;
    marker.pose.position.y = 0.0;
    marker.pose.position.z = 0.0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 0.0;
    marker.pose.position.x = x_pos;
    marker.scale.x = 0.05;
    marker.color.a = 0.3;
    marker.color.r = 1;
    marker.color.g = 0;
    marker.color.b = 0;
    marker.frame_locked = false;

    double points_list_x[3] = {corner_x, corner_x, -corner_x};
    double points_list_y[3] = {-corner_y, corner_y, corner_y};
    for (int i = 0; i < 3; ++i) {
        geometry_msgs::Point p;
        p.x = points_list_x[i];
        p.y = points_list_y[i];
        p.z = 0;
        marker.points.push_back(p);
    }

    g_marker_pub.publish(marker);
}

void publishCallback(const ros::TimerEvent &) {
    //ROS_INFO("Publishing");
    int32_t x_pos = 0.1;
    emitTableLine(x_pos);
    emitLineStrip("dangerous", x_pos, 1, 0, 0, 1, g_marker_pub, dangerous_state);
    emitLineStrip("warning", x_pos, 1, 1, 0.5, 2, g_marker_pub, warning_state);
    emitLineStrip("safety", x_pos, 0, 1, 0, 3, g_marker_pub, safe_state);
}

void scanValues(const sensor_msgs::LaserScan::ConstPtr &scan) {
    /*The Laser has 720 Samples */
    range = scan->ranges;
    double angle, x, y;
    float safe_d = 4;
    float warn_d = 2;
    float distance_now;

    for (int i = 1; i <= Samples; i++) {
        diff[i] = range[i] - old_range[i];
        if (abs(diff[i]) > 0.1 && abs(diff[i]) < 100) {
            /* When detect object, then calculate the distance and the judement*/
            /* angle = i * 270 / Samples * PI / 180; */
            angle = 1.5 * i * PI / Samples;

            x = range[i] * cos(angle + PI / 4) + corner_x;
            y = range[i] * sin(angle + PI / 4) + corner_y;

            distance_now = sqrt(x * x + y * y);

            if (distance_now > safe_d) {
                dangerous_state = 0.1;
                warning_state = 0.1;
                safe_state = 1.0;
            } else if (distance_now > (warn_d)) {
                dangerous_state = 0.1;
                warning_state = 1.0;
                safe_state = 0.1;
            } else {
                dangerous_state = 1.0;
                warning_state = 0.1;
                safe_state = 0.1;
            }
        }
    }
    old_range = range;
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "marker");
    ros::NodeHandle n;

    g_marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 0);
    ros::Timer publish_timer = n.createTimer(ros::Duration(1), publishCallback);

    ros::Subscriber sub;
    sub = n.subscribe("/mybot/laser/scan_right", 50, scanValues);

    ros::Duration(0.1).sleep();

    ros::spin();
}
