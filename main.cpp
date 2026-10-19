#include "mainwindow.h"
#include "startupdialog.h"

#include <windows.h>
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    AddDllDirectory(L"C:\\Qt\\6.5\\mingw_64\\bin", nullptr);

    QApplication application(argc, argv);
    QApplication::setWindowIcon(QIcon(":/mountain.ico"));

    StartupDialog startup;
    if (startup.exec() != QDialog::Accepted)
        return 0;  // user cancelled

    MainWindow window;
    window.showMaximized();
    return QApplication::exec();
}
