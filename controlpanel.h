#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
#include "audiotrackframe.h"

class ControlPanel : public QFrame {
    Q_OBJECT

public:
    ControlPanel(AudioTrackFrame *trackframe, QWidget *parent = nullptr);

private:
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *addTrackButton;
    AudioTrackFrame* trackFrame;

private slots:
    void onAddTrackButtonClicked(); // Slot for button click
    void onPlayButtonClicked();
    void onPauseButtonClicked();

signals:
    void trackAdded();
    void playClicked();
    void pauseClicked();
};

#endif // CONTROLPANEL_H
