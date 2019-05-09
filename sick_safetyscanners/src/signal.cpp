 
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include "sick_safetyscanners/SickSafetyscannersRos.h"
#include <vector>
#include <math.h> 

using namespace std;
int Samples = 720;
#define PI 3.1415926

// The Sensor angle range is from -2/3 PI to 2/3PI, and the angle increment is  0.00672220578417 Deg
//std::vector<float> old_range = std::vector<float>(Samples, 0);
//std::vector<float> range = std::vector<float>(Samples, 0); 
vector<unsigned char> status_now = vector<unsigned char>(20); 

void scanValues(const sick_safetyscanners::OutputPathsMsg::ConstPtr& output_paths)
{   
    /*The Laser has 720 Samples */
    status_now = output_paths->status;
    //cout<<status_now[1]<<endl;
    if(status_now[0]==false) 
    {
        cout<<"Dangerous"<<endl;
        //cout<<status_now[0]<<endl;
    }
    else if(status_now[1]==false) cout<<"Critial"<<endl;
    //else cout<<"Safety"<<endl;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "static");
  ros::NodeHandle n;
  ros::Subscriber sub;
  sub = n.subscribe("/sick_safetyscanners/output_paths", 50, scanValues);
  
  
  ros::spin();
  return 0;
}

