#include "ros/ros.h"
//#include "std_msgs/String.h"
#include "gazebo_msgs/ModelState.h"
#include <sstream>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define freq 100 


class Path {

public:
  
  long count;
  unsigned int number_of_steps;
  float pose_f_x, pose_f_y;
  float pose_i_x, pose_i_y;
  float next_x, next_y;
  float step_x, step_y;
  float d_x, d_y;
  float t;
  float human_speed;  
  char flag;
   
  void path_calc(void);
  void path_next(void);
  void path_goal(void);
  void avoid_collision (void);
  float distance (float a, float b);
};

void Path::path_goal(void){
    
    ROS_INFO("B");

    srand(time(NULL));
    pose_f_x= -3 + 6* (float)rand()/RAND_MAX;
    pose_f_y= -3 + 6* (float)rand()/RAND_MAX;
    
    //sets new initial position
    pose_i_x=next_x;
    pose_i_y=next_y;    
}

void Path::path_calc(void){
    
  count=0;
    
  d_x= pose_f_x - pose_i_x;
  d_y= pose_f_y - pose_i_y;
  
  //calculates time to reach position
  t= distance (d_x,d_y) / human_speed;
  
  //calculates step distance in each axis
  number_of_steps=t*freq;
  step_x = d_x / number_of_steps;
  step_y = d_y / number_of_steps;
  
  flag=1;  
}

void Path::path_next(void){
    
    float nx, ny, d;
    
    if (flag ==1 && count <= number_of_steps){    
    
          
        nx = pose_i_x + count* step_x;
        ny = pose_i_y + count* step_y;
        
        d = distance (nx, ny);
        
        if( d <= 2 ){
            human_speed=0.75;
                        
            if(d<=1.5){
                flag=0;
            }
            else{
                next_x=nx;
                next_y=ny;
            }
        }
        
        else{
            human_speed=1.5;
            
            next_x=nx;
            next_y=ny;                                            
        }
        
        ++count;        
    }
    
    else
    {                        
        flag=0;
        //count=0;
    }
}

void Path::avoid_collision(void){
 
    if( sqrt( pow(next_x,2) +pow (next_y,2) ) < 1.2 ){
        flag=0;
        ROS_INFO("AAAAAA");
        //human_speed=0.75;
        //pose_i_x=next_x;
        //pose_i_y=next_y;
    }
    
    
        
}

float Path::distance (float a, float b){
    return sqrt( pow(a,2) +pow (b,2));
}


int main(int argc, char **argv)
{
    
  ros::init(argc, argv, "move_human");

  ros::NodeHandle n;

  ros::Publisher chatter_pub = n.advertise<gazebo_msgs::ModelState>("/gazebo/set_model_state", 1000);///gazebo/set_model_state

  ros::Rate loop_rate(freq);
  
    
  gazebo_msgs::ModelState msg;
  
  Path move;
  move.count=0;
  //move.pose_f_x=3;
  //move.pose_f_y=2;
  move.human_speed=1.5;
  move.flag=0;
  
  msg.model_name="human";//  abb_irb120_3_58

  msg.pose.position.x=2;
  msg.pose.position.y=0;
  msg.pose.position.z=0;

  msg.pose.orientation.x=0;
  msg.pose.orientation.y=0;
  msg.pose.orientation.z=0;
  msg.pose.orientation.w=1;	

  msg.twist.linear.x=0;
  msg.twist.linear.y=0;
  msg.twist.linear.z=0;

  msg.twist.angular.x=0;
  msg.twist.angular.y=0;
  msg.twist.angular.z=0;

  msg.reference_frame="world";
  
  move.pose_i_x=msg.pose.position.x;
  move.pose_i_y=msg.pose.position.y;
  
  //set for 1st loop
  move.next_x=move.pose_i_x;
  move.next_y=move.pose_i_y;
  
  while (ros::ok())
  {

    //ROS_INFO("%d %ld", number_of_steps, count);
    
    
    if(move.flag==0){
        
       move.path_goal();
       move.path_calc();     
                      
    }
    else{
       
    move.path_next();
    //move.avoid_collision();
    
    }
          
    msg.pose.position.x = move.next_x;	
    msg.pose.position.y = move.next_y;
    
    chatter_pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();

    
  }


  return 0;
}


