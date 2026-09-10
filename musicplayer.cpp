#include "musicplayer.h"
#include "ui_musicplayer.h"

#include <QPixmap>
#include <QPalette>
#include <QtGlobal>

musicplayer::musicplayer(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::musicplayer)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
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

//    QPixmap bkgnd("qrc:/bg/metro.jpg");
//    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

//    QPalette palette;
//    palette.setBrush(QPalette::Window, bkgnd);
//    this->setPalette(palette);

    // Disconnect/delete when finished or simply trigger close on completion
    connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);

    ui->label->setText(path);

    player = new QMediaPlayer(this);
    audioOut = new QAudioOutput(this);
    player->setAudioOutput(audioOut);
    player->setSource(QUrl::fromLocalFile(path));
    audioOut->setVolume(1.0); // QAudioOutput uses a normalized 0.0-1.0 scale
    player->play();

    connect(ui->play, SIGNAL(clicked()), player, SLOT(play()));

    connect(ui->pause, SIGNAL(clicked()), player, SLOT(pause()));

    // Shortcut
    playShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    playShortcut->setContext(Qt::ApplicationShortcut);
    connect(playShortcut, &QShortcut::activated, this, [this]() {
        if (player->playbackState() == QMediaPlayer::PlayingState) {
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
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 position) {
        ui->horizontalSlider->setValue(
            static_cast<int>(qBound<qint64>(qint64(0), position, qint64(INT_MAX))));
    });

    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 position) {
        const qint64 totalSeconds = qMax<qint64>(0, position / 1000);
        const qint64 hours = totalSeconds / 3600;
        const qint64 minutes = (totalSeconds / 60) % 60;
        const qint64 seconds = totalSeconds % 60;
        ui->length->setText(QString("%1:%2:%3")
                            .arg(hours, 2, 10, QChar('0'))
                            .arg(minutes, 2, 10, QChar('0'))
                            .arg(seconds, 2, 10, QChar('0')));
    });

    // Update progress slider max boundary when a new file loads
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->horizontalSlider->setRange(0, static_cast<int>(qMin<qint64>(duration, INT_MAX)));
    });
}

musicplayer::~musicplayer()
{
    delete player;
    delete ui;
}

void musicplayer::showEvent(QShowEvent *event) {
    closing = false;
    fadeOut->stop();
    fadeIn->start();
    QWidget::showEvent(event);
}

void musicplayer::closeEvent(QCloseEvent *event) {
    if (closing) {
        event->accept();
        return;
    }

    closing = true;
    player->stop();
    fadeOut->start();
    event->ignore();
}

void musicplayer::paintEvent(QPaintEvent *event)
{
   // This boiler-plate code enables custom QWidget stylesheets
   QStyleOption opt;
   opt.initFrom(this);
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
    const qint64 duration = player->duration();
    if (duration <= 0) {
        return;
    }

    player->setPosition(qMin(player->position() + 5000, duration));
}
