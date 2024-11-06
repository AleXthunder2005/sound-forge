#include "controlpanel.h"
#include "projectconfiguration.h"

ControlPanel::ControlPanel(QWidget *parent): QFrame(parent) {
    playButton = new QPushButton("Play", this);
    pauseButton = new QPushButton("Pause", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(playButton);
    layout->addWidget(pauseButton);

    // Устанавливаем общий стиль для всех кнопок
    setStyleSheet("QPushButton { background-color: " + ProjectConfiguration::clDarkButton.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }"
                + "QPushButton:hover { background-color: " + ProjectConfiguration::clDarkButtonSelected.name() + "; }"
                + "ControlPanel { background-color: " + ProjectConfiguration::clSidePanel.name() + "; }");
}
