#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QWidget>
#include <QString>

#include <QPropertyAnimation>

#include <QShowEvent>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>

#include <QtMultimedia/QMediaPlayer>
#include <QUrl>

namespace Ui {
class musicplayer;
}

class musicplayer : public QWidget
{
    Q_OBJECT

public:
    explicit musicplayer(QString path, QWidget *parent = 0);
    ~musicplayer();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void on_rewind_clicked();

    void on_forward_clicked();

private:
    Ui::musicplayer *ui;
    QPropertyAnimation *fadeIn;
    QPropertyAnimation *fadeOut;
    QMediaPlayer *player;
};

#endif // MUSICPLAYER_H
