
#include <robo_shield_pkg/move_arm.h>

/**
 *@brief this file used to remap the motion of the real-world hardware to the
 * gazebo environment
 * */

int main(int argc, char **argv) {

    ros::init(argc, argv, "ur_sim");
    ros::NodeHandle nh;
    ros::Publisher JointControlpub = nh.advertise<trajectory_msgs::JointTrajectory>("arm_controller/command", 1);
    ros::Subscriber JointStateSubscriber = nh.subscribe<sensor_msgs::JointState>("/joint_states", 100,
                                                                                 JointStateCallBack);

    trajectory_msgs::JointTrajectory RealRobotState;

    RealRobotState.points.resize(1);

    RealRobotState.points[0].positions.resize(6);

    while (ros::ok()) {
        while (hasNewMsg) {
            RealRobotState.header.stamp = ros::Time::now();
            RealRobotState.joint_names = urJointStates.name;
            RealRobotState.points[0].time_from_start = ros::Duration(0.75);
            RealRobotState.points[0].positions = urJointStates.position;

            JointControlpub.publish(RealRobotState);
            hasNewMsg = false;

        }
        ros::spinOnce();
    }

    return 0;
}

void JointStateCallBack(const sensor_msgs::JointState::ConstPtr &msg) {
    urJointStates = *msg;
    hasNewMsg = true;
}
