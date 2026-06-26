#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
//    void on_checkBox_2_clicked(bool checked);

    void on_buttonBox_accepted();

//    void on_checkBox_clicked(bool checked);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::SettingsDialog *ui;

protected:
    bool useMediaPlayer;
    bool skip_warning_msg;
};

#endif // SETTINGSDIALOG_H
