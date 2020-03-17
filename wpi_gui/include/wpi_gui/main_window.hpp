#ifndef wpi_gui_MAIN_WINDOW_H
#define wpi_gui_MAIN_WINDOW_H

//Includes
#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"

//Namespace
namespace wpi_gui {

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

	void ReadSettings(); // Load up qt program settings at startup
	void WriteSettings(); // Save qt program settings when closing

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();

public Q_SLOTS:
	/******************************************
	** Auto-connections (connectSlotsByName())
	*******************************************/
	void positionClicked();
	void utilityClicked();
	void messageClicked();

private:
	Ui::MainWindowDesign ui;
	QNode qnode;
	QString lastClicked;
	int clickCounter;
};

}  // namespace wpi_gui

#endif // wpi_gui_MAIN_WINDOW_H
