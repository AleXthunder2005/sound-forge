#include "audiotrackframe.h"
#include "projectconfiguration.h"
#include "workspace-implementation/workspacemodel.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QScrollBar>

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
    trackTactCount(DEFAULT_TACT_COUNT),
    tactDuration(DEFAULT_TACT_DURATION),
    parentScrollArea(scrollArea),
    scaleFactor(1.0)
{
    setAcceptDrops(true);
    setMouseTracking(true);

    connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &AudioTrackFrame::onVerticalScrollBarChanged);
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
        const AudioTrack &track = model->getTracks().at(i);

        if (isTokenDragging && draggedToken->audioTrack == i) {
            for (int j = 0; j < track.getTokens().size(); j++) {
                if (draggedTokenIndex != j) {
                    AudioToken token = track.getTokens().at(j);
                    token.drawToken(&painter, scaleFactor); // Передаем масштаб
                }
            }
        } else {
            for (AudioToken token : track.getTokens()) {
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
    painter.drawLine(currTime * scaleFactor, 0, currTime * scaleFactor, height()); // Учет масштаба
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
            currTime = mousePos.x() / scaleFactor;      //currTime при 1.0
            isCurrTimeChanging = true;
            update();
            return;
        }

        int xClick = mousePos.x();
        int yClick = mousePos.y() - TIME_BAR_HEIGHT;

        int trackIndex = yClick / TRACK_HEIGHT;
        if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

        const AudioTrack &track = model->getTracks().at(trackIndex);
        for (int tokenIndex = 0; tokenIndex < track.getTokens().size(); ++tokenIndex) {
            const AudioToken &token = track.getTokens().at(tokenIndex);
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
            currTime = mousePos.x() / scaleFactor; // Учет масштаба                                //currTime при 1.0
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
void AudioTrackFrame::onTrackAdded() {
    resizeToFitContent(); // Пересчитать размеры
    update();             // Перерисовать содержимое
}

void AudioTrackFrame::onVerticalScrollBarChanged() {
    update();
}

