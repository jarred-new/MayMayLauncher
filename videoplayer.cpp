#include "videoplayer.h"
#include "ui_videoplayer.h"

#include <QPixmap>
#include <QPalette>

videoplayer::videoplayer(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::videoplayer)
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

    connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);

    player = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(this);

    player->setVideoOutput(videoWidget);

    ui->verticalLayout_2->addWidget(videoWidget);

    player->setMedia(QUrl::fromLocalFile(path));
    player->play();

    connect(ui->play, SIGNAL(clicked()), player, SLOT(play()));

    connect(ui->pause, SIGNAL(clicked()), player, SLOT(pause()));

    // User drags progress slider to seek through the audio track
    // Note: Use sliderMoved instead of valueChanged to avoid fight-back loops while playing
    connect(ui->horizontalSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);

    // ----------------------------------------------------
    // PLAYER SIGNALS -> UI WIDGETS
    // ----------------------------------------------------
    // Update progress slider track position as audio plays
    connect(player, &QMediaPlayer::positionChanged, ui->horizontalSlider, &QSlider::setValue);

    // Update progress slider max boundary when a new file loads
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->horizontalSlider->setRange(0, static_cast<int>(duration));
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
    fadeIn->start();
    QWidget::showEvent(event);
}

void videoplayer::closeEvent(QCloseEvent *event)
{
    player->stop();
    fadeOut->start();
    QWidget::closeEvent(event);
}

void videoplayer::paintEvent(QPaintEvent *event)
{
    // This boiler-plate code enables custom QWidget stylesheets
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // Always call the base class implementation if needed
    QWidget::paintEvent(event);
}

void videoplayer::on_rewind_clicked()
{
    // Rewind by 5 seconds (5000 ms)
    qint64 currentPosition = player->position();
    player->setPosition(qMax(qint64(0), currentPosition - 5000));
}

void videoplayer::on_forward_clicked()
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
