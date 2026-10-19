#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QFileInfo>
#include <QShortcut>
#include <QGuiApplication>
#include <QScreen>
#include "startupdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void loadProject(const QString &projectPath);
    void loadFile(const QString &path);
    void closeTab(int index);
    bool saveCurrentTab();
    void closeEvent(QCloseEvent *event) override;
    void updateProjectState();
    bool maybeSave();

private:
    Ui::MainWindow *ui;
    Project project = StartupDialog::selectedProject();
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    int screenWidth = screenSize.width();
    int screenHeight = screenSize.height();
};
#endif // MAINWINDOW_H
