#include "audiotrackframe.h"
#include "projectconfiguration.h"
#include "workspace-implementation/workspacemodel.h"
#include "audiofilelinker.h"
#include "controlpanel.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QScrollBar>
#include <QMimeData>
#include <QDrag>
#include <QTimer>
#include <QDateTime>

WorkspaceModel* AudioTrackFrame::model = nullptr;

AudioTrackFrame::AudioTrackFrame(QWidget *parent, QScrollArea* scrollArea)
    : QFrame(parent),
    draggedToken(nullptr),
    isTokenDragging(false),
    isCurrTimeChanging(false),
    draggedTokenStartX(0),
    draggedTokenStartY(0),
    draggedTokenIndex(-1),
    currTime(0),
    currViewTime(0),
    startCurrViewTime(0),
    trackTactCount(DEFAULT_TACT_COUNT),
    tactDuration(DEFAULT_TACT_DURATION),
    parentScrollArea(scrollArea),
    scaleFactor(1.0)
{
    setAcceptDrops(true);
    setMouseTracking(true);

    connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &AudioTrackFrame::onVerticalScrollBarChanged);
    connect(this, AudioTrackFrame::currTimeChanged, this, AudioTrackFrame::onCurrTimeChanged);
    connect(this, AudioTrackFrame::timeBarClicked, this, AudioTrackFrame::onPauseClicked);
}

void AudioTrackFrame::setModel(WorkspaceModel *model) {
    this->model = model;
    resizeToFitContent();
    update();
}

WorkspaceModel* AudioTrackFrame::getModel() {
    return model;
}

void AudioTrackFrame::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);
    if (!model) return;

    QPainter painter(this);
    int trackWidth = trackTactCount * tactDuration * scaleFactor; // Масштабируем ширину трека

    // Рисуем дорожки
    for (int i = 0; i < model->rowCount(); i++) {
        const AudioTrack &track = model->getTracks().at(i);

        // Фон дорожки
        painter.setBrush(ProjectConfiguration::clAudioTrack);
        painter.drawRect(0, TIME_BAR_HEIGHT + i * TRACK_HEIGHT, trackWidth, TRACK_HEIGHT);
    }

    // Рисуем сетку для дорожек (каждые 1/4 такта)
    drawTrackGrid(painter, model->rowCount());

    // Рисуем токены на дорожке
    for (int i = 0; i < model->rowCount(); i++) {
        const AudioTrack &track = model->tracks.at(i);

        if (isTokenDragging && draggedToken->audioTrack == i) {
            for (int j = 0; j < track.tokens.size(); j++) {
                if (draggedTokenIndex != j) {
                    AudioToken token = track.tokens.at(j);
                    token.drawToken(&painter, scaleFactor); // Передаем масштаб
                }
            }
        } else {
            for (AudioToken token : track.tokens) {
                token.drawToken(&painter, scaleFactor); // Передаем масштаб
            }
        }

        // Рисуем перетаскиваемый токен
        if (isTokenDragging) {
            painter.setBrush(Qt::red);
            int x = draggedTokenStartX;
            int targetTrack = (draggedTokenStartY) / TRACK_HEIGHT;
            if (targetTrack >= 0 && targetTrack < model->rowCount() && x >= 0) {
                int headerHeight = TRACK_HEIGHT * TOKEN_HEADER_RELATIVE_HEIGHT;
                int mainContentHeight = TRACK_HEIGHT * (1 - TOKEN_HEADER_RELATIVE_HEIGHT);

                // Верхняя часть токена с текстом
                painter.setPen(ProjectConfiguration::clTokenText);
                painter.setBrush(DARK_DRAGGED_TOKEN_COLOR);
                painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT, draggedToken->relativeDuration * scaleFactor, headerHeight);

                // Нижняя часть для будущего отображения аудиосигнала
                painter.setBrush(LIGHT_DRAGGED_TOKEN_COLOR);
                painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT + headerHeight, draggedToken->relativeDuration * scaleFactor, mainContentHeight);
            }
        }
    }

    // Рисуем временную шкалу
    drawTimeBar(painter, trackWidth);

    // Рисуем текущую позицию проигрывания
    painter.setPen(Qt::red);
    painter.drawLine(currViewTime * scaleFactor, 0, currViewTime * scaleFactor, height()); // Учет масштаба
}

void AudioTrackFrame::drawTimeBar(QPainter &painter, int width) {
    painter.setBrush(ProjectConfiguration::clTimeBar);

    int scrollOffset = parentScrollArea->verticalScrollBar()->value();
    painter.drawRect(0, scrollOffset, width, TIME_BAR_HEIGHT);

    painter.setPen(ProjectConfiguration::clTimeBarMark);

    // Рисуем такты и их номера
    for (int i = 0; i < trackTactCount; i++) {
        int x = i * tactDuration * scaleFactor; // Масштабируем такты

        // Длинная черта для такта
        painter.drawLine(x, scrollOffset, x, TIME_BAR_HEIGHT + scrollOffset);

        // Номер такта
        painter.drawText(x + 5, TIME_BAR_HEIGHT * 7 / 8 + scrollOffset, QString::number(i));

        // Короткие черточки для четвертей
        for (int j = 1; j < 4; j++) {
            int quarterX = x + j * (tactDuration / 4) * scaleFactor;
            painter.drawLine(quarterX, scrollOffset, quarterX, GRID_LINE_HEIGHT + scrollOffset);
        }
    }
}

void AudioTrackFrame::drawTrackGrid(QPainter &painter, int trackCount) {
    for (int i = 0; i < trackCount; i++) {
        int y = TIME_BAR_HEIGHT + i * TRACK_HEIGHT;

        for (int j = 0; j < trackTactCount; j++) {
            int x = j * tactDuration * scaleFactor; // Масштабируем такты

            // Линии такта
            painter.setPen(ProjectConfiguration::clAudioTrackBoldMark);
            painter.drawLine(x, y, x, y + TRACK_HEIGHT);

            // Линии четвертей такта
            for (int k = 1; k < 4; k++) {
                int quarterX = x + k * (tactDuration / 4) * scaleFactor;
                painter.setPen(ProjectConfiguration::clAudioTrackMark);
                painter.drawLine(quarterX, y, quarterX, y + TRACK_HEIGHT);
            }
        }
    }
}

void AudioTrackFrame::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint mousePos = event->pos();

        //клик по тайм бару
        int scrollOffset = parentScrollArea->verticalScrollBar()->value();
        if (mousePos.y() > scrollOffset && mousePos.y() < scrollOffset + TIME_BAR_HEIGHT) {
            currViewTime = mousePos.x() / scaleFactor;      //currTime при 1.0
            emit currTimeChanged();
            emit timeBarClicked();
            isCurrTimeChanging = true;
            update();
            return;
        }

        int xClick = mousePos.x();
        int yClick = mousePos.y() - TIME_BAR_HEIGHT;

        int trackIndex = yClick / TRACK_HEIGHT;
        if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

        const AudioTrack &track = model->getTracks().at(trackIndex);
        for (int tokenIndex = 0; tokenIndex < track.tokens.size(); ++tokenIndex) {
            const AudioToken &token = track.tokens.at(tokenIndex);
            double x = token.startPosition * scaleFactor;
            double w = token.relativeDuration * scaleFactor;

            if (xClick > x && xClick < (x + w)) {
                isTokenDragging = true;
                draggedToken = new AudioToken(token);
                draggedTokenDeltaX = xClick - token.startPosition * scaleFactor;   //startPosition при 1.0
                draggedTokenDeltaY = yClick - token.audioTrack * TRACK_HEIGHT;

                draggedTokenStartX = xClick - draggedTokenDeltaX;    //startX и deltaX в больших координатах
                draggedTokenStartY = yClick - draggedTokenDeltaY;
                draggedTokenIndex = tokenIndex;
                break;
            }
        }
    }
}

void AudioTrackFrame::mouseMoveEvent(QMouseEvent *event) {
    QPoint mousePos = event->pos();

    if (isCurrTimeChanging) {
        if (mousePos.x() > 0 && mousePos.x() < trackTactCount * tactDuration * scaleFactor) {
            currViewTime = mousePos.x() / scaleFactor; // Учет масштаба                                //currTime при 1.0
            emit currTimeChanged();
            update();
            return;
        }
    } else if (isTokenDragging) {
        draggedTokenStartX = (mousePos.x() - draggedTokenDeltaX);
        draggedTokenStartY = mousePos.y() - TIME_BAR_HEIGHT;

        // Проверки и корректировки
        if (draggedTokenStartX < 0) draggedTokenStartX = 0;
        if (draggedTokenStartY < 0) draggedTokenStartY = 0;

        int maxStartX = (trackTactCount * tactDuration - draggedToken->relativeDuration) * scaleFactor; //maxStartX в больших
        if (draggedTokenStartX > maxStartX)
            draggedTokenStartX = maxStartX;

        int maxStartY = (model->rowCount() - 1) * TRACK_HEIGHT;
        if (draggedTokenStartY > maxStartY)
            draggedTokenStartY = maxStartY;

        // Привязка к четвертям такта
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = (tactDuration * scaleFactor / 4);
            draggedTokenStartX = round(draggedTokenStartX / quarterDuration) * quarterDuration;
        }

        update();
    }
}

void AudioTrackFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (isTokenDragging) {
            isTokenDragging = false;

            int dropX = draggedTokenStartX;
            if (!(event->modifiers() & Qt::ShiftModifier)) {
                double quarterDuration = tactDuration * scaleFactor / 4;
                dropX = round(draggedTokenStartX / quarterDuration) * quarterDuration;  //dropX в больших координатах
            }

            int dropTrack = (draggedTokenStartY) / TRACK_HEIGHT;

            model->moveToken(draggedToken->audioTrack, dropTrack, draggedTokenIndex, dropX / scaleFactor);
            delete draggedToken;
            draggedToken = nullptr;
        }
        else if (isCurrTimeChanging) isCurrTimeChanging = false;

        update();
    }
}

void AudioTrackFrame::resizeToFitContent() {
    if (!model) return;

    // Рассчитываем ширину и высоту на основе количества тактов и дорожек
    int width = trackTactCount * tactDuration * scaleFactor;
    int height = model->rowCount() * TRACK_HEIGHT;

    // Устанавливаем минимальные размеры для trackFrame
    setMinimumWidth(width);
    setMinimumHeight(height);

    // Обновляем виджет
    updateGeometry();
    update();
}

void AudioTrackFrame::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
        scaleFactor *= factor;
        scaleFactor = qBound(MIN_SCALE, scaleFactor, MAX_SCALE); // Ограничиваем масштаб
        resizeToFitContent();
        update();
    } else {
        QFrame::wheelEvent(event);
    }
}

//drag and drop из списка:
void AudioTrackFrame::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) { // Проверяем MIME-тип
        event->acceptProposedAction();
        isTokenDragging = true;

        QPoint mousePos = event->position().toPoint();
        int xClick = mousePos.x();
        int yClick = mousePos.y() - TIME_BAR_HEIGHT;

        int dropTrack = yClick / TRACK_HEIGHT;

        int uniqueAudiofileID = QString::fromUtf8(event->mimeData()->data("application/x-audiofile")).toInt();

        draggedToken = new AudioToken(uniqueAudiofileID, xClick / scaleFactor, AudioFileLinker::calculateDuration(uniqueAudiofileID), dropTrack);

        draggedTokenDeltaX = 0;   //startPosition при 1.0
        draggedTokenDeltaY = TRACK_HEIGHT / 2;

        draggedTokenStartX = xClick - draggedTokenDeltaX;    //startX и deltaX в больших координатах
        draggedTokenStartY = yClick - draggedTokenDeltaY;
        draggedTokenIndex = -1;
    }
}

void AudioTrackFrame::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        QPoint mousePos = event->position().toPoint();
        draggedTokenStartX = mousePos.x();
        draggedTokenStartY = mousePos.y() - TIME_BAR_HEIGHT;

        // Ограничиваем перетаскиваемые координаты
        if (draggedTokenStartX < 0) draggedTokenStartX = 0;
        if (draggedTokenStartY < 0) draggedTokenStartY = 0;

        int maxStartX = trackTactCount * tactDuration * scaleFactor;
        if (draggedTokenStartX > maxStartX)
            draggedTokenStartX = maxStartX;

        int maxStartY = (model->rowCount() - 1) * TRACK_HEIGHT;
        if (draggedTokenStartY > maxStartY)
            draggedTokenStartY = maxStartY;

        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = (tactDuration * scaleFactor / 4);
            draggedTokenStartX = round(draggedTokenStartX / quarterDuration) * quarterDuration;
        }

        update(); // Перерисовываем временный токен
        event->acceptProposedAction();
    }
}

void AudioTrackFrame::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        int dropX = draggedTokenStartX;
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = tactDuration * scaleFactor / 4;
            dropX = round(draggedTokenStartX / quarterDuration) * quarterDuration;  //dropX в больших координатах
        }
        int dropTrack = (draggedTokenStartY) / TRACK_HEIGHT;

        int uniqueAudiofileID = QString::fromUtf8(event->mimeData()->data("application/x-audiofile")).toInt();

        // Привязка к четвертям такта
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = tactDuration * scaleFactor / 4;
            dropX = round(dropX / quarterDuration) * quarterDuration;
        }

        AudioToken *newToken = new AudioToken(uniqueAudiofileID, dropX / scaleFactor, AudioFileLinker::calculateDuration(uniqueAudiofileID), dropTrack);
        AudioTrack &track = model->tracks[dropTrack];
        track.addToken(*newToken);

        delete(draggedToken);
        draggedToken = nullptr;
        isTokenDragging = false;

        event->acceptProposedAction();
        update();
    }
}



void AudioTrackFrame::onTrackAdded() {
    resizeToFitContent(); // Пересчитать размеры
    update();             // Перерисовать содержимое
}

void AudioTrackFrame::onPlayClicked() {
    startTime = QDateTime::currentMSecsSinceEpoch(); // запоминаем время начала
    startCurrViewTime = currViewTime;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AudioTrackFrame::updateCurrTime);
    timer->start(1); // срабатывает каждые 1 мс
}

void AudioTrackFrame::onPauseClicked() {
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
        startCurrViewTime = currViewTime;
    }
    update();
}

void AudioTrackFrame::updateCurrTime() {
    int elapsedTime = QDateTime::currentMSecsSinceEpoch() - startTime; // прошедшее время
    currTime += elapsedTime;
    currViewTime = round((startCurrViewTime * MS_TO_PX + elapsedTime) / MS_TO_PX);
    update(); // перерисовываем содержимое
}

void AudioTrackFrame::onVerticalScrollBarChanged() {
    update();
}

void AudioTrackFrame::onCurrTimeChanged() {
    startCurrViewTime = currViewTime;
    currTime = currViewTime * MS_TO_PX;
    startTime = QDateTime::currentMSecsSinceEpoch();
}
