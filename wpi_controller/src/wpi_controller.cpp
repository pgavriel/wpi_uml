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

#include <manipulation_class.hpp>

//Velocity scaling may change how the gripper behaves
// I believe the range is 0 (OPEN) to 255 (Fully closed)
// For scaling 0.1, CLOSED = 170;
const int CLOSED = 170;

int rowSize = 4;
bool newMsg = false;
int position;
int cmdNum;
std::string attachedObject = "";
std::string target = "";
std_msgs::String message;
bool load_collisions, load_table, load_safety;

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
  collision_objects[0].primitives[0].dimensions[0] = 0.0;
  collision_objects[0].primitives[0].dimensions[1] = 1.2;
  collision_objects[0].primitives[0].dimensions[2] = tableHeight;

  collision_objects[0].primitive_poses.resize(1);
  collision_objects[0].primitive_poses[0].position.x = -0.55;
  collision_objects[0].primitive_poses[0].position.y = 0;
  collision_objects[0].primitive_poses[0].position.z = (tableHeight / 2) - 0.03;

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
  collision_objects[0].primitives[0].dimensions[0] = 0.9;
  collision_objects[0].primitives[0].dimensions[1] = 1.2;
  collision_objects[0].primitives[0].dimensions[2] = tableHeight;

  collision_objects[0].primitive_poses.resize(1);
  collision_objects[0].primitive_poses[0].position.x = -0.55;
  collision_objects[0].primitive_poses[0].position.y = 0;
  collision_objects[0].primitive_poses[0].position.z = (tableHeight / 2) - 0.03;

  collision_objects[0].operation = collision_objects[0].ADD;

  planning_scene_interface.applyCollisionObjects(collision_objects);
}

void addSafetyCollision(moveit::planning_interface::PlanningSceneInterface& planning_scene_interface){
  std::vector<moveit_msgs::CollisionObject> collision_objects;
  collision_objects.resize(2);

  collision_objects[0].id = "safety_player1";
  collision_objects[0].header.frame_id = "base_link";

  collision_objects[0].primitives.resize(1);
  collision_objects[0].primitives[0].type = collision_objects[0].primitives[0].BOX;
  collision_objects[0].primitives[0].dimensions.resize(3);
  collision_objects[0].primitives[0].dimensions[0] = 0.5;
  collision_objects[0].primitives[0].dimensions[1] = 0.5;
  collision_objects[0].primitives[0].dimensions[2] = 1.0;

  collision_objects[0].primitive_poses.resize(1);
  collision_objects[0].primitive_poses[0].position.x = -1.25;
  collision_objects[0].primitive_poses[0].position.y = 0.35;
  collision_objects[0].primitive_poses[0].position.z = 0;

  collision_objects[0].operation = collision_objects[0].ADD;

  collision_objects[1].id = "safety_player2";
  collision_objects[1].header.frame_id = "base_link";

  collision_objects[1].primitives.resize(1);
  collision_objects[1].primitives[0].type = collision_objects[0].primitives[0].BOX;
  collision_objects[1].primitives[0].dimensions.resize(3);
  collision_objects[1].primitives[0].dimensions[0] = 0.7;
  collision_objects[1].primitives[0].dimensions[1] = 0.5;
  collision_objects[1].primitives[0].dimensions[2] = 1.0;

  collision_objects[1].primitive_poses.resize(1);
  collision_objects[1].primitive_poses[0].position.x = -0.58;
  collision_objects[1].primitive_poses[0].position.y = 0.85;
  collision_objects[1].primitive_poses[0].position.z = 0;

  collision_objects[1].operation = collision_objects[1].ADD;

  planning_scene_interface.applyCollisionObjects(collision_objects);
}

void sequencePick(moveit::planning_interface::MoveGroupInterface& group, Manipulation manip){
  std::string recievedTarget = target;
  ROS_INFO("PICK SEQUENCE:");
  recievedTarget = target+"Up";
  ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
  group.setNamedTarget(recievedTarget);
  group.move();
  //IMPLEMENT OPEN GRIPPER
  ROS_INFO("OPENING GRIPPER");
  manip.gripper_open();
  manip.gripper_command.publish(manip.command);
  ros::WallDuration(0.1).sleep();
  recievedTarget = target+"Down";
  ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
  group.setNamedTarget(recievedTarget);
  group.move();
  ros::WallDuration(0.1).sleep();
  //IMPLEMENT CLOSE GRIPPER
  ROS_INFO("CLOSING GRIPPER");
  manip.gripper_set(CLOSED);
  manip.gripper_command.publish(manip.command);
  ros::WallDuration(0.3).sleep();
  if(load_collisions){
    attachedObject = "cube" + std::to_string(position+1);
    ROS_INFO("Attaching object \"%s\"",attachedObject.c_str());
    group.attachObject(attachedObject,"ee_link");
  }
  recievedTarget = target+"Up";
  ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
  group.setNamedTarget(recievedTarget);
  group.move();
  ros::WallDuration(0.1).sleep();
  ROS_INFO("TARGET SET: NEUTRAL");
  group.setNamedTarget("NEUTRAL");
  group.move();
}

void sequenceShow(moveit::planning_interface::MoveGroupInterface& group, Manipulation manip){
  ROS_INFO("DISPLAY SEQEUNCE:");
  group.setNamedTarget("NEUTRAL");
  group.move();
  ros::WallDuration(0.1).sleep();
  ROS_INFO("TARGET SET: PLAYERS");
  group.setNamedTarget("PLAYERS");
  group.move();
  ros::WallDuration(0.3).sleep();
  ROS_INFO("TARGET SET: SELF");
  group.setNamedTarget("SELF");
  group.move();
}

void sequenceDropoff(moveit::planning_interface::MoveGroupInterface& group, Manipulation manip){
  ROS_INFO("DROP-OFF SEQEUNCE:");
  ROS_INFO("TARGET SET: DROP-OFF");
  group.setNamedTarget("DESTROY");
  group.move();
  ros::WallDuration(0.2).sleep();
  ROS_INFO("OPENING GRIPPER");
  manip.gripper_open();
  manip.gripper_command.publish(manip.command);
  ros::WallDuration(0.2).sleep();
  ROS_INFO("CLOSING GRIPPER");
  manip.gripper_set(CLOSED);
  manip.gripper_command.publish(manip.command);
  ros::WallDuration(0.2).sleep();
  ROS_INFO("TARGET SET: NEUTRAL");
  group.setNamedTarget("NEUTRAL");
  group.move();
}

void gotoPosition(moveit::planning_interface::MoveGroupInterface& group, Manipulation manip){
  std::string recievedTarget = target;
  if (cmdNum == 1){ //POINT
    recievedTarget = target+"Up";
    ROS_INFO("TARGET SET: %s" ,recievedTarget.c_str());
    group.setNamedTarget(recievedTarget);
    group.move();
  }else if (cmdNum == 2){//PICK SEQUENCE
    sequencePick(group,manip);
  }else if (cmdNum == 3){//SHOW SEQUENCE
    sequenceShow(group,manip);
  }else if (cmdNum == 4){//DROPOFF SEQUENCE
    sequenceDropoff(group,manip);
  }
  ROS_INFO("Done.");
}

void gotoUtility(moveit::planning_interface::MoveGroupInterface& group){
  ROS_INFO("TARGET SET: %s",target.c_str());
  group.setNamedTarget(target);
  group.move();
  ROS_INFO("Done.");
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
    message.data = "";

    if(rows.find(row) != std::string::npos){ //POSITION COMMAND ----------------
      target = "";
      position += (int)col-'0';
      switch(row){
        case 'A': target.append("A"); break;
        case 'B': target.append("B"); position += rowSize; break;
        case 'C': target.append("C"); position += rowSize*2; break;
        case 'D': target.append("D"); position += rowSize*3; break;
        default: ROS_WARN("Row not recognized");
      }
      //Position now equals (row#-1)*rowsize+col#-1 (to make it proper array index, 1=0, 2=1, etc.)
      //Note that position is no longer that useful since implementing the use of setNamedTarget(),
      //    but may still come in handy.
      int pos = (int)col-'0';
      target.append(std::to_string(pos)+"_");
      //In this case, row==X is intended to mean anything other than a board position command
    }else if(row == 'X'){ //SPECIAL COMMAND ------------------------------------
        switch(col){
          case 'H': target = "HOME"; position = -1; break; //HOME
          case 'N': target = "NEUTRAL"; position = -1; break; //NEUTRAL
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
                    break;
          case '1': target = "PLAYER1"; position = -1; break; //Show PLAYER1
          case '2': target = "PLAYER2"; position = -1; break; //Show PLAYER2
          case 'B': target = "PLAYERS"; position = -1; break; //Show both players (corner area)
          case 'S': target = "SELF"; position = -1; break; //Show Self
          case 'O': position = -2; break; //Open Gripper
          case 'C': position = -3; break; //Close Gripper
          case 'R': ROS_INFO("RESET CLICK COUNTER"); return; //RESET (Handled by gui)
        }
    }else if(row == 'M'){//MESSAGE COMMAND -------------------------------------
      position = -4;
      switch(col){
        case '1': message.data = "How about this one?"; break;
        case '2': message.data = "I agree."; break;
        case '3': message.data = "I disagree."; break;
        case '4': message.data = ""; break;
        case '5': message.data = ""; break;
        case '6': message.data = ""; break;
        case '7': message.data = ""; break;
        default:  message.data = "Unknown message."; break;
      }
    }
    //Output recieved message and parsed information for debugging
    ROS_INFO("GUI MSG: [%s]", data.c_str());
    //ROS_INFO("PARSED: Row=%c  Col=%c  Num=%d  Pos=%d",row,col,cmdNum,position);
    //ROS_INFO("TARGET: %s",target.c_str());

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
  //Setup ROS
  ROS_INFO("Starting...");
  ros::init(argc, argv, "wpi_controller");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  ros::Subscriber sub = nh.subscribe("arm_command", 1000, commandCallback);
  message.data = "";
  ros::Publisher msg_sender = nh.advertise<std_msgs::String>("text_display", 1000);

  //Setup gripper
  Manipulation manipulation(nh);
  manipulation.activate_gripper();
  manipulation.gripper_command.publish(manipulation.command);
  ros::Duration(1.0).sleep();
  manipulation.gripper_open();
  manipulation.gripper_command.publish(manipulation.command);
  ros::Duration(0.5).sleep();

  //Setup ROS parameters
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
  moveit::planning_interface::MoveGroupInterface group("manipulator");
  float v_scale, plan_time;
  nh.getParam(ros::this_node::getName()+"/v_scale",v_scale);
  group.setMaxVelocityScalingFactor(v_scale);
  nh.getParam(ros::this_node::getName()+"/plan_time",plan_time);
  group.setPlanningTime(plan_time);
  ROS_INFO("Velocity Scaling: %.2f    Plan Time: %.2f",v_scale,plan_time);
  nh.getParam(ros::this_node::getName()+"/LCO",load_collisions);
  nh.getParam(ros::this_node::getName()+"/load_table",load_table);
  nh.getParam(ros::this_node::getName()+"/load_safety",load_safety);
  ROS_INFO("Load Collision Objects: %s",load_collisions?"YES":"NO");
  ROS_INFO("Load Table: %s",load_table?"YES":"NO");
  ROS_INFO("Load Safety: %s",load_safety?"YES":"NAH");

  //Setup collision objects
  if(load_collisions)
    addCollisionObjects(planning_scene_interface);
  else if(load_table)
    addTableCollision(planning_scene_interface);
  if(load_safety){
    addSafetyCollision(planning_scene_interface);
  }

  //Main callback loop
  ROS_INFO("DONE. Listening...");
  while(ros::ok()){
    if(newMsg){ //New message recieved from callback, figure out what to do.
      if(position>=0){ //Must be a board position
        message.data = "";
        msg_sender.publish(message);
        gotoPosition(group,manipulation);
      }else if (position == -1){ //Must be a utility position (home,dropoff,etc)
        message.data = "";
        msg_sender.publish(message);
        gotoUtility(group);
      }else if (position == -2){ //Open Gripper
        ROS_INFO("OPENING GRIPPER");
        manipulation.gripper_open();
        manipulation.gripper_command.publish(manipulation.command);
        if(attachedObject != ""){ //If there's a collision object attached, remove it (drop it)
          std::vector<std::string> toRemove;
          toRemove.resize(1);
          toRemove[0] = attachedObject;
          group.detachObject(attachedObject);
          ROS_INFO("Removing object \"%s\"", toRemove[0].c_str());
          planning_scene_interface.removeCollisionObjects(toRemove);
          attachedObject = "";
        }
      }else if (position == -3){ //Close Gripper
        ROS_INFO("CLOSING GRIPPER");
        manipulation.gripper_set(CLOSED);
        manipulation.gripper_command.publish(manipulation.command);
      }else if (position == -4){ //Display a message
        ROS_INFO("Show Message: %s",message.data.c_str());
        msg_sender.publish(message);
      }else
        ROS_INFO("Message unknown. No action taken.");

      newMsg = false;
    }
  }
  std::cout << "\nExiting...\n";
  ros::WallDuration(0.5).sleep();
  return 0;
}
/*
POSITION CODES:
-4 : Display message
-3 : Close Gripper
-2 : Open Gripper
-1 : Utility positions (Go directly to target)
0-15 : Block positions A1-D4
*/
