/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include "../include/participant_display/qnode.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace participant_display {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{
		message;
		new_msg;
	}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
	ros::init(init_argc,init_argv,"participant_display");
	if ( ! ros::master::check() ) {
		return false;
	}
	message = "";
	new_msg = false;
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	// Add your ros communications here.
	//topic_listener = n.subscribe("text_display", 1000, &QNode::displayCallback, this);
	//chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
	start();
	return true;
}

void QNode::displayCallback(const std_msgs::String::ConstPtr& msg){
	log(Info,std::string("CALLBACK REACHED "));
	message = msg->data.c_str();
	new_msg = true;
	Q_EMIT message_recieved(QString(message.c_str()));
	log(Info,std::string("Leaving callback new_msg: ")+std::string(new_msg?"TRUE":"FALSE"));
}

void QNode::run() {
	ros::NodeHandle n;
	topic_listener = n.subscribe("text_display", 1000, &QNode::displayCallback, this);
	ros::Rate loop_rate(10);
	while ( ros::ok() ) {
		log(Info,std::string("New_msg: ")+std::string(new_msg?"TRUE":"FALSE"));
		if(new_msg){

			log(Info, std::string("MESSAGE RECIEVED: ")+message.c_str());
			//Q_EMIT message_recieved(message);
			new_msg = false;
		}
		ros::spin();
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}

void QNode::log( const LogLevel &level, const std::string &msg) {
	logging_model.insertRows(logging_model.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
				logging_model_msg << "[DEBUG] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
				logging_model_msg << "[ERROR] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
				logging_model_msg << "[FATAL] [" << ros::Time::now() << "]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
	logging_model.setData(logging_model.index(logging_model.rowCount()-1),new_row);
	Q_EMIT loggingUpdated(); // used to readjust the scrollbar
}

}  // namespace participant_display
