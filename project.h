#ifndef PROJECT_H
#define PROJECT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

static QStringList jsonToStringList(const QJsonObject &obj, const QString &key)
{
    QStringList result;
    for (const QJsonValue &v : obj[key].toArray())
        result << v.toString();
    return result;
}

struct OpenFile {
    QString relativePath;
    int cursorLine = 0;
    int scrollPos = 0;

    QJsonObject toJson() const {
        return {
            {"path", relativePath},
            {"cursorLine", cursorLine},
            {"scrollPos", scrollPos}
        };
    }

    static OpenFile fromJson(const QJsonObject &obj) {
        OpenFile f;
        f.relativePath = obj["path"].toString();
        f.cursorLine   = obj["cursorLine"].toInt();
        f.scrollPos    = obj["scrollPos"].toInt();
        return f;
    }
};

struct Project
{
    // --- Identity ---
    QString projectName;
    QString rootPath;          // absolute path to project root
    QString description;       // optional free-form notes

    // --- Languages & Editor Defaults ---
    QStringList languages;     // e.g. {"cpp", "cmake", "python"}
    int indentSize = 4;
    int tabSize = 4;
    QList<OpenFile> openFiles;
    int activeTab = 0;

    // --- Build / Run ---
    QString buildCommand;      // e.g. "cmake --build build"
    QString runCommand;        // e.g. "./build/myapp"
    QString outputDir;         // e.g. "build/"
    QStringList includePaths;  // additional header search dirs
    QHash<QString, QString> buildEnv; // env vars for build/run

    // --- Metadata ---
    QDateTime created;
    QDateTime lastModified;

    QJsonObject toJson() const {
        QJsonArray files;
        for (const auto &file : openFiles)
            files.append(file.toJson());

        return {
            {"projectName", projectName},
            {"rootPath", rootPath},
            {"description", description},
            {"languages", QJsonArray::fromStringList(languages)},
            {"indentSize", indentSize},
            {"tabSize", tabSize},
            {"buildCommand", buildCommand},
            {"runCommand", runCommand},
            {"outputDir", outputDir},
            {"includePaths", QJsonArray::fromStringList(includePaths)},
            {"openFiles", files},
            {"activeTab", activeTab}
        };
    }

    static Project fromJson(const QJsonObject &obj) {
        Project project;
        project.projectName  = obj["projectName"].toString();
        project.rootPath     = obj["rootPath"].toString();
        project.description  = obj["description"].toString();
        project.indentSize   = obj["indentSize"].toInt(4);
        project.tabSize      = obj["tabSize"].toInt(4);
        project.buildCommand = obj["buildCommand"].toString();
        project.runCommand   = obj["runCommand"].toString();
        project.outputDir    = obj["outputDir"].toString();
        project.languages    = jsonToStringList(obj, "languages");
        project.includePaths = jsonToStringList(obj, "includePaths");

        project.openFiles.clear();
        for (const auto &val : obj["openFiles"].toArray())
            project.openFiles.append(OpenFile::fromJson(val.toObject()));

        project.activeTab = obj["activeTab"].toInt(0);
        return project;
    }
};

#endif // PROJECT_H
