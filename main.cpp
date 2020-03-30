#include "notepad.h"
#include "word.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Notepad w;
    w.show();

    a.setApplicationName("Pablo Text Editor");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("University of Belize, CMPS2210:gui Programming");
    a.setOrganizationDomain("ub.edu.bz");



    return a.exec();
}
