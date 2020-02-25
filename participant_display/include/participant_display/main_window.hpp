/**
 * @file /include/participant_display/main_window.hpp
 *
 * @brief Qt based gui for participant_display.
 *
 * @date November 2010
 **/
#ifndef participant_display_MAIN_WINDOW_H
#define participant_display_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <std_msgs/String.h>

#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace participant_display {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();
	//void displayCallback(const std_msgs::String::ConstPtr& msg);


public Q_SLOTS:
  void new_message(QString msg);

private:

	Ui::MainWindowDesign ui;
	QNode qnode;
  ros::Subscriber topic_listener;
	bool new_msg;
	std::string message;
	std::string last_msg;
	bool drawing;
};

}  // namespace participant_display

#endif // participant_display_MAIN_WINDOW_H
