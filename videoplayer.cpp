#include "videoplayer.h"
#include "ui_videoplayer.h"

#include <QPixmap>
#include <QPalette>
#include <QtGlobal>
#include <QToolTip>
#include <QCursor>

videoplayer::videoplayer(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::videoplayer)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    this->setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);
    this->setWindowState(Qt::WindowFullScreen);
    ui->dial->setTracking(true);

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

    connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);

    player = new QMediaPlayer(this);
    audioOut = new QAudioOutput(this);
    videoWidget = new QVideoWidget(this);

    audioOut->setVolume(static_cast<float>(ui->dial->value()) / 100.0f);
    player->setAudioOutput(audioOut);
    player->setVideoOutput(videoWidget);

    ui->verticalLayout_2->addWidget(videoWidget);

    player->setSource(QUrl::fromLocalFile(path));
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

    connect(ui->dial, &QDial::valueChanged, this, [this](int value) {
        if (audioOut) {
            audioOut->setVolume(static_cast<float>(value) / 100.0f);

            // Format the text
            QString tooltipText = QString::number(value);
            
            // Anchor it above the center of the dial itself:
            QPoint globalPos = ui->dial->mapToGlobal(QPoint(ui->dial->width() / 2, 0));

            // Show the tooltip instantly without a delay
            QToolTip::showText(globalPos, tooltipText, ui->dial);
        }
    });
}

videoplayer::~videoplayer()
{
    delete player;
    delete videoWidget;
    delete ui;
}

void videoplayer::showEvent(QShowEvent *event)
{
    closing = false;
    fadeOut->stop();
    fadeIn->start();
    QWidget::showEvent(event);
}

void videoplayer::closeEvent(QCloseEvent *event)
{
    if (closing) {
        event->accept();
        return;
    }

    closing = true;
    player->stop();
    fadeOut->start();
    event->ignore();
}

void videoplayer::paintEvent(QPaintEvent *event)
{
    // This boiler-plate code enables custom QWidget stylesheets
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // Always call the base class implementation if needed
    QWidget::paintEvent(event);
}

//void videoplayer::keyPressEvent(QKeyEvent *event)
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

void videoplayer::on_rewind_clicked()
{
    player->setPosition(qMax<qint64>(qint64(0), player->position() - 5000));
}

void videoplayer::on_forward_clicked()
{
    const qint64 duration = player->duration();
    if (duration <= 0) {
        return;
    }

    player->setPosition(qMin(player->position() + 5000, duration));
}
