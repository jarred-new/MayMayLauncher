#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QWidget>

namespace Ui {
class midiplayer;
}

class midiplayer : public QWidget
{
    Q_OBJECT

public:
    explicit midiplayer(QWidget *parent = nullptr);
    ~midiplayer();

private:
    Ui::midiplayer *ui;
};

#endif // MIDIPLAYER_H
