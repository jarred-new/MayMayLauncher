#include "pictureviewer.h"
#include "ui_pictureviewer.h"

#include <QPixmap>
#include <QMovie>
#include <QSize>

pictureviewer::pictureviewer(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pictureviewer)
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

    if (!path.endsWith(".gif")) {
        pic = new QPixmap(path);

        ui->pic->setPixmap(pic->scaled(
                    ui->pic->size(),
                    Qt::AspectRatioMode::KeepAspectRatio));

        //ui->pic->setPixmap(pic);
    }
    else {
        gif = new QMovie(path);

        ui->pic->setMovie(gif);
        gif->start();
    }
}

pictureviewer::~pictureviewer()
{
    delete ui;

    if (pic) {
        delete pic;
    }
    else if (gif) {
        delete gif;
    }
}

void pictureviewer::showEvent(QShowEvent *event)
{
    fadeIn->start();
    QWidget::showEvent(event);
}

void pictureviewer::closeEvent(QCloseEvent *event)
{
    if (pic) {
        ui->pic->clear();
    }
    else if (gif) {
        gif->stop();
        //gif->deleteLater();
    }
    fadeOut->start();
    QWidget::closeEvent(event);
}

void pictureviewer::paintEvent(QPaintEvent *event)
{
    // This boiler-plate code enables custom QWidget stylesheets
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // Always call the base class implementation if needed
    QWidget::paintEvent(event);
}
