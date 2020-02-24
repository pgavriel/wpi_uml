/*
Arm Controller for WPI HRI Synchrony Study
Written by Peter Gavriel - pgavriel@mail.middlesex.edu
*/
// ROS
#include <ros/ros.h>
#include <ros/console.h>
#include "std_msgs/String.h"

// MoveIt!
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit/move_group_interface/move_group_interface.h>

// TF2
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

#include <iostream>
#include <exception>

//#include <manipulation_class.hpp>

const int CLOSED = 170;

int rowSize = 4;
bool newMsg = false;
int position;
int cmdNum;
std::string attachedObject = "";
std::string target = "";
std::string message = "";
bool load_collisions, load_table;

void addCollisionObjects(moveit::planning_interface::PlanningSceneInterface& planning_scene_interface)
{
  int blocks = 16;       //Number of cubes to systematically add
  float cubeSize = 0.02; //Size of cubes
  int perRow = rowSize;  //Number of cubes per row, defined globally

  float cubeOffset = cubeSize * 3.5 + 0.02; //Spacing between cubes

  //Define x & y origin to start drawing cubes
  float xOri, yOri, xPos, yPos;
  xPos = xOri = -0.5;
  yPos = yOri = (((cubeOffset * perRow)/ 2)-(cubeSize*2));

  float tableHeight = -0.6; //tableHeight ( ͠° ͟ʖ ͡°)

  // Create vector to hold the collision objects.
  std::vector<moveit_msgs::CollisionObject> collision_objects;
  collision_objects.resize(blocks+1);

  //Add the table
  collision_objects[0].id = "table";
  collision_objects[0].header.frame_id = "base_link";

  collision_objects[0].primitives.resize(1);
  collision_objects[0].primitives[0].type = collision_objects[0].primitives[0].BOX;
  collision_objects[0].primitives[0].dimensions.resize(3);
  collision_objects[0].primitives[0].dimensions[0] = 0.6;
  collision_objects[0].primitives[0].dimensions[1] = 0.8;
  collision_objects[0].primitives[0].dimensions[2] = tableHeight;

  collision_objects[0].primitive_poses.resize(1);
  collision_objects[0].primitive_poses[0].position.x = -0.625;
  collision_objects[0].primitive_poses[0].position.y = 0;
  collision_objects[0].primitive_poses[0].position.z = tableHeight / 2;

  collision_objects[0].operation = collision_objects[0].ADD;

  //Add Cubes
  ROS_INFO("Generating Collision Cubes...");
  for(int i = 1 ; i <= blocks ; i++){
    std::stringstream cubeName;
    collision_objects[i].header.frame_id = "base_link";
    cubeName << "cube" << i;
    collision_objects[i].id = cubeName.str();
    //Define Primitive and its dimensions
    collision_objects[i].primitives.resize(1);
    collision_objects[i].primitives[0].type = collision_objects[1].primitives[0].BOX;
    collision_objects[i].primitives[0].dimensions.resize(3);
    collision_objects[i].primitives[0].dimensions[0] = cubeSize;
    collision_objects[i].primitives[0].dimensions[1] = cubeSize;
    collision_objects[i].primitives[0].dimensions[2] = cubeSize;
    //Define the pose
    collision_objects[i].primitive_poses.resize(1);
    collision_objects[i].primitive_poses[0].position.x = xPos;
    collision_objects[i].primitive_poses[0].position.y = yPos;
    collision_objects[i].primitive_poses[0].position.z = (cubeSize / 2)+ 0.01;
    //Add cube
    ROS_INFO("i: %d   xPos: %f   yPos: %f",i,xPos,yPos);
    collision_objects[i].operation = collision_objects[i].ADD;
    //Adjust x and y positions
    if(i%perRow == 0){
      xPos -= cubeOffset;
      yPos = yOri;
    }else{
      yPos -= cubeOffset;
    }
  }
  //Apply all defined collision objects to planning scene
  planning_scene_interface.applyCollisionObjects(collision_objects);
}

void addTableCollision(moveit::planning_interface::PlanningSceneInterface& planning_scene_interface){
  float tableHeight = -0.6; //tableHeight ( ͠° ͟ʖ ͡°)

  std::vector<moveit_msgs::CollisionObject> collision_objects;
  collision_objects.resize(1);

  //Add the table
  collision_objects[0].id = "table";
  collision_objects[0].header.frame_id = "base_link";

  collision_objects[0].primitives.resize(1);
  collision_objects[0].primitives[0].type = collision_objects[0].primitives[0].BOX;
  collision_objects[0].primitives[0].dimensions.resize(3);
  collision_objects[0].primitives[0].dimensions[0] = 0.6;
  collision_objects[0].primitives[0].dimensions[1] = 0.8;
  collision_objects[0].primitives[0].dimensions[2] = tableHeight;

  collision_objects[0].primitive_poses.resize(1);
  collision_objects[0].primitive_poses[0].position.x = -0.625;
  collision_objects[0].primitive_poses[0].position.y = 0;
  collision_objects[0].primitive_poses[0].position.z = tableHeight / 2;

  collision_objects[0].operation = collision_objects[0].ADD;

  planning_scene_interface.applyCollisionObjects(collision_objects);
}

void gotoPosition(moveit::planning_interface::MoveGroupInterface& group){//, Manipulation manip){
  std::string recievedTarget = target;
  if (cmdNum == 1){ //HOVER (Up Position)
    recievedTarget = target+"Up";
    ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
    group.setNamedTarget(recievedTarget);
    group.move();
  }else if (cmdNum == 2){//Pick up (Up, Down, Grab, Up)
    recievedTarget = target+"Up";
    ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
    group.setNamedTarget(recievedTarget);
    group.move();
    //IMPLEMENT OPEN GRIPPER
    //manipulation.gripper_open();
    //manipulation.gripper_command.publish(manipulation.command);
    recievedTarget = target+"Down";
    ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
    group.setNamedTarget(recievedTarget);
    group.move();
    ros::WallDuration(1.0).sleep();
    //IMPLEMENT CLOSE GRIPPER
    //manipulation.gripper_set(CLOSED);
    //manipulation.gripper_command.publish(manipulation.command);
    if(load_collisions){
      attachedObject = "cube" + std::to_string(position+1);
      ROS_INFO("Attaching object \"%s\"",attachedObject.c_str());
      group.attachObject(attachedObject,"ee_link");
    }
    recievedTarget = target+"Up";
    ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
    group.setNamedTarget(recievedTarget);
    group.move();
  }else if (cmdNum == 3){//Show Player
    group.setNamedTarget("PLAYER");
    group.move();
  }else if (cmdNum == 4){//Show self
    group.setNamedTarget("SELF");
    group.move();
  }else if (cmdNum == 5){//Move to drop off
    group.setNamedTarget("DESTROY");
    group.move();
  }
}

void gotoUtility(moveit::planning_interface::MoveGroupInterface& group){
  ROS_INFO("Going to Utility Target: %s",target.c_str());
  group.setNamedTarget(target);
  group.move();
}

void commandCallback(const std_msgs::String::ConstPtr& msg)
{
  //This program assumes codes sent in the form of two characters followed
  //by a dash, followed by an integer (i.e.- A1-2, XH-12)
  ROS_INFO("- - - - - - - - - - - - - - - - - - - - - -");
  std::string data = msg->data.c_str();
  try{
    std::size_t pos = data.find("-"); //Find '-' delimiter
    cmdNum = std::stoi(data.substr(pos+1)); //Set cmdNum to the number following the delimiter
    std::string rows = "ABCD"; //Define which Row characters are valid positions
    char row = data[0];
    char col = data[1];
    position = -1;
    message = "";

    if(rows.find(row) != std::string::npos){ //If character Row is contained in valid rows
      target = "";
      position += (int)col-'0';
      switch(row){
        case 'A': target.append("A"); break;
        case 'B': target.append("B"); position += rowSize; break;
        case 'C': target.append("C"); position += rowSize*2; break;
        case 'D': target.append("D"); position += rowSize*3; break;
        default: ROS_WARN("Row not recognized");
      }
      //Position now equals (row#-1*rowsize)+col value-1 (to make it proper array index, 1=0, 2=1, etc.)
      //Note that position is no longer that useful since implementing the use of setNamedTarget(),
      //    but may still come in handy.
      int pos = (int)col-'0';
      target.append(std::to_string(pos)+"_");
      //In this case, row==X is intended to mean anything other than a board position command
    }else if(row == 'X'){ //Special command
        switch(col){
          case 'H': target = "HOME"; position = -1; break; //HOME
          case 'D': target = "DESTROY"; position = -1; break; //DROP OFF AKA DESTROY
          case 'W': //A messy way of manually oscillating between up and down of the last board position
                   if(rows.find(target[0]) != std::string::npos && target != "DESTROY"){
                     std::string s;
                     if(target.find("Down") != std::string::npos){
                       s = "Up";
                     }else{
                       s = "Down";
                     }
                     pos = target.find("_")+1;
                     target = target.substr(0,pos);
                     ROS_INFO("Target after substr: %s",target.c_str());
                     target.append(s);
                     ROS_INFO("Target after append: %s",target.c_str());
                     position = -1;
                    }
                    break; //Move down at current position
          case '1': target = "PLAYER1"; position = -1; break; //Show PLAYER1
          case '2': target = "PLAYER2"; position = -1; break; //Show PLAYER2
          case 'S': target = "SELF"; position = -1; break; //Show Self
          case 'O': position = -3; break; //Open Gripper
          case 'C': position = -4; break; //Close Gripper
          case 'R': ROS_INFO("RESET CLICK COUNTER"); return; //RESET (Handled by gui)
        }
    }else if(row == 'M'){
      position = -5;
      switch(col){ //Message Command
        case '1': message = "Test message 1."; break;
        case '2': message = "Test message 2."; break;
        case '3': message = "Test message 3."; break;
        default:  message = "Unknown message."; break;
      }
      message.append(std::to_string(col-'0'));
    }
    //Output recieved message and parsed information for debugging
    ROS_INFO("GUI MSG: [%s]", data.c_str());
    ROS_INFO("PARSED: Row=%c  Col=%c  Num=%d  Pos=%d",row,col,cmdNum,position);
    ROS_INFO("TARGET: %s",target.c_str());

    //Tell main a new message was recieved
    newMsg = true;
    return;
  }catch(std::exception e){
    ROS_WARN("Something went wrong: %s", e.what());
    return;
  }
}

int main(int argc, char** argv)
{
  ROS_INFO("Starting...");
  ros::init(argc, argv, "wpi_controller");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  //Manipulation manipulation(nh);

  ros::Subscriber sub = nh.subscribe("arm_command", 1000, commandCallback);
  ros::Publisher msg_sender = nh.advertise<std_msgs::String>("text_display", 1000);

  nh.getParam(ros::this_node::getName()+"/LCO",load_collisions);
  nh.getParam(ros::this_node::getName()+"/load_table",load_table);
  ROS_INFO("Load Collision Objects: %s",load_collisions?"YES":"NO");
  ROS_INFO("Load Table: %s",load_table?"YES":"NO");

  ros::WallDuration(1.0).sleep();

  //manipulation.activate_gripper();
  //manipulation.gripper_command.publish(manipulation.command);
  ros::Duration(1.5).sleep();

  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
  moveit::planning_interface::MoveGroupInterface group("manipulator");
  float v_scale, plan_time;
  nh.getParam(ros::this_node::getName()+"/v_scale",v_scale);
  group.setMaxVelocityScalingFactor(v_scale);
  nh.getParam(ros::this_node::getName()+"/plan_time",plan_time);
  group.setPlanningTime(plan_time);
  ROS_INFO("Velocity Scaling: %.2f    Plan Time: %.2f",v_scale,plan_time);

  if(load_collisions)
    addCollisionObjects(planning_scene_interface);
  else if(load_table)
    addTableCollision(planning_scene_interface);

  ROS_INFO("DONE. Listening...");
  while(ros::ok()){
    if(newMsg){ //New message recieved from callback, figure out what to do.
      if(position>=0) //Must be a board position
        gotoPosition(group);//,manipulation);
      else if (position == -1){ //Must be a utility position (home,dropoff,etc)
        gotoUtility(group);
      }else if(position == -2){
        gotoUtility(group);
      }else if (position == -3){ //Open Gripper
        //manipulation.gripper_open();
        //manipulation.gripper_command.publish(manipulation.command);
        if(attachedObject != ""){ //If there's a collision object attached, remove it (drop it)
          std::vector<std::string> toRemove;
          toRemove.resize(1);
          toRemove[0] = attachedObject;
          group.detachObject(attachedObject);
          ROS_INFO("Removing object \"%s\"", toRemove[0].c_str());
          planning_scene_interface.removeCollisionObjects(toRemove);
          attachedObject = "";
        }
      }else if (position == -4){ //Close Gripper
        //manipulation.gripper_set(CLOSED);
        //manipulation.gripper_command.publish(manipulation.command);
      }else if (position == -5){ //Display a message
        ROS_INFO("ToImplement: Show Message %s",message.c_str());
        std_msgs::String msg;
        msg.data = message;
        msg_sender.publish(msg);
      }else
        ROS_INFO("Message unknown. No action taken.");

      newMsg = false;
    }
  }
  std::cout << "\nExiting..." << std::endl;
  ros::WallDuration(0.5).sleep();
  return 0;
}
/*
POSITION CODES:
-4 : Display message
-3 : Close Gripper
-2 : Open Gripper
-1 : Utility positions
0-15 : Block positions A1-D4
*/
