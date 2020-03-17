//Includes
#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/wpi_gui/main_window.hpp"

//Namespaces
namespace wpi_gui {

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
	lastClicked = QString("");
	clickCounter = 0;

	QMainWindow::move(0,0);

	QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));
	if ( !qnode.init() ) {
		showNoMasterMessage();
	}
}

MainWindow::~MainWindow() {}

//Implement SLOTS:
void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
  close();
}

void MainWindow::positionClicked() {
	//Save the text of the button that was pressed to a QString.
	QPushButton *clickedbutton = qobject_cast<QPushButton *>(sender());
  QString btnText = clickedbutton->text();
	//Construct a message to be sent to QNode
	if (btnText == lastClicked){
		clickCounter++;
	}else{
		clickCounter = 1;
		lastClicked = btnText;
	}
	std::stringstream code;
	code << btnText.toUtf8().constData();
	code << "-" << clickCounter;
	//Send message to QNode which then publishes on the relevant rostopic
	qnode.setMessage(code.str());
	//Set UI textbox to message sent for debugging
	ui.lineStatus->setText(QString(code.str().c_str()));
}

void MainWindow::utilityClicked() {
	//Save the text of the button that was pressed to a QString.
	std::stringstream code;
	QPushButton *clickedbutton = qobject_cast<QPushButton *>(sender());
  QString btnText = clickedbutton->text();
	//Construct a message to be sent to QNode
	if (btnText == lastClicked){
		clickCounter++;
	}else{
		clickCounter = 1;
		lastClicked = btnText;
	}
	if(btnText == "HOME"){
		code << "XH-"<<clickCounter;
	}else if (btnText == "NEUTRAL"){
		code << "XN-"<<clickCounter;
	}else if (btnText == "DESTROY"){
		code << "XD-"<<clickCounter;
	}else if (btnText == "DOWN"){
		code << "XW-"<<clickCounter;
	}else if (btnText == "P1"){
		code << "X1-"<<clickCounter;
	}else if (btnText == "P2"){
		code << "X2-"<<clickCounter;
	}else if (btnText == "BOTH"){
		code << "XB-"<<clickCounter;
	}else if (btnText == "SELF"){
		code << "XS-"<<clickCounter;
	}else if (btnText == "OPEN"){
		code << "XO-"<<clickCounter;
	}else if (btnText == "CLOSE"){
		code << "XC-"<<clickCounter;
	}else if (btnText == "RESET"){
		code << "XR-"<<clickCounter;
		lastClicked = QString("");
		clickCounter = 0;
	}else{
		code << "ERROR";
	}
	//Send message to QNode which then publishes on the relevant rostopic
	qnode.setMessage(code.str());
	//Set UI textbox to message sent for debugging
	ui.lineStatus->setText(code.str().c_str());
}

void MainWindow::messageClicked(){
	//Save the text of the button that was pressed to a QString.
	std::stringstream code;
	QPushButton *clickedbutton = qobject_cast<QPushButton *>(sender());
  QString btnText = clickedbutton->text();
	//Construct a message to be sent to QNode
	if (btnText.startsWith("MSG1")){
		code << "M1-"<<clickCounter;
	}else if (btnText.startsWith("MSG2")){
		code << "M2-"<<clickCounter;
	}else if (btnText.startsWith("MSG3")){
		code << "M3-"<<clickCounter;
	}else if (btnText.startsWith("MSG4")){
		code << "M4-"<<clickCounter;
	}else if (btnText.startsWith("MSG5")){
		code << "M5-"<<clickCounter;
	}else if (btnText.startsWith("MSG6")){
		code << "M6-"<<clickCounter;
	}else if (btnText.startsWith("MSG7")){
		code << "M7-"<<clickCounter;
	}else{
		code << "ERROR";
	}

	//Send message to QNode which then publishes on the relevant rostopic
	qnode.setMessage(code.str());
	//Set UI textbox to message sent for debugging
	ui.lineStatus->setText(code.str().c_str());
}

//MainWindow settings not implemented. --
void MainWindow::ReadSettings() {

}

void MainWindow::WriteSettings() {

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	WriteSettings();
	QMainWindow::closeEvent(event);
}

}  // namespace wpi_gui
