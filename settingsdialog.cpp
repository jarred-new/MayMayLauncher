#include "settingsdialog.h"
#include "mainwindow.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QProcess>

#include <Windows.h>

#pragma comment(lib, "User32.lib")

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QSettings settings("JarredApps", "MayMayLauncher");

    ui->checkBox->setChecked(
                settings.value("useMediaPlayer", true).toBool());

    ui->checkBox_2->setChecked(
                settings.value("skip_warning_msg", false).toBool());

    if (settings.value("viewMode", "icons").toString()
            == "icons") {
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setChecked(false);
    }
    else if (settings.value("viewMode", "icons").toString()
             == "list") {
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(true);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

//void SettingsDialog::on_checkBox_clicked(bool checked)
//{
//    if (checked == true) {
//        useMediaPlayer = true;
//    }
//    else if (checked == false) {
//        useMediaPlayer = false;
//    }
//}

//void SettingsDialog::on_checkBox_2_clicked(bool checked)
//{
//    if (checked == true) {
//        skip_warning_msg = true;
//    }
//    else if (checked == false) {
//        skip_warning_msg = false;
//    }
//}

void SettingsDialog::on_buttonBox_accepted()
{
    QSettings settings("JarredApps", "MayMayLauncher");

    useMediaPlayer = ui->checkBox->isChecked();
    skip_warning_msg = ui->checkBox_2->isChecked();

    if (ui->radioButton->isChecked() && !ui->radioButton_2->isChecked()) {
        settings.setValue("viewMode", "icons");
    }
    else if (!ui->radioButton->isChecked() && ui->radioButton_2->isChecked()) {
        settings.setValue("viewMode", "list");
    }

    MainWindow::updateListDisplay();

    settings.setValue("skip_warning_msg", skip_warning_msg);
    settings.setValue("useMediaPlayer", useMediaPlayer);
}



void SettingsDialog::on_pushButton_clicked()
{
    QMessageBox::StandardButton comfirmReset1 = QMessageBox::question(this,
                                                                     "Are you sure",
                                                                     "<h1>Do you want to reset your profile?</h1>"
                                                                     "</br>"
                                                                     "<p>this will not affect your settings...</p>",
                                                                     QMessageBox::Yes | QMessageBox::No);

    if (comfirmReset1 == QMessageBox::Yes){
        QSettings settings("JarredApps", "MayMayLauncher");

        settings.setValue("firstTime", true);
        QMessageBox::StandardButton doneMsg = QMessageBox::information(this,
                                                                       "Done!",
                                                                       "Reset success! Click OK to restart this app",
                                                                       QMessageBox::Ok);

        if (doneMsg == QMessageBox::Ok) {
            // restart
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}

void SettingsDialog::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton comfirmReset2 = QMessageBox::question(this,
                                                                     "Are you sure",
                                                                     "<h1>Do you want to reset your profile?</h1>"
                                                                     "</br>"
                                                                     "<p>this <strong>WILL</strong> affect your lists...</p>",
                                                                     QMessageBox::Yes | QMessageBox::No);

    if (comfirmReset2 == QMessageBox::Yes){
        QSettings settings("JarredApps", "MayMayLauncher");

        settings.setValue("firstTime", true);
        MainWindow::clearAllLists();
        QMessageBox::StandardButton doneMsg = QMessageBox::information(this,
                                                                       "Done!",
                                                                       "Reset success! Click OK to restart this app",
                                                                       QMessageBox::Ok);

        if (doneMsg == QMessageBox::Ok) {
            // restart
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}

void SettingsDialog::on_pushButton_5_clicked()
{
    QMessageBox::StandardButton comfirmClear = QMessageBox::information(this,
                                                                        "Are you sure?",
                                                                        "<h1>Do you want to clear all your saved items?</h1>"
                                                                        "</br>"
                                                                        "<p>make sure you backed up all your lists"
                                                                        "</br>continue?</p>",
                                                                        QMessageBox::Yes | QMessageBox::No);

    if (comfirmClear == QMessageBox::Yes) {
        MainWindow::clearAllLists();
    }
}

void SettingsDialog::on_pushButton_3_clicked()
{
    MainWindow::importLists();
}

void SettingsDialog::on_pushButton_4_clicked()
{
    MainWindow::exportLists();
}

void SettingsDialog::on_pushButton_6_clicked()
{
    ShellExecuteW(NULL, L"open", L"https://github.com/jarred-new/MayMayLauncher/releases", NULL, NULL, SW_SHOW);
}
