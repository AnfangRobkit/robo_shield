#include "ros/ros.h"
//#include "std_msgs/String.h"
//#include "gazebo_msgs/ModelState.h"
#include "sensor_msgs/JointState.h"
#include "trajectory_msgs/JointTrajectory.h"
#include <sstream>
#include <math.h>


int main(int argc, char **argv) {

    ros::init(argc, argv, "pub");

    ros::NodeHandle n;

    ros::Publisher chatter_pub = n.advertise<trajectory_msgs::JointTrajectory>("/arm_controller/command", 1000);

    ros::Rate loop_rate(1);

    int count = 0;

    trajectory_msgs::JointTrajectory msg;


    //msg.header.stamp=ros::Time::now();

    //msg.name.resize(6);
    msg.joint_names.resize(6);
    msg.points.resize(1);

    msg.points[0].positions.resize(6);


    msg.joint_names[0] = "elbow_joint";
    msg.joint_names[1] = "shoulder_lift_joint";
    msg.joint_names[2] = "shoulder_pan_joint";
    msg.joint_names[3] = "wrist_1_joint";
    msg.joint_names[4] = "wrist_2_joint";
    msg.joint_names[5] = "wrist_3_joint";


    while (ros::ok()) {
        //ROS_INFO("kkkkk");

        msg.header.stamp = ros::Time::now();
        msg.points[0].time_from_start = ros::Duration(0.75);

        if (count % 3 == 0) {
            msg.points[0].positions[0] = +1.57;
            msg.points[0].positions[1] = -1.57;
            msg.points[0].positions[2] = 0;
            msg.points[0].positions[3] = 0;
            msg.points[0].positions[4] = 0;
            msg.points[0].positions[5] = 0;
        }

        if (count % 3 == 1) {
            msg.points[0].positions[0] = +2.0;
            msg.points[0].positions[1] = -1.2;
            msg.points[0].positions[2] = +1.57;
            msg.points[0].positions[3] = 0;
            msg.points[0].positions[4] = 0;
            msg.points[0].positions[5] = 0;
        }

        if (count % 3 == 2) {
            msg.points[0].positions[0] = +2.0;
            msg.points[0].positions[1] = -1.2;
            msg.points[0].positions[2] = -1.57;
            msg.points[0].positions[3] = 0;
            msg.points[0].positions[4] = 0;
            msg.points[0].positions[5] = 0;
        }


        chatter_pub.publish(msg);

        ros::spinOnce();

        loop_rate.sleep();

        ++count;
    }


    return 0;
}


