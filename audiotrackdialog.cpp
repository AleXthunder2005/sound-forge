#include "AudioTrackDialog.h"

// Реализация методов AudioTrackDialog
AudioTrackDialog::AudioTrackDialog(int trackIndex, int tokenIndex, AudioFileLinker *linker, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Edit Audio Track");
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Верхнее меню
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = menuBar->addMenu("File");
    QAction *saveAction = fileMenu->addAction("Save");
    connect(saveAction, &QAction::triggered, this, [this]() {
        // Логика сохранения
    });

    menuBar->setStyleSheet(
        "QMenuBar { background-color: " + ProjectConfiguration::clSidePanel.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }" +
        "QMenuBar::item:selected { background-color: " + ProjectConfiguration::clMenuBarItemSelected.name() + "; }" +
        "QMenuBar::item:pressed { background-color: " + ProjectConfiguration::clMenuBarItemSelected.name() + "; }" +
        "QMenu { background-color: " + ProjectConfiguration::clSidePanel.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }" +
        "QMenu::item:selected { background-color: " + ProjectConfiguration::clMenuBarItemSelected.name() + "; }"
        );


    layout->setMenuBar(menuBar);


    // Центральная область с прокруткой
    QScrollArea *scrollArea = new QScrollArea(this);
    dialogTrackFrame = new AudioTrackFrame(DIALOG_TRACK_HEIGHT, this, scrollArea, linker);
    dialogTrackFrame->setMinimumWidth(1000);
    dialogTrackFrame->setMinimumHeight(DIALOG_TRACK_HEIGHT + TIME_BAR_HEIGHT);

    scrollArea->setWidget(dialogTrackFrame);
    scrollArea->setWidgetResizable(true); // Позволяет динамически изменять размеры
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    WorkspaceModel *dialogModel = new WorkspaceModel();

    dialogTrack = new AudioTrack();
    dialogModel->addTrack(dialogTrack);
    dialogTrackFrame->setModel(dialogModel);
    dialogTrackFrame->show();

    layout->addWidget(scrollArea);

    // Нижние кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *playButton = new QPushButton("Play", this);
    QPushButton *pauseButton = new QPushButton("Pause", this);
    QPushButton *stopButton = new QPushButton("Stop", this);
    QPushButton *sliceButton = new QPushButton("Slice", this);
    QPushButton *confirmButton = new QPushButton("Confirm", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(stopButton);
    buttonLayout->addWidget(sliceButton);
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);

    // Устанавливаем общий стиль для всех кнопок
    setStyleSheet("QPushButton { background-color: " + ProjectConfiguration::clDarkButton.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }"
                  + "QPushButton:hover { background-color: " + ProjectConfiguration::clDarkButtonSelected.name() + "; }");

    layout->addLayout(buttonLayout);



    // Подключаем сигнал подтверждения
    connect(confirmButton, &QPushButton::clicked, this, &AudioTrackDialog::confirmChanges);
    connect(cancelButton, &QPushButton::clicked, this, &AudioTrackDialog::cancelChanges);
}

void AudioTrackDialog::confirmChanges() {
    accept();
}

void AudioTrackDialog::cancelChanges() {
    reject();
}
