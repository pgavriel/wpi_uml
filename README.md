# WPI-UML ROS GUI and Controller

## Overview
This repository contains three ROS packages, wpi_gui, wpi_controller, and participant_display.
* wpi_gui is written using QT with the help of QT's IDE, and publishes coded messages to a ROS topic.
* wpi_controller implements MoveIt, listens on the published topic, decodes messages sent by the GUI, and decides what to do.
* participant_display is a window that will show messages that are published on the text_display topic in large readable text. 

The purpose of these packages is to use a GUI to puppet a robotic arm between a number
of hard-coded positions to play a pattern matching game with a participant. During the
game, the robot must be able to point to, pick, and place various cubes that constitute the game board.

**Quick Resources**  
[QT Open Source Download](https://www.qt.io/download-qt-installer)  
[QT Overview (Especially Core Internals)](https://doc.qt.io/qt-5/overviews-main.html)  
[Qt Reference](https://doc.qt.io/qt-5/reference-overview.html)  
[Qt Examples And Tutorials](https://doc.qt.io/qt-5/qtexamplesandtutorials.html)  


## Functionality / Implementation
It pick up block good.

## Methods  

#### Creating the GUI
This process was not very straight-forward, however it works reasonably well once all the pieces come together.  
**Essentially:**  
1. I used qt_create to make a default QT ROS package that included a .ui XML file, and a node for publishing to ROS topics.
2. I used the QT Open Source IDE to visually construct my desired GUI, and built the program to generate a corresponding .ui file.
3. I then transplanted the generated .ui file into the ROS package, completely replacing the original one.  
**NOTE**: After doing this, you'll need to go through the source and header files of your ROS package and prune out the slots and functions that implemented the template UI.

**STEP BY STEP:**  
First I started by installing all of the ROS QT packages, which likely includes some that are unnecessary, but I know enough of them are needed to justify doing it this way for the sake of simplicity.

> sudo apt-get install ros-kinetic-qt-*

I then used the installed qt_create package to create a template QT ROS package.

> cd catkin_ws/src  
> catkin_create_qt_pkg [PACKAGE NAME]  
> cd ..; catkin build  

Next, install the [QT Open Source IDE](https://www.qt.io/download-qt-installer) to construct your desired GUI. Unfortunately you need to create an account to finish the installation process, but you should be able to use a throw away email.

Create a new QT Widgets Application, and technically name it whatever you want but I think naming it "MainWindowDesign" will be slightly less of a headache when it comes to transplanting your ui file.

Now use the QT Creator to make whatever GUI you want, build the program, and you should be able to see your generated ui file.

Copy the entire ui file, and completely replace the one generated for your ROS package, and attempt to build. There's almost no chance it will succeed, but it may inform you what needs to be pruned from your source and header files (the slots and functions that were being used by the template).

Once you get the package to build successfully, you have your desired UI, and a ROS framework, but you still need to connect your UI elements to SLOTS in order for them to function. You can do this either within your UI file (see the bottom of my UI file) or, within your code (see QT documentation on slots).

From here, it all depends on the purpose of your gui/program. Broadly, the qnode.cpp file can be co-opted to publish some kind of messages to a desired topic, and your main_window.cpp file should implement your slots to tell your qnode what to publish.

In the end, you should have your desired GUI which is able to publish messages to a topic that any other ROS node will be able to subscribe to. Now you can create a ROS package to listen and respond to your topic accordingly.

#### Writing the controller  
**Messages:**  
Since this is a relatively simple arm controller that uses pre-defined joint states, the messages sent by my GUI were also pretty simple.
The format was as follows:
[char primary_code][char secondary_code]-[int message_counter]
Which ended up looking like:
"A2-2", "XH-1", "C4-12", etc.

With the controller node subscribed to the topic my GUI was publishing on, after hearing a new message it would execute it's callback function which decided what was supposed to happen.

Primary codes 'A-D' were used to indicate a position command in which the character represented the row in the grid of the game board, while the 'X' character was reserved for indicating various utility positions like HOME or open gripper.

Secondary code interpretation depended on the primary code, specifying which position was being asked for.

Message counter represented how many times one button had been pressed sequentially, which was used to augment the meaning of position commands which allowed for the use of less buttons overall.

**Joint States:**  
Desired joint states for positions were defined in a .xacro file (in the config folder) as a <xacro:macro>, which allows them to be easily added to the semantic description of a robot.

Originally I was essentially recording and defining joint states manually, but anticipating potentially having to redo all of them (multiple times), I wrote some scripts to help me.  

**Joint Logger:**  
In *wpi_controller/util/jointlogger* there's a simple script called log which can help to quickly record joint states.

Usage:  
> cd wpi_controller/util/jointlogger  
> ./log [STATE NAME]  

This will append the state name followed by the current joint positions of the robot to the *joints.txt* file contained in the same folder.

Once you have all your joint states recorded, you can make use of the **groupstate_generator** node to automatically generate the proper XML to define your group_states.

> rosrun wpi_controller groupstate_generator

This should create an *output.txt* file in the *util* folder which will contain your XML group_states.
