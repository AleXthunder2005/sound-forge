#include "controlpanel.h"
#include "projectconfiguration.h"
#include "audiotrackframe.h"
#include "workspace-implementation/workspacemodel.h"

ControlPanel::ControlPanel(QWidget *parent): QFrame(parent) {
    playButton = new QPushButton("Play", this);
    pauseButton = new QPushButton("Pause", this);
    addTrackButton = new QPushButton("Add track", this);

    addTrackButton->setFixedWidth(ADD_TRACK_BUTTON_WIDTH);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(addTrackButton);
    layout->addWidget(playButton);
    layout->addWidget(pauseButton);

    // Устанавливаем общий стиль для всех кнопок
    setStyleSheet("QPushButton { background-color: " + ProjectConfiguration::clDarkButton.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }"
                + "QPushButton:hover { background-color: " + ProjectConfiguration::clDarkButtonSelected.name() + "; }"
                + "ControlPanel { background-color: " + ProjectConfiguration::clSidePanel.name() + "; }");

    connect(addTrackButton, &QPushButton::clicked, this, &ControlPanel::onAddTrackButtonClicked);
}

void ControlPanel::onAddTrackButtonClicked() {
    // Create a new track and add it to the model
    AudioTrack *newTrack = new AudioTrack();
    AudioTrackFrame::model->addTrack(*newTrack);

    emit trackAdded();
}

