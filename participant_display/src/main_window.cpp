#include <ros/ros.h>
#include <ros/network.h>
#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include <cstdio>
#include <ctime>
#include "../include/participant_display/main_window.hpp"

//Namespaces
namespace participant_display {

using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
	: QMainWindow(parent)
	, qnode(argc,argv)
{
	ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
	setWindowIcon(QIcon(":/images/icon.png"));
  QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));

  QObject::connect(&qnode, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));

	QObject::connect(&qnode, SIGNAL(message_recieved(QString)), this, SLOT(new_message(QString)));


	// ros::init(argc,argv,"participant_display");
	// if ( ! ros::master::check() ) {
	// 	std::cout << "ERR" << std::endl;
	// }
	// ros::start(); // explicitly needed since our nodehandle is going out of scope.
	// ros::NodeHandle n;
	// //topic_listener = n.subscribe("text_display", 1000, &MainWindow::displayCallback, this);
	// topic_listener = n.subscribe("text_display", 1000,displayCallback);
	// message = "";
	// new_msg = false;
	// ros::spin();

	if ( !qnode.init() ) {
		showNoMasterMessage();
	}
}

MainWindow::~MainWindow() {
	if(ros::isStarted()) {
		ros::shutdown(); // explicitly needed since we use ros::start();
		ros::waitForShutdown();
	}
	wait();

}

/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
  close();
}

// void displayCallback(const std_msgs::String::ConstPtr& msg){
// 	ROS_INFO("CALLBACK CALLED");
// 	//new_msg = true;
// }
// void MainWindow::displayCallback(const std_msgs::String::ConstPtr& msg){
// 	ROS_INFO("CALLBACK CALLED");
// 	message = msg->data.c_str();
// 	ui.label->setText(message.c_str());
// 	//new_msg = true;
// }
void delay(int ms)
{
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::new_message(QString msg) {
	std::string message = msg.toStdString();
	std::string fancy = "";
	std::clock_t start;
	int clock_offset = 2;
	char cursor = '_';
	bool fancydraw = true;
	int speed = 20;
	ROS_INFO("MAIN WINDOW MESSAGE RECIEVED: %s",message.c_str());

	if(fancydraw){
		ui.label->setText("_");
		delay(100);
		for(int i = 0 ; i < message.length()-1 ; i++){
			fancy+=message[i];
			ui.label->setText((fancy+cursor).c_str());
			delay(speed);
		}
		fancy+=message[message.length()-1];
		ui.label->setText((fancy).c_str());
	}else{
		ui.label->setText(message.c_str());
	}
	return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);
}

}  // namespace participant_display
