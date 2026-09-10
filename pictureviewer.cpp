#include "pictureviewer.h"
#include "ui_pictureviewer.h"

#include <QPixmap>
#include <QMovie>
#include <QResizeEvent>
#include <QSize>

pictureviewer::pictureviewer(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pictureviewer)
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

    if (!path.toLower().endsWith(".gif")) {
        pic = new QPixmap(path);
        ui->pic->setAlignment(Qt::AlignCenter);
        ui->pic->setPixmap(pic->scaled(ui->pic->size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation));
    }
    else {
        gif = new QMovie(path);

        ui->pic->setMovie(gif);
        ui->pic->setAlignment(Qt::AlignCenter);
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
    closing = false;
    fadeOut->stop();
    fadeIn->start();
    QWidget::showEvent(event);
}

void pictureviewer::closeEvent(QCloseEvent *event)
{
    if (closing) {
        event->accept();
        return;
    }

    closing = true;
    if (pic) {
        ui->pic->clear();
    }
    else if (gif) {
        gif->stop();
        //gif->deleteLater();
    }
    fadeOut->start();
    event->ignore();
}

void pictureviewer::resizeEvent(QResizeEvent *event)
{
    if (pic && !pic->isNull()) {
        ui->pic->setPixmap(pic->scaled(ui->pic->size(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
    }
    QWidget::resizeEvent(event);
}

void pictureviewer::paintEvent(QPaintEvent *event)
{
    // This boiler-plate code enables custom QWidget stylesheets
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // Always call the base class implementation if needed
    QWidget::paintEvent(event);
}
