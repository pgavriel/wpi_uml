#include <ros/ros.h>
#include <ros/network.h>
#include <QtGui>
#include <QMessageBox>
#include <iostream>
// #include <cstdio>
// #include <ctime>
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

	if ( !qnode.init() ) {
		showNoMasterMessage();
	}
	last_msg = ui.label->text().toStdString();
	drawing = false;
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

//Function used for creating delays in ms.
void delay(int ms)
{
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::new_message(QString msg) {
	if (drawing){
		ROS_INFO("MESSAGE RECIEVED WHILE DRAWING, IGNORING.\n");
		return;
	}
	drawing = true;

	bool fancydraw = true;
	char cursor = '_';
	int speed = 20;
	int time = 150; //Milliseconds to draw
	std::string fancy = "";
	std::string message = msg.toStdString();

	ROS_INFO("MAIN WINDOW RECIEVED: %s",message.c_str());

	if(fancydraw){
		//If recieved message is the same as the last one, ignore it (don't redraw).
		ROS_INFO("LAST MESSAGE: %s",last_msg.c_str());
		if(last_msg == message){
			ROS_INFO("DUPLICATE MESSAGE SENT, IGNORING.");
			drawing = false;
			return;
		}

		//If label is not currently empty, "destruct" the message.
		if(last_msg != ""){
			ROS_INFO("DESTRUCTING MESSAGE");
			std::string tmp_msg;
			tmp_msg = last_msg.substr(0,last_msg.length()-1);
			for(int i = tmp_msg.length()-2 ; i > 0 ; i--){
				ui.label->setText((tmp_msg+cursor).c_str());
				tmp_msg = last_msg.substr(0,i);
				delay(speed);
			}
		}

		//If the new message is empty, set last_msg and return.
		if(message == ""){
			last_msg = "";
			ui.label->setText("_");
			delay(200);
			ui.label->setText("");
			drawing = false;
			return;
		}

		//Then "construct" the new message.
		ROS_INFO("CONSTRUCTING MESSAGE");
		ui.label->setText("_");
		delay(100);
		for(int i = 0 ; i < message.length()-1 ; i++){
			fancy+=message[i];
			ui.label->setText((fancy+cursor).c_str());
			delay(speed);
		}
		ui.label->setText(message.c_str());
		last_msg = message;
		QCoreApplication::processEvents();
		ROS_INFO("LAST_MSG SET TO: %s",last_msg.c_str());
	}else{
		//Very boring
		ui.label->setText(message.c_str());
	}
	drawing = false;
	return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);
}

}  // namespace participant_display
