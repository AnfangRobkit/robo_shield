#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include <vector>
#include <math.h> 

using namespace std;
int Samples = 720;
#define PI 3.1415926

std::vector<float> old_range = std::vector<float>(Samples, 0);
std::vector<float> range = std::vector<float>(Samples, 0); 
std::vector<float> diff = std::vector<float>(Samples,0);

void scanValues(const sensor_msgs::LaserScan::ConstPtr& scan)
{   
    /*The Laser has 720 Samples */

    range = scan->ranges;
    double angle,x,y;
    float safe_d = 2;
    float warn_d = 1.5;
    float distance_now;
    //cout<<range[100]<<endl;
    for(int i = 1; i<=Samples;i++)
    {   
        diff[i] = range[i] - old_range[i];
       
        if ( abs(diff[i]) > 0.1 && abs(diff[i]) < 100 ) {
        /* When detect object, then calculate the distance and the judement*/
         /* angle = i * 270 / Samples * PI / 180; */
        angle =1.5*i*PI/Samples;
        
        x= range[i] * cos(PI-angle) -2;
        y= range[i] * sin(PI-angle) -2;
       
        distance_now = sqrt(x*x+y*y); 
        
        if(distance_now>safe_d)  {
                ROS_INFO("Safety!");
              
            }
           else  if(distance_now>warn_d&&distance_now<safe_d) {
          
                ROS_INFO("Critical!");                 
            }
            else if (distance_now<(warn_d)) {   
                ROS_INFO("Dangerous!");
            }
        
        }
        
    }
   old_range = range;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "scanner");
  ros::NodeHandle n;
  ros::Subscriber sub;
  //sub = n.subscribe("/mybot/laser/scan", 50, scanValues);
  sub = n.subscribe("/sick_safetyscanners/scan", 50, scanValues);
  
  
  ros::spin();
  return 0;
}

