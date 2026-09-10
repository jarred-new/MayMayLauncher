#include "midiplayer.h"
#include "ui_midiplayer.h"

midiplayer::midiplayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::midiplayer)
{
    ui->setupUi(this);
}

midiplayer::~midiplayer()
{
    delete ui;
}
