#ifndef PICTUREVIEWER_H
#define PICTUREVIEWER_H

#include <QWidget>
#include <QString>

#include <QShowEvent>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>

#include <QPropertyAnimation>

namespace Ui {
class pictureviewer;
}

class pictureviewer : public QWidget
{
    Q_OBJECT

public:
    explicit pictureviewer(QString path, QWidget *parent = 0);
    ~pictureviewer();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::pictureviewer *ui;
    QPropertyAnimation *fadeIn;
    QPropertyAnimation *fadeOut;
};

#endif // PICTUREVIEWER_H
