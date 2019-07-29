#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include "sick_safetyscanners/SickSafetyscannersRos.h"
#include <vector>
#include <math.h> 


#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>

using namespace std;
using namespace cv;
namespace enc = sensor_msgs::image_encodings;

vector<unsigned char> status_now = vector<unsigned char>(20); 

int Trigger = 0;

void scanValues(const sick_safetyscanners::OutputPathsMsg::ConstPtr& output_paths)
{   //cout<<Trigger<<"!!!!!!"<<endl;
    /*The Laser has 720 Samples */
    status_now = output_paths->status;
    //cout<<status_now[1]<<endl;
    if(status_now[0]==false) 
    {	
    	cv::circle(image, cv::Point(512,215),100,CV_RGB(255,0,0), CV_FILLED,8,0);
        cout<<"Dangours"<<endl;
        Trigger = 0;

        //cout<<status_now[0]<<endl;
    }
    else if(status_now[1]==false) 
    	{
    	cout<<"Critial"<<endl;
    	cv::circle(image, cv::Point(512,215),100,CV_RGB(0,0,255), CV_FILLED,8,0);
    	Trigger = 1;
    	}
    else 
    	{
    	cout<<"Safety"<<endl;
    	cv::circle(image, cv::Point(512,215),100,CV_RGB(0,255,0), CV_FILLED,8,0);
    	Trigger = 2;
    	}
    ros::spin();
}

int main(int argc, char **argv)
{
    
	//cv::circle(image, cv::Point(512,215),100,CV_RGB(255,0,0), CV_FILLED,8,0);
    ros::init(argc, argv, "draw_circle");
    ros::NodeHandle m;
    image_transport::ImageTransport it_(m);
    image_transport::Publisher image_pub_ = it_.advertise("/traj_output", 1000);
    cv_bridge::CvImagePtr cv_ptr(new cv_bridge::CvImage);
    //ros::Rate loop_rate(10); //10 hz
    
    ros::Time time = ros::Time::now();
    cv_ptr->encoding = "bgr8";
    cv_ptr->header.stamp = time;
    cv_ptr->header.frame_id = "/traj_output";
    cv_ptr->image = image;
    image_pub_.publish(cv_ptr->toImageMsg());
    //ROS_INFO("ImageMsg Sent.");
    //ROS_INFO("Subscribers: %d", image_pub_.getNumSubscribers());
    cout<<Trigger<<"!!!"<<endl;
    ros::NodeHandle n;
    ros::Subscriber sub;
    sub = n.subscribe("/sick_safetyscanners/output_paths", 10, scanValues);
    

    ros::spin();
    return 0;
 
}
