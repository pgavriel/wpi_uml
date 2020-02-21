//Includes
#include <QtGui>
#include <QApplication>
#include "../include/wpi_gui/main_window.hpp"

//Main
int main(int argc, char **argv) {

    /*********************
    ** Qt
    **********************/
    QApplication app(argc, argv);
    wpi_gui::MainWindow w(argc,argv);
    w.show();
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    int result = app.exec();

	return result;
}
