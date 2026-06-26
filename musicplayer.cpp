#include "musicplayer.h"
#include "ui_musicplayer.h"

#include <QPixmap>
#include <QPalette>
#include <QTime>

musicplayer::musicplayer(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::musicplayer)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);
    this->setWindowState(Qt::WindowFullScreen);

    fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(500); // Duration in milliseconds
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->start();

    fadeOut = new QPropertyAnimation(this, "windowOpacity");
    fadeOut->setDuration(500);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->start();

//    QPixmap bkgnd("qrc:/bg/metro.jpg");
//    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

//    QPalette palette;
//    palette.setBrush(QPalette::Window, bkgnd);
//    this->setPalette(palette);

    // Disconnect/delete when finished or simply trigger close on completion
    connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);

    ui->label->setText(path);

    player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(path));
    player->setVolume(100); // Volume between 0 and 100
    player->play();

    connect(ui->play, SIGNAL(clicked()), player, SLOT(play()));

    connect(ui->pause, SIGNAL(clicked()), player, SLOT(pause()));

    // Shortcut
    playShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    playShortcut->setContext(Qt::ApplicationShortcut);
    connect(playShortcut, &QShortcut::activated, this, [this]() {
        if (player->state() == QMediaPlayer::PlayingState) {
            emit player->pause();
        } else {
            emit player->play();
        }
    });

    // User drags progress slider to seek through the audio track
    // Note: Use sliderMoved instead of valueChanged to avoid fight-back loops while playing
    connect(ui->horizontalSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);

    // ----------------------------------------------------
    // PLAYER SIGNALS -> UI WIDGETS
    // ----------------------------------------------------
    // Update progress slider track position as audio plays
    connect(player, &QMediaPlayer::positionChanged, ui->horizontalSlider, &QSlider::setValue);

    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 position) {
        int seconds = (position / 1000) % 60;
        int minutes = (position / 60000) % 60;
        int hours = (position / 3600000) % 24;

        QTime time(hours, minutes, seconds);
        ui->length->setText(time.toString("hh:mm:ss"));
    });

    // Update progress slider max boundary when a new file loads
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->horizontalSlider->setRange(0, static_cast<int>(duration));
    });
}

musicplayer::~musicplayer()
{
    delete player;
    delete ui;
}

void musicplayer::showEvent(QShowEvent *event) {
    fadeIn->start();
    QWidget::showEvent(event);
}

void musicplayer::closeEvent(QCloseEvent *event) {
    player->stop();
    fadeOut->start();
    QWidget::closeEvent(event);
}

void musicplayer::paintEvent(QPaintEvent *event)
{
   // This boiler-plate code enables custom QWidget stylesheets
   QStyleOption opt;
   opt.init(this);
   QPainter p(this);
   style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

   // Always call the base class implementation if needed
   QWidget::paintEvent(event);
}

//void musicplayer::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_Space) {
//        if (player->state() == QMediaPlayer::PlayingState) {
//            emit player->pause();
//        } else {
//            emit player->play();
//        }
//    }
//    QWidget::keyPressEvent(event);
//}

void musicplayer::on_rewind_clicked()
{
    // Rewind by 5 seconds (5000 ms)
    qint64 currentPosition = player->position();
    player->setPosition(qMax(qint64(0), currentPosition - 5000));
}

void musicplayer::on_forward_clicked()
{
    // Get the current position in milliseconds
    qint64 currentPos = player->position();

    // Add 5 seconds (5,000 milliseconds)
    qint64 targetPos = currentPos + 5000;

    // Ensure we don't seek past the total duration of the media
    if (targetPos < player->duration()) {
        player->setPosition(targetPos);
    } else {
        player->setPosition(player->duration());
    }
}
