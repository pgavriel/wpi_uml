#ifndef wpi_gui_QNODE_HPP_
#define wpi_gui_QNODE_HPP_

//Includes
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <QThread>
#include <QStringListModel>

//Namespace
namespace wpi_gui {

//Class
class QNode : public QThread {
Q_OBJECT

public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();
	bool init(const std::string &master_url, const std::string &host_url);
	void run();

	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

	QStringListModel* loggingModel() { return &logging_model; }
	void log( const LogLevel &level, const std::string &msg);
  void setMessage(std::string msg);

Q_SIGNALS:
	void loggingUpdated();
  void rosShutdown();

private:
	int init_argc;
	char** init_argv;
	ros::Publisher chatter_publisher;
  QStringListModel logging_model;
  std::string message;
  bool msgchanged;
};

}  // namespace wpi_gui

#endif /* wpi_gui_QNODE_HPP_ */
