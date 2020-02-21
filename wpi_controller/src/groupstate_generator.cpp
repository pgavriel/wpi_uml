#include <ros/ros.h>
#include <ros/console.h>
#include <ros/package.h>
#include "std_msgs/String.h"

#include <iostream>
#include <fstream>
#include <exception>

using namespace std;

int main(int argc, char** argv)
{
  ROS_INFO("Starting...");
  ros::init(argc, argv, "groupstate_generator");
  ros::NodeHandle nh;

  string tab = "    ";
  string group = "manipulator";
  string name = "";
  string joints[] = {"shoulder_pan_joint","shoulder_lift_joint","elbow_joint","wrist_1_joint","wrist_2_joint","wrist_3_joint"};
  string line;
  bool namefound = false;
  string path = ros::package::getPath("wpi_controller");
  ifstream infile(path+"/util/jointlogger/joints.txt");
  ofstream outfile(path+"/util/output.txt");
  //outfile.open (path+"/util/output.txt");

  while(getline(infile,line)){
    line.erase(remove(line.begin(), line.end(), ' '), line.end());
    if(!namefound){
      //If it hasn't seen a name= line yet
      if(line.rfind("name=",0) == 0){
        name = line.substr(5);
        namefound = true;
        ROS_INFO("%s",name.c_str());
      }
    }else if(line[0] == '['){ //Line with values
      //Remove square brackets at beginning and end of line
      line = line.substr(1);
      line.pop_back();
      stringstream ss(line);
      string token;
      vector<string> values;
      string forshow = "";
      //Populate string vector with joint values
      while(getline(ss,token,',')){
        values.push_back(token);
        forshow.append(token+" , ");
      }
      //With name and joint values collected, now we'll write to our xml file
      stringstream toWrite("");
      toWrite << tab + "<group_state name=\"" + name + "\" group=\"" + group + "\">\n";
      for(int i = 0 ; i < values.size() ; i++){
        toWrite << tab+tab+"<joint name=\""+joints[i]+"\" value=\""+values[i]+"\" />\n";
      }
      toWrite << tab+"</group_state>\n";
      outfile << toWrite.str();

      ROS_INFO("Values: %s", forshow.c_str());
      namefound = false;
    }

    //ROS_INFO("%s",line.c_str());

  }

  outfile.close();

  cout << "\nExiting..." << endl;
  ros::WallDuration(0.5).sleep();
  return 0;
}
