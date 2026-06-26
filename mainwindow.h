#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPropertyAnimation>

#include <QShowEvent>
#include <QCloseEvent>
#include <QKeyEvent>

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
    static void updateListDisplay();
    static void clearAllLists();
    static void exportLists();
    static void importLists();

//signals:
//    void enterPressed(const QModelIndex &index);

protected:
    QPropertyAnimation *fadeInAnimation;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    bool fileMatchesCategory(const QString& filePath, const QString& category);
    void changeProfilePicture();
    void changeProfileName();

//private slots:
//    void on_MainWindow_destroyed();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

public slots:
    void changeTypeToIcon();
    void changeTypeToList();
    void clearListView();
    bool exportStandardModelToJson(const QString& fileName);
    bool importJsonToStandardModel(const QString& fileName);

private:
    void loadList();
    void saveList();

private:
    Ui::MainWindow *ui;
    static MainWindow *instance; // Add a static pointer
    QStandardItemModel *m_model;
};

#endif // MAINWINDOW_H
