#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <vector>
#include <math.h> 

using namespace std;
using namespace sensor_msgs;
using namespace message_filters;
int Samples = 720;
#define PI 3.1415926

std::vector<float> old_range = std::vector<float>(Samples, 0);
std::vector<float> range = std::vector<float>(Samples, 0); 
std::vector<float> diff = std::vector<float>(Samples,0);

std::vector<float> old_range2 = std::vector<float>(Samples, 0);
std::vector<float> range2 = std::vector<float>(Samples, 0); 
std::vector<float> diff2 = std::vector<float>(Samples,0);

void scanValues(const sensor_msgs::LaserScan::ConstPtr& scan, const sensor_msgs::LaserScan::ConstPtr& scan2)

{   
    /*The Laser has 720 Samples */
    float safe_d = 1.8;
    float warn_d = 1.4;
    int flag_left, flag_right;
    range = scan->ranges;
    range2 = scan2->ranges;
    
    double angle,x,y;
    float distance_now;
    double angle2,x2,y2;
    float distance_now2;
    
    for(int i = 1; i<=Samples;i++)
    {   
        diff[i] = range[i] - old_range[i];
        diff2[i] = range2[i] - old_range2[i];
        if ( abs(diff[i]) > 0.1 && abs(diff[i]) < 100 ) {
        /* When detect object, then calculate the distance and the judement*/
         /* angle = i * 270 / Samples * PI / 180; */
        angle =1.5*i*PI/Samples;
        
        x= range[i] * cos(0.5*PI-angle) -2;
        y= range[i] * sin(0.5*PI-angle) +2;
       
        distance_now = sqrt(x*x+y*y); 
        // ROS_INFO("%d,angle %f,lx %f,ly %f",i,angle,x,y);
        /* if(distance_now>safe_d)  {
                ROS_INFO("Safety!");
            }
           else */
        if(distance_now>warn_d&&distance_now<safe_d) {
                flag_left = 1;                
            }
            else if (distance_now<(warn_d)) {   
                flag_left = 2; 
            }
        
        }
    if ( abs(diff2[i]) > 0.1 && abs(diff2[i]) < 100 ) {
        
        angle2 =1.5*i*PI/Samples;
        
        x2= range2[i] * cos(1.5*PI-angle2) +2;
        y2= range2[i] * sin(1.5*PI-angle2) -2;
       
        distance_now2 = sqrt(x2*x2+y2*y2); 
        
       // ROS_INFO("%d,angle %f,x %f,y %f",i,angle2,x2,y2);
       /* if(distance_now>safe_d)  {
                ROS_INFO("Safety!");
            }
           else */
        if(distance_now2>warn_d&&distance_now2<safe_d) {
            flag_right=1;   
                            
            }
            else if (distance_now2<(warn_d)) {   
                flag_right=2;
                
            }
    }
    }
    if(flag_left==2 || flag_right ==2) {
        ROS_INFO("Dangerous!");
       
    } else if(flag_left==1 || flag_right ==1) {
         ROS_INFO("Critical!"); 
    }
    
    
   old_range = range;
   old_range2 = range2;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "scanner");
  ros::NodeHandle n;

  message_filters::Subscriber<sensor_msgs::LaserScan> leftlaser_sub(n, "/mybot/laser/scan", 1000);
  message_filters::Subscriber<sensor_msgs::LaserScan> rightlaser_sub(n, "/mybot/laser/scan_right", 1000);
  TimeSynchronizer<sensor_msgs::LaserScan, sensor_msgs::LaserScan> sync(leftlaser_sub,  rightlaser_sub, 1000);
  sync.registerCallback(boost::bind(&scanValues, _1, _2));
  
  ros::spin();
  return 0;
}









