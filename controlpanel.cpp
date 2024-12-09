#include "controlpanel.h"
#include "projectconfiguration.h"
#include "audiotrackframe.h"
#include "workspace-implementation/workspacemodel.h"

ControlPanel::ControlPanel(AudioTrackFrame *trackframe, QWidget *parent): QFrame(parent) {
    playButton = new QPushButton("Play", this);
    pauseButton = new QPushButton("Pause", this);
    stopButton = new QPushButton("Stop", this);
    exitButton = new QPushButton("Exit", this);
    addTrackButton = new QPushButton("Add track", this);
    this->trackFrame = trackframe;

    addTrackButton->setFixedWidth(ADD_TRACK_BUTTON_WIDTH);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(addTrackButton);
    layout->addWidget(playButton);
    layout->addWidget(pauseButton);
    layout->addWidget(stopButton);
    layout->addWidget(exitButton);

    // Устанавливаем общий стиль для всех кнопок
    setStyleSheet("QPushButton { background-color: " + ProjectConfiguration::clDarkButton.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }"
                + "QPushButton:hover { background-color: " + ProjectConfiguration::clDarkButtonSelected.name() + "; }"
                + "ControlPanel { background-color: " + ProjectConfiguration::clSidePanel.name() + "; }");

    connect(addTrackButton, &QPushButton::clicked, this, &ControlPanel::onAddTrackButtonClicked);
    connect(playButton, &QPushButton::clicked, this, &ControlPanel::onPlayButtonClicked);
    connect(pauseButton, &QPushButton::clicked, this, &ControlPanel::onPauseButtonClicked);
    connect(stopButton, &QPushButton::clicked, this, &ControlPanel::onStopButtonClicked);
    connect(exitButton, &QPushButton::clicked, this, &ControlPanel::onExitButtonClicked);
}

void ControlPanel::onAddTrackButtonClicked() {
    // Create a new track and add it to the model
    AudioTrack *newTrack = new AudioTrack();
    trackFrame->model->addTrack(newTrack);

    emit trackAdded();
}

void ControlPanel::onPlayButtonClicked() {

    emit playClicked();
}

void ControlPanel::onPauseButtonClicked() {

    emit pauseClicked();
}

void ControlPanel::onStopButtonClicked() {

    emit stopClicked();
}

void ControlPanel::onExitButtonClicked() {

    emit exitClicked();
}
