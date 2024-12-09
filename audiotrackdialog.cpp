#include "AudioTrackDialog.h"

// Реализация методов AudioTrackDialog
AudioTrackDialog::AudioTrackDialog(int trackIndex, int tokenIndex, AudioFileLinker *linker, QWidget *parent)
    : QDialog(parent),
    //isDialogTokenDragging(false),
    //dialogDraggedToken(nullptr),
    //scaleFactor(1.0),
    dialogTrackHeight(100)
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
    layout->setMenuBar(menuBar);


    // Центральная область с прокруткой
    QScrollArea *scrollArea = new QScrollArea(this);
    dialogTrackFrame = new AudioTrackFrame(this, scrollArea, linker);
    dialogTrackFrame->setMinimumWidth(1000);
    dialogTrackFrame->setMinimumHeight(dialogTrackHeight + TIME_BAR_HEIGHT);

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

    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(stopButton);
    buttonLayout->addWidget(sliceButton);
    buttonLayout->addWidget(confirmButton);

    layout->addLayout(buttonLayout);

    // Подключаем сигнал подтверждения
    connect(confirmButton, &QPushButton::clicked, this, &AudioTrackDialog::confirmChanges);

    //dialogTrackFrame->installEventFilter(this);
}

// QList<AudioToken> AudioTrackDialog::getTokens() const {
//     return dialogTokens;
// }

// void AudioTrackDialog::paintEvent(QPaintEvent *event) {
//     QPainter painter = new QPainter(dialogTrack);

//     // Рисуем дорожку
//     painter.setBrush(ProjectConfiguration::clAudioTrack);
//     painter.drawRect(0, TIME_BAR_HEIGHT, dialogTrack->width(), dialogTrackHeight);

//     // Рисуем временную шкалу
//     drawTimeBar(painter);

//     // Рисуем токены
//     for (const auto &token : dialogTokens) {
//         token.drawToken(&painter, scaleFactor, dialogTrackHeight);
//     }

//     // Рисуем перетаскиваемый токен
//     if (isDialogTokenDragging) {
//         painter.setBrush(Qt::red);
//         int x = dialogDdraggedTokenStartX;
//         int targetTrack = (draggedTokenStartY) / TRACK_HEIGHT;
//         if (targetTrack >= 0 && targetTrack < model->rowCount() && x >= 0) {
//             int headerHeight = TRACK_HEIGHT * TOKEN_HEADER_RELATIVE_HEIGHT;
//             int mainContentHeight = TRACK_HEIGHT * (1 - TOKEN_HEADER_RELATIVE_HEIGHT);

//             // Верхняя часть токена с текстом
//             painter.setPen(ProjectConfiguration::clTokenText);
//             painter.setBrush(DARK_DRAGGED_TOKEN_COLOR);
//             painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT, dialogDraggedToken->relativeDurationView * scaleFactor, headerHeight);

//             // Нижняя часть для будущего отображения аудиосигнала
//             painter.setBrush(LIGHT_DRAGGED_TOKEN_COLOR);
//             painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT + headerHeight, dialogDraggedToken->relativeDurationView * scaleFactor, mainContentHeight);
//         }
//     }

//     delete(painter);
// }

// void AudioTrackDialog::drawTimeBar(QPainter &painter) {
//     painter.setBrush(Qt::gray);
//     painter.drawRect(0, 0, dialogTrack->width(), TIME_BAR_HEIGHT);

//     painter.setPen(Qt::black);

//     for (int i = 0; i < trackTactCount; i++) {
//         int x = i * tactDuration * scaleFactor;

//         // Длинная черта такта
//         painter.drawLine(x, 0, x, TIME_BAR_HEIGHT);

//         // Номер такта
//         painter.drawText(x + 5, TIME_BAR_HEIGHT - 5, QString::number(i + 1));

//         // Короткие черточки для четвертей
//         for (int j = 1; j < 4; j++) {
//             int quarterX = x + j * (tactDuration / 4) * scaleFactor;
//             painter.drawLine(quarterX, TIME_BAR_HEIGHT - 10, quarterX, TIME_BAR_HEIGHT);
//         }
//     }
// }

// bool AudioTrackDialog::eventFilter(QObject *object, QEvent *event) {
//     if (object == dialogTrack) {
//         if (event->type() == QEvent::MouseButtonPress) {
//             auto *mouseEvent = static_cast<QMouseEvent *>(event);
//             handleMousePress(mouseEvent);
//         } else if (event->type() == QEvent::MouseMove) {
//             auto *mouseEvent = static_cast<QMouseEvent *>(event);
//             handleMouseMove(mouseEvent);
//         } else if (event->type() == QEvent::MouseButtonRelease) {
//             auto *mouseEvent = static_cast<QMouseEvent *>(event);
//             handleMouseRelease(mouseEvent);
//         }
//     }
//     return QDialog::eventFilter(object, event);
// }

// void AudioTrackDialog::handleMousePress(QMouseEvent *event) {
//     int xClick = event->pos().x() / scaleFactor;

//     for (int i = 0; i < dialogTokens.size(); ++i) {
//         auto &token = dialogTokens[i];
//         if (xClick > token.startPositionView && xClick < (token.startPositionView + token.relativeDurationView)) {
//             isDialogTokenDragging = true;
//             dialogDraggedToken = &token;
//             dialogDraggedTokenDeltaX = event->pos().x() - token.startPositionView * scaleFactor;
//             dialogDraggedTokenIndex = i;
//             break;
//         }
//     }
// }

// void AudioTrackDialog::handleMouseMove(QMouseEvent *event) {
//     if (isDialogTokenDragging && dialogDraggedToken) {
//         dialogDdraggedTokenStartX = event->pos().x() - dialogDraggedTokenDeltaX;
//         if (dialogDdraggedTokenStartX < 0) dialogDdraggedTokenStartX = 0;

//         update();
//     }
// }

// void AudioTrackDialog::handleMouseRelease(QMouseEvent *event) {
//     if (isDialogTokenDragging && dialogDraggedToken) {
//         dialogDraggedToken->startPositionView = dialogDdraggedTokenStartX / scaleFactor;
//         isDialogTokenDragging = false;
//         dialogDraggedToken = nullptr;

//         update();
//     }
// }

void AudioTrackDialog::confirmChanges() {
    accept();
}
