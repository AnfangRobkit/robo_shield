 
#ifndef MOVEARM_INCLUDEDEF_H
#define MOVEARM_INCLUDEDEF_H

#include "ros/ros.h"
#include "sensor_msgs/JointState.h"
#include "trajectory_msgs/JointTrajectory.h"
#include <sstream>
#include <math.h>


void JointStateCallBack(const sensor_msgs::JointState::ConstPtr& msg);
sensor_msgs::JointState urJointStates;
bool hasNewMsg=true;

#endif
