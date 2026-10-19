#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cpphighlighter.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QDir>
#include <QScrollBar>
#include <QFileSystemModel>
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->FileEditor->clear();
    ui->FileEditor->setTabsClosable(true);
    connect(ui->FileEditor, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    ui->FileEditor->setMovable(true);
    ui->sidebarLeft->setMaximumWidth(screenWidth/4.5); //Max width = 22% of the monitor width size

    auto *saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::saveCurrentTab);

    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(project.rootPath);
    model->setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    ui->sidebarLeft->setModel(model);
    ui->sidebarLeft->setRootIndex(model->index(project.rootPath));
    ui->sidebarLeft->setExpandsOnDoubleClick(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::maybeSave()
{
    for (int i = 0; i < ui->FileEditor->count(); ++i) {
        QWidget *tab = ui->FileEditor->widget(i);
        QTextEdit *editor = tab->findChild<QTextEdit*>();
        if (editor && editor->document()->isModified()) {
            const QString fileName = QFileInfo(tab->property("filePath").toString()).fileName();

            auto result = QMessageBox::question(this, "Unsaved changes",
                                                "Save changes to " + fileName + "?",
                                                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (result == QMessageBox::Cancel)
                return false;

            if (result == QMessageBox::Save) {
                ui->FileEditor->setCurrentIndex(i);
                if (!saveCurrentTab())
                    return false;
            }
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!maybeSave()) {
        event->ignore();
        return;
    }

    updateProjectState();

    QJsonObject json = project.toJson();
    QFile file(project.rootPath + "/.project.json");
    if (file.open(QIODevice::WriteOnly))
        file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));

    event->accept();
}

void MainWindow::loadProject(const QString &rootPath)
{
    QFile file(rootPath + "/.project.json");
    if (!file.open(QIODevice::ReadOnly))
        return;  // no saved state, start fresh

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    project = Project::fromJson(doc.object());

    for (int i = 0; i < project.openFiles.size(); ++i) {
        const auto &of = project.openFiles[i];
        loadFile(QDir(project.rootPath).filePath(of.relativePath));
        // Optionally restore cursor/scroll:
        // if (i == project.activeTab) restoreCursorPosition(of);
    }

    ui->FileEditor->setCurrentIndex(project.activeTab);
}

void MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File couldn't be read:" << path;
        return;
    }
    const QString contents = QString::fromUtf8(file.readAll());

    // Build the tab: container widget -> layout -> QTextEdit.
    auto *tab      = new QWidget(ui->FileEditor);
    auto *layout   = new QVBoxLayout(tab);
    auto *editor   = new QTextEdit(tab);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(editor);
    tab->setLayout(layout);

    tab->setProperty("filePath", path);
    editor->setPlainText(contents);
    new CppHighlighter(editor->document());
    const QString label = QFileInfo(path).fileName();
    const int index = ui->FileEditor->addTab(tab, label);
    ui->FileEditor->setCurrentIndex(index);

    // Update tab title when modified state changes
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, [this, tab, label](bool modified) {
                int idx = ui->FileEditor->indexOf(tab);
                if (idx != -1)
                    ui->FileEditor->setTabText(idx, (modified ? "* " : "") + label);
            });
}

void MainWindow::updateProjectState()
{
    project.openFiles.clear();
    for (int i = 0; i < ui->FileEditor->count(); ++i) {
        QWidget *tab = ui->FileEditor->widget(i);
        OpenFile of;
        of.relativePath = QDir(project.rootPath)
                              .relativeFilePath(tab->property("filePath").toString());

        QTextEdit *editor = tab->findChild<QTextEdit*>();
        if (editor) {
            QTextCursor cursor = editor->textCursor();
            of.cursorLine = cursor.blockNumber();
            of.scrollPos  = editor->verticalScrollBar()->value();
        }
        project.openFiles.append(of);
    }
    project.activeTab = ui->FileEditor->currentIndex();
}

bool MainWindow::saveCurrentTab()
{
    QWidget *tab = ui->FileEditor->currentWidget();
    if (!tab) return false;

    QTextEdit *editor = tab->findChild<QTextEdit*>();
    if (!editor) return false;

    const QString path = tab->property("filePath").toString();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not save file.");
        return false;
    }
    file.write(editor->toPlainText().toUtf8());
    editor->document()->setModified(false);
    return true;
}

void MainWindow::closeTab(int index)
{
    QWidget *tab = ui->FileEditor->widget(index);
    QTextEdit *editor = tab->findChild<QTextEdit*>();

    if (editor && editor->document()->isModified()) {
        auto result = QMessageBox::question(this, "Unsaved changes",
                                            "Save changes to " + QFileInfo(tab->property("filePath").toString()).fileName() + "?",
                                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Cancel)
            return;

        if (result == QMessageBox::Save) {
            ui->FileEditor->setCurrentIndex(index);
            saveCurrentTab();
        }
    }

    ui->FileEditor->removeTab(index);
    tab->deleteLater();
}