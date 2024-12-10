#include "AudioTrackDialog.h"

// Реализация методов AudioTrackDialog
AudioTrackDialog::AudioTrackDialog(WorkspaceModel *baseModel, int trackIndex, int tokenIndex, AudioFileLinker *linker, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Edit Audio Track");
    resize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint & ~Qt::WindowMinimizeButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    this->baseModel = baseModel;
    editingToken = &(baseModel->tracks[trackIndex]->tokens[tokenIndex]);

    // Верхнее меню
    QMenuBar *menuBar = new QMenuBar(this);
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

    AudioToken editingTokenCopy = baseModel->tracks[trackIndex]->tokens[tokenIndex];
    editingTokenCopy.startPositionView = 0;
    editingTokenCopy.audioTrack = 0;
    dialogTrack->addToken(editingTokenCopy);
    dialogTrack->isTrackChanged = true;

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

    // Ваша существующая часть кода
    QMenu *fileMenu = menuBar->addMenu("File");
    QMenu *helpMenu = menuBar->addMenu("Help");

    // Действие для экспорта
    QAction *exportAction = fileMenu->addAction("Export");
    connect(exportAction, &QAction::triggered, this, [this]() {
        // Открываем диалоговое окно для выбора места сохранения
        QString fileName = QFileDialog::getSaveFileName(this, "Save WAV File", "wavfile.wav", "WAV Files (*.wav)");

        // // Проверяем, выбрано ли имя файла
        // if (!fileName.isEmpty()) {
        //     // Создаем файл для записи
        //     QFile file(fileName);
        //     if (file.open(QIODevice::WriteOnly)) {
        //         // Записываем данные из trackData в файл
        //         dialogTrack->processAudioTrack();
        //         QByteArray data = *(dialogTrack->trackData); // Предполагается, что trackData - это QByteArray
        //         file.write(data);
        //         file.close();
        //     } else {
        //         // Обработка ошибки открытия файла
        //         QMessageBox::warning(this, "Error", "Could not open file for writing.");
        //     }
        // }

        if (!fileName.isEmpty()) {
            qDebug() << "File name:" << fileName;

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                dialogTrack->processAudioTrack();

                if (dialogTrack->trackData == nullptr || dialogTrack->trackData->isEmpty()) {
                    qDebug() << "trackData is null or empty";
                    QMessageBox::warning(this, "Error", "No data to write.");
                    return;
                }

                QByteArray data = *(dialogTrack->trackData);
                qDebug() << "Data size:" << data.size();

                qint64 bytesWritten = file.write(data);
                if (bytesWritten == -1) {
                    qDebug() << "Error writing to file:" << file.errorString();
                    QMessageBox::warning(this, "Error", "Failed to write data to file.");
                } else {
                    qDebug() << "Successfully written" << bytesWritten << "bytes.";
                }
                file.close();
            } else {
                qDebug() << "Error opening file:" << file.errorString();
                QMessageBox::warning(this, "Error", "Could not open file for writing.");
            }
        }
    });

    // Действие для Help
    QAction *helpAction = helpMenu->addAction("Help");
    connect(helpAction, &QAction::triggered, this, [this]() {
        // Создаем диалоговое окно
        QDialog *helpDialog = new QDialog(this);
        helpDialog->setWindowTitle("Help");

        // Создаем QLabel с текстом
        QLabel *helpLabel = new QLabel("This is a simple audio editor. Use the 'Export' option to save your audio tracks as WAV files.", helpDialog);

        // Создаем кнопку для закрытия диалога
        QPushButton *closeButton = new QPushButton("Close", helpDialog);
        connect(closeButton, &QPushButton::clicked, helpDialog, &QDialog::accept);

        // Устанавливаем вертикальный layout
        QVBoxLayout *layout = new QVBoxLayout(helpDialog);
        layout->addWidget(helpLabel);
        layout->addWidget(closeButton);

        // Устанавливаем layout для диалога
        helpDialog->setLayout(layout);

        // Показываем диалог
        helpDialog->exec();
    });

    // Подключаем сигнал подтверждения
    connect(confirmButton, &QPushButton::clicked, this, &AudioTrackDialog::confirmChanges);
    connect(cancelButton, &QPushButton::clicked, this, &AudioTrackDialog::cancelChanges);
    connect(sliceButton, &QPushButton::clicked, this, &AudioTrackDialog::onSliceButtonClicked);

    connect(pauseButton, &QPushButton::clicked, dialogTrackFrame, &AudioTrackFrame::onPauseClicked);
    connect(playButton, &QPushButton::clicked, dialogTrackFrame, &AudioTrackFrame::onPlayClicked);
    connect(stopButton, &QPushButton::clicked, dialogTrackFrame, &AudioTrackFrame::onStopClicked);
}

void AudioTrackDialog::confirmChanges() {
    for (AudioToken &token : dialogTrack->tokens) {
        token.startPositionView += editingToken->startPositionView;
        token.audioTrack = editingToken->audioTrack;
    }
    dialogTokens = dialogTrack->tokens;
    dialogTrackFrame->isSlicing = false;
    dialogTrack->pauseTrack();
    accept();
}

void AudioTrackDialog::cancelChanges() {
    dialogTrackFrame->isSlicing = false;
    dialogTrack->pauseTrack();
    reject();
}

void AudioTrackDialog::onSliceButtonClicked() {
    dialogTrackFrame->isSlicing = !(dialogTrackFrame->isSlicing);
    dialogTrackFrame->setFocus();
}
