#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPropertyAnimation>

#include <QShowEvent>
#include <QCloseEvent>

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QStandardItem>
#include <QSettings>
#include <QDesktopServices>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    QPropertyAnimation *fadeInAnimation;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

//private slots:
//    void on_MainWindow_destroyed();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

private:
    void loadList();
    void saveList();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *m_model;
};

#endif // MAINWINDOW_H
