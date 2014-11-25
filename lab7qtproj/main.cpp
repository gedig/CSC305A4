#include <QApplication>
#include "window.h"
#include "glwidget.h"

//Main is needed in all win32 applications
int main(int argc, char* argv[])
{
    //Instance of new application pass arguments received from commandline
    QApplication app(argc, argv);

    //Instead of creating our GUI components ourselves. We will visually design
    //Everything then we subclass
    Window appWindow;

    //Show application MainWindow
    appWindow.show();

    //Execute app
    app.exec();

    return 0;
}
