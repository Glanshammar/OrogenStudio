#include "startupdialog.h"
#include "ui_startupdialog.h"

#include <QFileDialog>
#include <QDebug>

StartupDialog::StartupDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartupDialog)
    , projectsFile("projects.json")
{
    ui->setupUi(this);

    QFileInfo fileInfo(projectsFile);
    if (fileInfo.exists() && fileInfo.isFile()) {
        if (!projectsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open projects file:" << projectsFile.errorString();
        } else {
            const QByteArray data = projectsFile.readAll();
            projectsFile.close();

            QJsonParseError err;
            const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
            if (err.error != QJsonParseError::NoError) {
                qDebug() << "Parse error:" << err.errorString();
            } else if (doc.isArray()) {
                // Expected format: [ {project}, {project}, ... ]
                for (const QJsonValue &v : doc.array()) {
                    const QJsonObject o = v.toObject();
                    if (o.contains("projectName"))
                        projects.append(o.value("projectName").toString());
                }
            }
        }
    }

    listModel->setStringList(projects);
    ui->projectList->setModel(listModel);
}

StartupDialog::~StartupDialog()
{
    delete ui;
}

void StartupDialog::on_buttonOpenProject_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Project Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


    if (path.isEmpty())
        return;

    m_project.rootPath     = path;
    m_project.projectName  = QFileInfo(path).baseName();
    m_project.created      = QDateTime::currentDateTime();
    m_project.lastModified = m_project.created;
    m_project.indentSize   = 4;
    m_project.tabSize      = 4;
    m_project.activeTab    = -1;

    QDir directory(path);
    directory.setNameFilters({"*.cpp", "*.py", "*.rs", "*.c", "*.cmake", "CMakeLists.txt"});
    for (const QFileInfo &file : directory.entryInfoList()) {
        QString lang = file.suffix().toLower();
        if (file.fileName() == "CMakeLists.txt")
            lang = "cmake";
        if (!m_project.languages.contains(lang))
            m_project.languages.append(lang);
    }

    const QJsonObject json = m_project.toJson();

    // --- 1. Write the per-project project.json inside the project dir --
    QFile projectFile(QDir(path).filePath("project.json"));
    if (projectFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        projectFile.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
        projectFile.close();
    } else {
        qDebug() << "Could not write" << projectFile.fileName() << projectFile.errorString();
    }

    // --- 2. Read the IDE-level list, append, write back as an array ----
    QJsonArray arr;
    QFile listFile("projects.json");
    if (listFile.exists() && listFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QByteArray data = listFile.readAll();
        listFile.close();

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray())
            arr = doc.array();
        else if (doc.isObject() && doc.object().contains("projectName"))
            arr.append(doc.object());     // migrate old single-object file
    }

    // Remove an existing entry with the same rootPath (avoid duplicates)
    for (int i = 0; i < arr.size(); ++i) {
        if (arr.at(i).toObject().value("rootPath").toString() == path) {
            arr.removeAt(i);
            --i;
        }
    }
    arr.append(json);

    if (listFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        listFile.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        listFile.close();
    } else {
        qDebug() << "Could not write projects list:" << listFile.errorString();
    }

    done(QDialog::Accepted);
}

Project StartupDialog::selectedProject(){
    return m_project;
}