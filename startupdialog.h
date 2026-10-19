#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QStringListModel>
#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include "project.h"

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog();
    static Project selectedProject();

private slots:
    void on_buttonOpenProject_clicked();

private:
    Ui::StartupDialog *ui;
    inline static Project m_project;
    QStringListModel *listModel = new QStringListModel(this);
    QStringList projects;
    QFile projectsFile;
};

#endif // STARTUPDIALOG_H
