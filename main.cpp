#include "mainwindow.h"
#include "entering_parameters.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    EnterWindow w(480, 500);
    w.show();

    return a.exec();
}
