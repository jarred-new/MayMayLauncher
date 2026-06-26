#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QString>

#include <QtMultimedia/QMediaPlayer>
#include <QUrl>
#include <QtMultimediaWidgets/QVideoWidget>

#include <QShowEvent>
#include <QCloseEvent>
#include <QPaintEvent>
//#include <QKeyEvent>
#include <QShortcut>
#include <QStyleOption>
#include <QPainter>

#include <QPropertyAnimation>

namespace Ui {
class videoplayer;
}

class videoplayer : public QWidget
{
    Q_OBJECT

public:
    explicit videoplayer(QString path, QWidget *parent = 0);
    ~videoplayer();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
//    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_rewind_clicked();

    void on_forward_clicked();

private:
    Ui::videoplayer *ui;
    QPropertyAnimation *fadeIn;
    QPropertyAnimation *fadeOut;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QShortcut* playShortcut;
};

#endif // VIDEOPLAYER_H
