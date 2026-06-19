#include "mainwindow.h"
#include "musicplayer.h"
#include "videoplayer.h"
#include "pictureviewer.h"
//#include "executerbg.h"

#include "ui_mainwindow.h"

#include <windows.h>
#include <ShlDisp.h>

#include <QGraphicsOpacityEffect>

#include <QTimer>
#include <QTime>

#include <QMenu>
#include <QAction>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Ole32.lib")

void RestoreUpAll() {
    // Initialize COM library
        CoInitialize(NULL);

        IShellDispatch* pShell = NULL;

        // Create an instance of the Shell object
        HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pShell);

        if (SUCCEEDED(hr) && pShell != NULL) {
            // Call the official UndoMinimizeAll method
            pShell->UndoMinimizeALL();
            pShell->Release();
        }

        // Uninitialize COM
        CoUninitialize();
}

void MainWindow::loadList()
{
    QSettings settings("JarredApps", "MayMayLauncher");

    int size = settings.beginReadArray("Files");

    QFileIconProvider iconProvider;

    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);

        QString path =
            settings.value("Path").toString();

        QFileInfo info(path);

        if (!info.exists())
            continue;

        QStandardItem *item =
            new QStandardItem(
                iconProvider.icon(info),
                info.fileName());

        item->setData(path, Qt::UserRole);

        m_model->appendRow(item);
    }

    settings.endArray();
}

void MainWindow::saveList()
{
    QSettings settings("JarredApps", "MayMayLauncher");

    settings.beginWriteArray("Files");

    for (int i = 0; i < m_model->rowCount(); i++)
    {
        settings.setArrayIndex(i);

        QString path =
            m_model->item(i)->data(Qt::UserRole).toString();

        settings.setValue("Path", path);
    }

    settings.endArray();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);
    this->setWindowState(Qt::WindowFullScreen);

    m_model = new QStandardItemModel(this);

    ui->listView->setModel(m_model);

    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setIconSize(QSize(48, 48));

    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->listView->setStyleSheet("QListView {"
                            "background-color: rgba(194, 120, 10, 64);"
                            "border-radius: 15px;"
                            "}"
                            "QListView::item {"
                            "color: white;"
                            "shadow: 0px 0px 5px rgba(0, 0, 0, 0.5);"
                            "}"
                            "QListView::item:selected {"
                            "background: rgba(255, 255, 255, 0.3);"
                            "}"
                            "QListView::item:hover {"
                            "background: rgba(255, 255, 255, 0.1);"
                            "}");

    loadList();

    QSettings settings("JarredApps", "MayMayLauncher");

    QString profilePath = settings.value(
                "Profile/Picture",
                ":/default/profile.jpg").toString();

    QString profileName = settings.value(
                "Profile/Nickname",
                "defaultUser").toString();

    QPixmap profilePic(profilePath);

    ui->profile->setPixmap(
                profilePic.scaled(
                    ui->profile->size(),
                    Qt::KeepAspectRatioByExpanding,
                    Qt::SmoothTransformation));
    ui->profile->setFixedSize(128,128);
    ui->profile->setAlignment(Qt::AlignCenter);
    ui->profile->setFrameShape(QFrame::Box);

    ui->userName->setText(profileName);

    // Set the label a running clock
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        ui->clock->setText(QTime::currentTime().toString("hh:mm:ss"));
    });
    timer->start(1000); // Update every second

    QMenu *contextMenu = new QMenu(this);
    QAction *aboutAction = contextMenu->addAction("About MayMayLauncher");
    QAction *settingsAction = contextMenu->addAction("Settings");
        connect(aboutAction, &QAction::triggered, this, []() {
            QString htmlAbout;

            htmlAbout.append("<h1>MayMayLauncher</h1>");
            htmlAbout.append("</br>");
            htmlAbout.append("<p>Version 0.1 (prerelease)</p>");
            htmlAbout.append("</br>");
            htmlAbout.append("<p>Created by: Jarred</p>");

            QMessageBox aboutBox;

            aboutBox.setWindowTitle("About MayMayLauncher (prerelease)");
            aboutBox.setInformativeText(htmlAbout);

            QPushButton* okButton = aboutBox.addButton(QMessageBox::Ok);
            QPushButton* aboutQtButton = aboutBox.addButton(tr("About Qt"), QMessageBox::ActionRole);

            aboutBox.exec();

            if (aboutBox.clickedButton() == okButton) {
                htmlAbout.clear(); // memory saving
            }
            if (aboutBox.clickedButton() == aboutQtButton) {
                htmlAbout.clear(); // memory saving
                QMessageBox::aboutQt(nullptr, "About Qt");
            }
        });
        connect(settingsAction, &QAction::triggered, this, []() {
            QMessageBox::information(nullptr,
                "Settings",
                "<h1>Settings</h1>"
                "<p>No settings available yet.</p>");
        });
    ui->toolButton->setMenu(contextMenu);
    ui->toolButton->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->centralWidget);
    ui->centralWidget->setGraphicsEffect(opacityEffect);

    fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeInAnimation->setDuration(1500);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);
}

MainWindow::~MainWindow()
{
    delete ui;
    RestoreUpAll();
}

//void MainWindow::on_MainWindow_destroyed()
//{

//}

void MainWindow::showEvent(QShowEvent *event) {
    fadeInAnimation->start();
    QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton comfirmQuit = QMessageBox::question(this,
                "Are you sure?",
                "<h1>Do you want to quit</h1>",
                QMessageBox::Yes|QMessageBox::No);

    if (comfirmQuit == QMessageBox::Yes) {
        saveList();
        QMainWindow::closeEvent(event);
    }
//    QMainWindow::closeEvent(event);
}

void MainWindow::on_pushButton_2_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                    this,
                    "Select Files",
                    QString(),
                    "All Files (*.*)");

    QFileIconProvider iconProvider;

    foreach (QString filePath, files)
    {
        QFileInfo info(filePath);

        QStandardItem *item =
                new QStandardItem(
                    iconProvider.icon(info),
                    info.fileName());

        item->setData(filePath, Qt::UserRole);

        m_model->appendRow(item);

        saveList();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::StandardButton comfirmDelete = QMessageBox::question(this,
                "Are you sure?",
                "<h1>You're going to delete this item on this list...</h1>"
                "</br>"
                "<h3>Proceed?</h3>",
                QMessageBox::Yes|QMessageBox::No);

    if (comfirmDelete == QMessageBox::Yes) {
        QModelIndex index = ui->listView->currentIndex();

        if (!index.isValid())
            return;

        m_model->removeRow(index.row());

        saveList();
    }
}

void MainWindow::on_pushButton_clicked()
{
    this->close();
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QString path =
            index.data(Qt::UserRole).toString();
    
    // Sure if the file exists, or else, it will remove from the list
    if (!QFileInfo::exists(path)) {
        QMessageBox::StandardButton comfirmDelete = QMessageBox::question(this,
                    "File Not Found",
                    "<h1>The file doesn't exist...</h1>"
                    "</br>"
                    "<h3>Remove from the list?</h3>",
                    QMessageBox::Yes|QMessageBox::No);
        if (comfirmDelete == QMessageBox::Yes) {
            m_model->removeRow(index.row());
            saveList();
        }
        return;
    }

    // Open the file with the default application associated with its type.
    // Media apps will be viewed on the built-in media player
    if (path.endsWith(".mp3")
            || path.endsWith(".wav")
            || path.endsWith(".ogg")) {
        musicplayer *player = new musicplayer(path, this);
//        player->setStyleSheet("#musicplayer {"
//                              "border-image: url(:/bg/metro.jpg) 0 0 0 0 stretch stretch;"
//                              "}");
        player->show();
    }
    else if (path.endsWith(".mp4")) {
        videoplayer *player = new videoplayer(path, this);
//        player->setStyleSheet("#videoplayer {"
//                              "border-image: url(:/bg/metro.jpg) 0 0 0 0 stretch stretch;"
//                              "}");
        player->show();
    }
    else if (path.endsWith(".png")
             || path.endsWith(".jpg")
             || path.endsWith(".jpeg")
             || path.endsWith(".gif")) {
        pictureviewer* picView = new pictureviewer(path, this);
        picView->show();
    }
    else {
//        executerbg* exe = new executerbg(path, this);
//        exe->show();
        QDesktopServices::openUrl(
                    QUrl::fromLocalFile(path));
    }
}
