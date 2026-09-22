#include "mainwindow.h"
#include "musicplayer.h"
#include "midiplayer.h"
#include "videoplayer.h"
#include "pictureviewer.h"
#include "settingsdialog.h"
//#include "executerbg.h"

#include "ui_mainwindow.h"

#include <windows.h>
#include <ShlDisp.h>

#include <QGraphicsOpacityEffect>

#include <QTimer>
#include <QTime>

#include <QMenu>
#include <QAction>

#include <QInputDialog>
#include <QFileDialog>

#include <QMetaObject>
#include <QApplication>
#include <QUrl>

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QAbstractItemModel>

// Wrap libmagic in extern "C" because MSVC 2013 requires C linkage for C libraries
extern "C" {
#include <magic.h>
}

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
    QStringList validPaths;

    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);

        QString path =
            settings.value("Path").toString();

        QFileInfo info(path);

        if (!info.exists())
            continue;

        validPaths.append(path);

        QStandardItem *item =
            new QStandardItem(
                iconProvider.icon(info),
                info.fileName());

        item->setData(path, Qt::UserRole);

        m_model->appendRow(item);
    }

    settings.endArray();

    if (validPaths.size() != size) {
        settings.beginWriteArray("Files");
        for (int i = 0; i < validPaths.size(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("Path", validPaths.at(i));
        }
        settings.endArray();
    }
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

bool MainWindow::fileMatchesCategory(const QString& filePath, const QString& category) {
    magic_t magic_cookie = magic_open(MAGIC_MIME_TYPE | MAGIC_SYMLINK);
        if (magic_cookie == NULL) {
            return false;
        }

        // 1. Get the directory where your .exe is currently running
        QString exeDir = QCoreApplication::applicationDirPath();

        // 2. Point to the magic.mgc file located in that same folder
        QString databasePath = exeDir + "/magic.mgc";

        // 3. Clean up the separators for Windows compatibility
        databasePath = QDir::toNativeSeparators(databasePath);

        // 4. Convert the string safely for libmagic
        QByteArray encodedDbPath = QFile::encodeName(databasePath);

        // 5. Load the database using the absolute local path
        if (magic_load(magic_cookie, encodedDbPath.constData()) != 0) {
            qWarning() << "Cannot load magic database:" << magic_error(magic_cookie);
            magic_close(magic_cookie);
            return false;
        }

        // 6. Perform the actual file testing
        QByteArray encodedFilePath = QFile::encodeName(filePath);
        const char* mime = magic_file(magic_cookie, encodedFilePath.constData());
        bool isMatch = false;

        if (mime != NULL) {
            QString mimeStr = QString::fromUtf8(mime);
            if (mimeStr.startsWith(category + "/")) {
                isMatch = true;
            }
        }

        magic_close(magic_cookie);
        return isMatch;
}

void MainWindow::changeProfilePicture()
{
    QString profilePic = QFileDialog::getOpenFileName(nullptr,
                                                     "Change Profile Pic",
                                                     nullptr,
                                                     "Profile pics (*.png; *.jpg; *.jpeg; *.gif;)");

    if (!profilePic.isEmpty()) {
        //profilePath = profilePic;

        QSettings settings("JarredApps", "MayMayLauncher");
        settings.setValue("Profile/Picture", profilePic);

        QPixmap profilePic(profilePic);

        ui->profile->setScaledContents(true);
        ui->profile->setPixmap(
                    profilePic.scaled(
                        ui->profile->size(),
                        Qt::KeepAspectRatioByExpanding,
                        Qt::SmoothTransformation));
        ui->profile->setFixedSize(128,128);
        ui->profile->setAlignment(Qt::AlignCenter);
        ui->profile->setFrameShape(QFrame::Box);
    }
}

void MainWindow::changeProfileName()
{
    bool ok;
    QString profileNameInput = QInputDialog::getText(nullptr,
                                               "Change your username/nickname",
                                               "Username/Nickname",
                                               QLineEdit::Normal,
                                               "",
                                               &ok);

    if (ok && !profileNameInput.isEmpty()) {
        //profileName = profileNameInput;

        QSettings settings("JarredApps", "MayMayLauncher");
        settings.setValue("Profile/Nickname",
                          profileNameInput);

        ui->userName->setText(profileNameInput);
    }
}

void MainWindow::setLauncherBg()
{
    QSettings settings("JarredApps", "MayMayLauncher");
    QString backgroundPath = settings.value(
                "Launcher/Background",
                ":/bg/metro.jpg").toString();

        if (!backgroundPath.startsWith(":/")
            && !QFileInfo::exists(backgroundPath)) {
        backgroundPath = ":/bg/metro.jpg";
        }

    QString backgroundUrl = backgroundPath.startsWith(":/")
            ? backgroundPath
            : QDir::fromNativeSeparators(backgroundPath);

    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (widget->objectName() == "MainWindow") {
            QWidget *centralWidget = widget->findChild<QWidget*>("centralWidget");
            if (centralWidget != nullptr) {
                centralWidget->setStyleSheet(QString(
                    "#centralWidget { border-image: url('%1') 0 0 0 0 stretch stretch; }")
                    .arg(backgroundUrl));
                centralWidget->update();
            }
        }
        else if (widget->objectName() == "musicplayer"
                 || widget->objectName() == "videoplayer"
                 || widget->objectName() == "pictureviewer"
                 || widget->objectName() == "midiplayer") {
            widget->setStyleSheet(QString(
                "#%1 { border-image: url('%2') 0 0 0 0 stretch stretch; }")
                .arg(widget->objectName(), backgroundUrl));
            widget->update();
        }
    }
}

void MainWindow::setLauncherBg_slot()
{
    MainWindow::setLauncherBg();
}

void MainWindow::changeBg()
{
    QString backgroundPath = QFileDialog::getOpenFileName(
                this,
                "Change BG",
                QString(),
                "Images (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (!backgroundPath.isEmpty()) {
        QSettings settings("JarredApps", "MayMayLauncher");
        settings.setValue("Launcher/Background", backgroundPath);
        MainWindow::setLauncherBg();
    }
}

void MainWindow::resetBg()
{
    QSettings settings("JarredApps", "MayMayLauncher");
    settings.remove("Launcher/Background");
    MainWindow::setLauncherBg();
}

MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindow::instance = this;
    MainWindow::setLauncherBg();

    QSettings settings("JarredApps", "MayMayLauncher");

    this->setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);
    this->setWindowState(Qt::WindowFullScreen);

    m_model = new QStandardItemModel(this);

    ui->listView->setModel(m_model);

    searchBox = new QLineEdit(this);
    searchBox->setObjectName("launcherSearchBox");
    searchBox->setPlaceholderText("Search games, apps, music, videos, and pictures...");
    searchBox->setClearButtonEnabled(true);
    searchBox->setToolTip("Type to filter your launcher list. Press Enter to open the selected item.");
    searchBox->setStyleSheet(
        "QLineEdit { background: rgba(255, 255, 255, 220); color: #202020; "
        "border: 1px solid rgba(255, 255, 255, 180); border-radius: 6px; "
        "padding: 8px 12px; font-size: 14px; }"
        "QLineEdit:focus { border: 2px solid #b8e356; padding: 7px 11px; }");
    ui->verticalLayout->insertWidget(3, searchBox);

    connect(searchBox, &QLineEdit::textChanged,
            this, &MainWindow::filterLauncherList);
    connect(searchBox, &QLineEdit::returnPressed, this, [this]() {
        const QModelIndex index = ui->listView->currentIndex();
        if (index.isValid()) {
            on_listView_doubleClicked(index);
        }
    });

    QShortcut *searchShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    searchShortcut->setContext(Qt::WindowShortcut);
    connect(searchShortcut, &QShortcut::activated, this, [this]() {
        searchBox->setFocus();
        searchBox->selectAll();
    });

    if (settings.value("viewMode", "icons").toString()
            == "icons") {
        ui->listView->setViewMode(QListView::IconMode);
        ui->listView->setIconSize(QSize(48, 48));
    }
    else if (settings.value("viewMode", "icons").toString()
             == "list") {
        ui->listView->setViewMode(QListView::ListMode);
        ui->listView->setIconSize(QSize(48, 48));
    }

    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

//    ui->listView->setStyleSheet("QListView {"
//                            "background-color: rgba(194, 120, 10, 64);"
//                            "border-radius: 15px;"
//                            "}"
//                            "QListView::item {"
//                            "color: white;"
//                            "shadow: 0px 0px 5px rgba(0, 0, 0, 0.5);"
//                            "}"
//                            "QListView::item:selected {"
//                            "background: rgba(255, 255, 255, 0.3);"
//                            "}"
//                            "QListView::item:hover {"
//                            "background: rgba(255, 255, 255, 0.1);"
//                            "}");

    loadList();

    QString profilePath = settings.value(
                "Profile/Picture",
                ":/default/profile.jpg").toString();

    QString profileName = settings.value(
                "Profile/Nickname",
                "defaultUser").toString();

    QPixmap profilePic(profilePath);

    ui->profile->setScaledContents(true);
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
    QMenu *changeBgMenu = contextMenu->addMenu("Change BG");
    QAction *fromFileAction = changeBgMenu->addAction("From File...");
    QAction *defaultBgAction = changeBgMenu->addAction("Default BG");
    QAction *testCrashAction = contextMenu->addAction("Test Crash (for debug only)");
        connect(aboutAction, &QAction::triggered, this, [this]() {
            QString htmlAbout;

            htmlAbout.append("<h1>MayMayLauncher</h1>");
            htmlAbout.append("</br>");
            htmlAbout.append("<p>Version 2.0</p>");
            htmlAbout.append("</br>");
            htmlAbout.append("<p>Created by: Jarred</p>");

            htmlAbout.append("<h3>Open Source Libraries:</h3>");

            // libmagic
            htmlAbout.append("<h4>File/libmagic</h4></br></p>");
            htmlAbout.append("Copyright (c) Ian F. Darwin 1986-1995.</br>Software written by Ian F. Darwin and others;</br>");
            htmlAbout.append("maintained by Christos Zoulas.</br>This product includes software developed by Christos Zoulas and others.</br>");
            htmlAbout.append("Licensed under the BSD 2-Clause License.</p><br>");

            // FluidSynth
            htmlAbout.append("<h4>FluidSynth</h4></br></p>");
            htmlAbout.append("Copyright (c) Peter Hanappe 2000-2026.</br>Software written by Peter Hanappe and others;</br>");
            htmlAbout.append("maintained by the FluidSynth Development Team.</br>This product includes software developed by FluidSynth and others.</br>");
            htmlAbout.append("Licensed under the GNU Lesser General Public License v2.1 or later.</p>");

            QMessageBox aboutBox;

            //aboutBox.setWindowTitle("About MayMayLauncher (prerelease)"); //prerelease
            aboutBox.setWindowTitle("About MayMayLauncher");
            aboutBox.setInformativeText(htmlAbout);

            QPushButton* okButton = aboutBox.addButton(QMessageBox::Ok);
            QPushButton* aboutQtButton = aboutBox.addButton(tr("About Qt"), QMessageBox::ActionRole);
            aboutQtButton->installEventFilter(this);

            aboutBox.exec();

            if (aboutBox.clickedButton() == okButton) {
                htmlAbout.clear(); // memory saving
            }
            if (aboutBox.clickedButton() == aboutQtButton) {
                if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
                    //settings.setValue("Profile/Picture", ":/default/secretprofile.jpg");


                }
                else {
                    htmlAbout.clear(); // memory saving

                }
            }
        });
        connect(settingsAction, &QAction::triggered, this, [this]() {
//            QMessageBox::information(nullptr,
//                "Settings",
//                "<h1>Settings</h1>"
//                "<p>No settings available yet.</p>");
            SettingsDialog settingsDlg(this);
            settingsDlg.exec();
        });
        connect(fromFileAction, &QAction::triggered,
                this, &MainWindow::changeBg);
        connect(defaultBgAction, &QAction::triggered,
            this, &MainWindow::resetBg);
        connect(testCrashAction, &QAction::triggered, this, []() {
            QMessageBox::StandardButton comfirm = QMessageBox::warning(nullptr,
                                                                       "Are you sure?",
                                                                       "<h1>This is designed for debuggers only!</h1>"
                                                                       "test crash?",
                                                                       QMessageBox::Yes | QMessageBox::No);

            if (comfirm == QMessageBox::Yes) {
                RaiseException(EXCEPTION_ACCESS_VIOLATION,
                               0,
                               0,
                               nullptr);
            }
        });
    ui->toolButton->setMenu(contextMenu);
//    ui->toolButton->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *contextMenuAccount = new QMenu(this);
    QAction *changeProfilePic = contextMenuAccount->addAction("Change Profile Pic");
    QAction *changeProfileName = contextMenuAccount->addAction("Change Profile Name");

    connect(changeProfilePic, &QAction::triggered, this, &MainWindow::changeProfilePicture);

    connect(changeProfileName, &QAction::triggered, this, &MainWindow::changeProfileName);

    ui->toolButton_2->setMenu(contextMenuAccount);
//    connect(ui->listView,
//            SIGNAL(enterPressed(QModelIndex)),
//            this,
//            SLOT(on_listView_doubleClicked(QModelIndex)));

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
        event->accept();
        QMainWindow::closeEvent(event);
    }
    else {
        event->ignore();
    }
    //    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter)
    {
        QModelIndex index = ui->listView->currentIndex();

        if (index.isValid())
            emit on_listView_doubleClicked(index);

        //return;
    }
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_A)
            emit on_pushButton_2_clicked();
    }
    if (event->key() == Qt::Key_Delete)
    {
//        QMessageBox::StandardButton comfirmDelete = QMessageBox::question(this,
//                    "Are you sure?",
//                    "<h1>You're going to delete this item on this list...</h1>"
//                    "</br>"
//                    "<h3>Proceed?</h3>",
//                    QMessageBox::Yes|QMessageBox::No);

//        if (comfirmDelete == QMessageBox::Yes) {
//            QModelIndex index = ui->listView->currentIndex();

//            if (!index.isValid())
//                return;

//            m_model->removeRow(index.row());

//            saveList();
//        }
        this->on_pushButton_3_clicked();
    }
    QMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (qobject_cast<QPushButton*>(obj) && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // Intercept Shift + Left Click
        if (mouseEvent->button() == Qt::LeftButton && (mouseEvent->modifiers() & Qt::ShiftModifier)) {
            QPixmap profilePic(":/default/secretprofile.jpg");

            ui->profile->setScaledContents(true);
            ui->profile->setPixmap(
                        profilePic.scaled(
                            ui->profile->size(),
                            Qt::KeepAspectRatioByExpanding,
                            Qt::SmoothTransformation));
            ui->profile->setFixedSize(128,128);
            ui->profile->setAlignment(Qt::AlignCenter);
            ui->profile->setFrameShape(QFrame::Box);

            ui->userName->setText("meimei");

            return true; // Stop the click from propagating normally
        }
        else if (mouseEvent->button() == Qt::LeftButton) {
            QMessageBox::aboutQt(this, "About Qt");
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_pushButton_2_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                    this,
                    "Select Files",
                    QString(),
                    "All Files (*.*)");

    QFileIconProvider iconProvider;

    for (const QString &filePath : qAsConst(files))
    {
        QFileInfo info(filePath);

        if (filePath.endsWith(".mml")) {
            this->importJsonToStandardModel(filePath);
        }
        else
        {
            QStandardItem *item =
                new QStandardItem(
                    iconProvider.icon(info),
                    info.fileName());

            item->setData(filePath, Qt::UserRole);

            m_model->appendRow(item);

            saveList();
        }
    }

    filterLauncherList(searchBox->text());
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::StandardButton comfirmDelete = QMessageBox::question(this,
                "Are you sure?",
                "<h1>You're going to delete this item on this list...</h1>"
                "</br>"
                "<p>Proceed?</p>",
                QMessageBox::Yes|QMessageBox::No);

    if (comfirmDelete == QMessageBox::Yes) {
        QModelIndex index = ui->listView->currentIndex();

        if (!index.isValid())
            return;

        m_model->removeRow(index.row());

        saveList();
    }
}

void MainWindow::filterLauncherList(const QString &text)
{
    const QString query = text.trimmed();
    int firstVisibleRow = -1;

    for (int row = 0; row < m_model->rowCount(); ++row) {
        const QStandardItem *item = m_model->item(row);
        const QString fileName = item->text();
        const QString path = item->data(Qt::UserRole).toString();
        const bool matches = query.isEmpty()
            || fileName.contains(query, Qt::CaseInsensitive)
            || path.contains(query, Qt::CaseInsensitive);

        ui->listView->setRowHidden(row, !matches);
        if (matches && firstVisibleRow < 0) {
            firstVisibleRow = row;
        }
    }

    if (firstVisibleRow >= 0) {
        ui->listView->setCurrentIndex(m_model->index(firstVisibleRow, 0));
    } else {
        ui->listView->clearSelection();
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

    QSettings settings("JarredApps", "MayMayLauncher");
    
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

        QString lowerPath = path.toLower();
        bool useMediaPlayer = settings.value("useMediaPlayer", true).toBool();

        if (useMediaPlayer &&
                (lowerPath.endsWith(".mid") || lowerPath.endsWith(".midi"))) {
            midiplayer *player = new midiplayer(path, this);
            player->show();
            MainWindow::setLauncherBg();
        }
        else if (useMediaPlayer &&
            (lowerPath.endsWith(".mp3")
            || lowerPath.endsWith(".wav")
            || lowerPath.endsWith(".ogg")
            || fileMatchesCategory(path, "audio"))) {

        musicplayer *player = new musicplayer(path, this);
        player->show();
        MainWindow::setLauncherBg();
    }
    else if (useMediaPlayer &&
             (lowerPath.endsWith(".mp4")
             || lowerPath.endsWith(".avi")
             || lowerPath.endsWith(".mpg")
             || lowerPath.endsWith(".mkv")
             || lowerPath.endsWith(".flv")
             || fileMatchesCategory(path, "video"))) {
        videoplayer *player = new videoplayer(path, this);
        player->show();
        MainWindow::setLauncherBg();
    }
    else if (useMediaPlayer &&
             (lowerPath.endsWith(".png")
             || lowerPath.endsWith(".jpg")
             || lowerPath.endsWith(".jpeg")
             || lowerPath.endsWith(".gif")
             || fileMatchesCategory(path, "image"))) {
        pictureviewer* picView = new pictureviewer(path, this);
        picView->show();
        MainWindow::setLauncherBg();
    }
    else {
//        executerbg* exe = new executerbg(path, this);
//        exe->show();
        QDesktopServices::openUrl(
                    QUrl::fromLocalFile(path));
    }
}

void MainWindow::changeTypeToIcon()
{
    this->ui->listView->setViewMode(QListView::IconMode);
    this->ui->listView->setIconSize(QSize(48, 48));
}

void MainWindow::changeTypeToList()
{
    this->ui->listView->setViewMode(QListView::ListMode);
    this->ui->listView->setIconSize(QSize(48, 48));
}

void MainWindow::clearListView()
{
    // Cast the base model to QStandardItemModel and clear rows
    if (auto *model = qobject_cast<QStandardItemModel*>(ui->listView->model())) {
        model->removeRows(0, model->rowCount());
        saveList();
        QMessageBox::information(this,
                                 "Cleared Successfully!",
                                 "<h1>Your lists are cleared!</h1>",
                                 QMessageBox::Ok);
    }
}

bool MainWindow::exportStandardModelToJson(const QString &fileName)
{
    QJsonObject root;
    QJsonArray files;

    for (int i = 0; i < m_model->rowCount(); i++)
    {
        QString path =
            m_model->item(i)->data(Qt::UserRole).toString();

        files.append(path);
    }

    root["files"] = files;

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(
            QJsonDocument(root).toJson(
                QJsonDocument::Indented));
        file.close();

        QMessageBox::information(this,
                                 "Exported!",
                                 "<h1>Your lists are exported at: " + fileName + "</h1>",
                                 QMessageBox::Ok);
    }
    else {
        QMessageBox::critical(this,
                              "Not Exported!",
                              "<h1>Cannot export list!</h1>",
                              QMessageBox::Ok);
        return false;
    }
    return true;
}

bool MainWindow::importJsonToStandardModel(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,
                              "Not Imported!",
                              "<h1>Cannot open and import this list!</h1>",
                              QMessageBox::Ok);
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc =
        QJsonDocument::fromJson(data);

    if (!doc.isObject())
        return false;

    QJsonObject root = doc.object();
    QJsonArray files =
        root.value("files").toArray();

    QFileIconProvider iconProvider;

    m_model->removeRows(0, m_model->rowCount());

    for (const QJsonValue &value : qAsConst(files))
    {
        QString path = value.toString();

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

    this->saveList();
    filterLauncherList(searchBox->text());

    QMessageBox::information(this,
                             "Imported Successfully!",
                             "<h1>Your lists are imported!</h1>",
                             QMessageBox::Ok);

    return true;
}

void MainWindow::updateListDisplay()
{
    QSettings settings("JarredApps", "MayMayLauncher");

    if (settings.value("viewMode", "icons").toString()
            == "icons") {
        if (instance) {
            QMetaObject::invokeMethod(instance,
                                      "changeTypeToIcon",
                                      Qt::QueuedConnection);
        }
    }
    else if (settings.value("viewMode", "icons").toString()
             == "list") {
        if (instance) {
            QMetaObject::invokeMethod(instance,
                                      "changeTypeToList",
                                      Qt::QueuedConnection);
        }
    }
}

void MainWindow::clearAllLists()
{
    if (instance) {
        QMetaObject::invokeMethod(instance,
                                  "clearListView",
                                  Qt::QueuedConnection);
    }
}

void MainWindow::exportLists()
{
    QString filePath = QFileDialog::getSaveFileName(nullptr,
                                                    "Export Launcher Tiles",
                                                    nullptr,
                                                    "MayMayLauncher Launch Tiles File (*.mml);;JSON File (*.json)"
                                                    );

    if (!filePath.isEmpty()) {
        if (instance) {
            QMetaObject::invokeMethod(instance,
                                      "exportStandardModelToJson",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, filePath));
        }
    }
}

void MainWindow::importLists()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr,
                                                    "Import Launcher Tiles",
                                                    nullptr,
                                                    "MayMayLauncher Launch Tiles File (*.mml);;JSON File (*.json)"
                                                    );

    if (!filePath.isEmpty()) {
        QMessageBox::StandardButton comfirmClear = QMessageBox::warning(nullptr,
                                                                        "Note!",
                                                                        "<h1>Importing these lists will clear all your recent lists!</h1>"
                                                                        "</br>"
                                                                        "<p>continue?</p>",
                                                                        QMessageBox::Yes | QMessageBox::No);

        if (comfirmClear == QMessageBox::Yes) {
            if (instance) {
                QMetaObject::invokeMethod(instance,
                                          "importJsonToStandardModel",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, filePath));
            }
        }
    }
}
