#include "midiplayer.h"
#include "ui_midiplayer.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QSettings>
#include <QUrl>

#include <algorithm>

namespace {
QString formatDuration(int tick, int division, int bpm)
{
    if (division <= 0 || bpm <= 0) {
        return QStringLiteral("00:00:00");
    }

    const qint64 seconds = (static_cast<qint64>(tick) * 60) /
                           (static_cast<qint64>(division) * bpm);
    return QStringLiteral("%1:%2:%3")
        .arg(seconds / 3600, 2, 10, QChar('0'))
        .arg((seconds / 60) % 60, 2, 10, QChar('0'))
        .arg(seconds % 60, 2, 10, QChar('0'));
}
}

midiplayer::midiplayer(const QString &path, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::midiplayer)
    , positionTimer(new QTimer(this))
    , settings(nullptr)
    , synth(nullptr)
    , audioDriver(nullptr)
    , midiPlayer(nullptr)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);
    setWindowState(Qt::WindowFullScreen);
    ui->label->setText(QFileInfo(path).fileName());
    applyLauncherBackground();

    fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    fadeOut = new QPropertyAnimation(this, "windowOpacity");
    fadeOut->setDuration(500);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);

    // Shortcut
    playShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    playShortcut->setContext(Qt::ApplicationShortcut);
    connect(playShortcut, &QShortcut::activated, this, [this]() {
        if (midiPlayer) {
            if (fluid_player_get_status(midiPlayer) == FLUID_PLAYER_PLAYING) {
                emit pause();
            } else {
                emit play();
            }
        }
    });

    connect(ui->play, &QPushButton::clicked, this, &midiplayer::play);
    connect(ui->pause, &QPushButton::clicked, this, &midiplayer::pause);
    connect(ui->rewind, &QPushButton::clicked, this, &midiplayer::rewind);
    connect(ui->forward, &QPushButton::clicked, this, &midiplayer::forward);
    connect(ui->horizontalSlider, &QSlider::sliderMoved,
            this, &midiplayer::setPosition);
    connect(positionTimer, &QTimer::timeout, this, &midiplayer::updatePosition);
    connect(ui->dial, &QDial::valueChanged, this, [this](int value) {
        if (synth) {
            fluid_synth_set_gain(synth, static_cast<float>(value) / 100.0f);
        }
    });
    positionTimer->start(100);

    if (!initializeSynth(path)) {
        ui->play->setEnabled(false);
        ui->pause->setEnabled(false);
        ui->rewind->setEnabled(false);
        ui->forward->setEnabled(false);
    }
}

midiplayer::~midiplayer()
{
    stopPlayback();
    delete playShortcut;
    delete ui;
}

QString midiplayer::findSoundFont() const
{
    QSettings appSettings("JarredApps", "MayMayLauncher");
    const QString configuredPath = appSettings.value("Midi/SoundFont").toString();
    const QStringList candidates = {
        configuredPath,
        QCoreApplication::applicationDirPath() + "/midi/soundfonts/default.sf2"//,
        //QCoreApplication::applicationDirPath() + "/midi/soundfonts/default.sf3"
    };

    for (const QString &candidate : candidates) {
        if (!candidate.isEmpty() && QFileInfo::exists(candidate)) {
            return candidate;
        }
    }

    return QString();
}

bool midiplayer::initializeSynth(const QString &path)
{
    QString soundFont = findSoundFont();
    if (soundFont.isEmpty()) {
        QMessageBox::information(this, "Choose a SoundFont",
                                 "<h1>MIDI playback needs a SoundFont (.sf2 or .sf3).</h1>"
                                 "Choose one to use for this and future MIDI files.");
        soundFont = QFileDialog::getOpenFileName(
            this, "Open SoundFont", QString(),
            "SoundFonts (*.sf2 *.sf3)");
        if (soundFont.isEmpty()) {
            this->close();
            return false;
        }
        QSettings("JarredApps", "MayMayLauncher")
            .setValue("Midi/SoundFont", soundFont);
    }

    settings = new_fluid_settings();
    synth = settings ? new_fluid_synth(settings) : nullptr;
    if (!settings || !synth) {
        return false;
    }

    const QByteArray soundFontPath = QFile::encodeName(soundFont);
    if (fluid_synth_sfload(synth, soundFontPath.constData(), 1) < 0) {
        QMessageBox::critical(this, "MIDI playback unavailable",
                              "FluidSynth could not load the selected SoundFont.");
        stopPlayback();
        return false;
    }

    fluid_settings_setstr(settings, "audio.driver", "wasapi");
    audioDriver = new_fluid_audio_driver(settings, synth);
    if (!audioDriver) {
        fluid_settings_setstr(settings, "audio.driver", "dsound");
        audioDriver = new_fluid_audio_driver(settings, synth);
    }
    midiPlayer = new_fluid_player(synth);
    if (!audioDriver || !midiPlayer) {
        QMessageBox::critical(this, "MIDI playback unavailable",
                              "FluidSynth could not initialize audio playback.");
        stopPlayback();
        return false;
    }

    const QByteArray midiPath = QFile::encodeName(path);
    if (fluid_player_add(midiPlayer, midiPath.constData()) != 0) {
        QMessageBox::critical(this, "MIDI playback unavailable",
                              "FluidSynth could not open this MIDI file.");
        stopPlayback();
        return false;
    }

    const int totalTicks = std::max(1, fluid_player_get_total_ticks(midiPlayer));
    ui->horizontalSlider->setRange(0, totalTicks);
    ui->length->setText(formatDuration(totalTicks,
                                       fluid_player_get_division(midiPlayer),
                                       fluid_player_get_bpm(midiPlayer)));
    if (fluid_player_play(midiPlayer) != FLUID_OK) {
        QMessageBox::critical(this, "MIDI playback unavailable",
                              "FluidSynth could not start MIDI playback. "
                              "Check the audio output device and SoundFont.");
        stopPlayback();
        return false;
    }
    return true;
}

void midiplayer::stopPlayback()
{
    positionTimer->stop();
    if (midiPlayer) {
        fluid_player_stop(midiPlayer);
        fluid_player_join(midiPlayer);
        delete_fluid_player(midiPlayer);
        midiPlayer = nullptr;
    }
    if (audioDriver) {
        delete_fluid_audio_driver(audioDriver);
        audioDriver = nullptr;
    }
    if (synth) {
        delete_fluid_synth(synth);
        synth = nullptr;
    }
    if (settings) {
        delete_fluid_settings(settings);
        settings = nullptr;
    }
}

void midiplayer::updatePosition()
{
    if (!midiPlayer) {
        return;
    }

    const int totalTicks = std::max(1, fluid_player_get_total_ticks(midiPlayer));
    const int tick = std::clamp(fluid_player_get_current_tick(midiPlayer),
                                0, totalTicks);
    if (ui->horizontalSlider->maximum() != totalTicks) {
        ui->horizontalSlider->setRange(0, totalTicks);
    }
    QSignalBlocker blocker(ui->horizontalSlider);
    ui->horizontalSlider->setValue(tick);
    ui->length->setText(formatDuration(tick,
                                       fluid_player_get_division(midiPlayer),
                                       fluid_player_get_bpm(midiPlayer)));
}

void midiplayer::play()
{
    if (midiPlayer) {
        const int totalTicks = fluid_player_get_total_ticks(midiPlayer);
        if (fluid_player_get_current_tick(midiPlayer) >= totalTicks) {
            pausedTick = 0;
        }
        if (pausedTick > 0) {
            fluid_player_seek(midiPlayer, pausedTick);
            pausedTick = 0;
        }
        fluid_player_play(midiPlayer);
    }
}

void midiplayer::pause()
{
    if (midiPlayer) {
        pausedTick = std::max(0, fluid_player_get_current_tick(midiPlayer));
        fluid_player_stop(midiPlayer);
    }
}

void midiplayer::setPosition(int tick)
{
    if (midiPlayer) {
        pausedTick = std::clamp(tick, 0, ui->horizontalSlider->maximum());
        fluid_player_seek(midiPlayer, pausedTick);
    }
}

void midiplayer::rewind()
{
    if (midiPlayer) {
        setPosition(fluid_player_get_current_tick(midiPlayer) - 480);
    }
}

void midiplayer::forward()
{
    if (midiPlayer) {
        setPosition(fluid_player_get_current_tick(midiPlayer) + 480);
    }
}

void midiplayer::closeEvent(QCloseEvent *event)
{
    if (closing) {
        event->accept();
        return;
    }

    closing = true;
    stopPlayback();
    fadeOut->start();
    event->ignore();
}

void midiplayer::paintEvent(QPaintEvent *event)
{
    // This boiler-plate code enables custom QWidget stylesheets
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // Always call the base class implementation if needed
    QWidget::paintEvent(event);
}

void midiplayer::showEvent(QShowEvent *event)
{
    closing = false;
    fadeOut->stop();
    applyLauncherBackground();
    fadeIn->start();
    QWidget::showEvent(event);
}

void midiplayer::applyLauncherBackground()
{
    QSettings appSettings("JarredApps", "MayMayLauncher");
    QString backgroundPath = appSettings.value(
        "Launcher/Background", ":/bg/metro.jpg").toString();

    if (!backgroundPath.startsWith(":/")
        && !QFileInfo::exists(backgroundPath)) {
        backgroundPath = ":/bg/metro.jpg";
    }

    const QString backgroundUrl = backgroundPath.startsWith(":/")
        ? backgroundPath
        : QDir::fromNativeSeparators(backgroundPath);

    setStyleSheet(QString(
        "#midiplayer { border-image: url('%1') 0 0 0 0 stretch stretch; }")
        .arg(backgroundUrl));
    update();
}
