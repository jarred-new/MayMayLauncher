#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QWidget>
#include <QString>
#include <QCloseEvent>
#include <QShowEvent>
#include <QTimer>
#include <QPropertyAnimation>

#include <fluidsynth.h>

namespace Ui {
class midiplayer;
}

class QCloseEvent;

class midiplayer : public QWidget
{
    Q_OBJECT

public:
    explicit midiplayer(const QString &path, QWidget *parent = nullptr);
    ~midiplayer();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updatePosition();
    void play();
    void pause();
    void rewind();
    void forward();

private:
    bool initializeSynth(const QString &path);
    void stopPlayback();
    void setPosition(int tick);
    QString findSoundFont() const;
    void applyLauncherBackground();

    Ui::midiplayer *ui;
    QPropertyAnimation *fadeIn;
    QPropertyAnimation *fadeOut;
    QTimer *positionTimer;
    fluid_settings_t *settings;
    fluid_synth_t *synth;
    fluid_audio_driver_t *audioDriver;
    fluid_player_t *midiPlayer;
    int pausedTick = 0;
    bool closing = false;
};

#endif // MIDIPLAYER_H
