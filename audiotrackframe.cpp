#include "audiotrackframe.h"
#include "projectconfiguration.h"
#include "workspace-implementation/workspacemodel.h"
#include <QPainter>
#include <QMouseEvent>

WorkspaceModel* AudioTrackFrame::model = nullptr;

AudioTrackFrame::AudioTrackFrame(QWidget *parent)
    : QFrame(parent),
    draggedToken(nullptr),
    isTokenDragging(false),
    draggedTokenMouseX(0),
    draggedTokenMouseY(0),
    draggedTokenIndex(-1),
    currTime(5),  //пока что 5
    trackTactCount(DEFAULT_TACT_COUNT),
    tactDuration(DEFAULT_TACT_DURATION)
{
    setAcceptDrops(true);
    setMouseTracking(true);
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
    int trackWidth = trackTactCount * tactDuration;

    // Рисуем временную шкалу
    drawTimeBar(painter, trackWidth);

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

        // Токены на дорожке
        if (isTokenDragging && draggedToken->audioTrack == i) {
            for (int j = 0; j < track.getTokens().size(); j++) {
                if (draggedTokenIndex != j) {
                    AudioToken token = track.getTokens().at(j);
                    token.drawToken(&painter);
                }
            }
        } else {
            for (AudioToken token : track.getTokens()) {
                token.drawToken(&painter);
            }
        }

        // Рисуем перетаскиваемый токен
        if (isTokenDragging) {
            painter.setBrush(Qt::red);
            int x = draggedTokenMouseX - (draggedToken->relativeDuration / 2);
            int targetTrack = (draggedTokenMouseY - TIME_BAR_HEIGHT) / TRACK_HEIGHT;
            if (targetTrack >= 0 && targetTrack < model->rowCount() && x >= 0) {
                painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT, draggedToken->relativeDuration, TRACK_HEIGHT);
            }
        }
    }

    // Рисуем текущую позицию проигрывания
    painter.setPen(Qt::red);
    painter.drawLine(currTime, 0, currTime, height());
}

void AudioTrackFrame::drawTimeBar(QPainter &painter, int width) {
    painter.setBrush(ProjectConfiguration::clTimeBar);
    painter.drawRect(0, 0, width, TIME_BAR_HEIGHT);

    painter.setPen(ProjectConfiguration::clTimeBarMark);

    // Рисуем такты и их номера
    for (int i = 0; i < trackTactCount; i++) {
        int x = i * tactDuration;

        // Длинная черта для такта
        painter.drawLine(x, 0, x, TIME_BAR_HEIGHT);

        // Номер такта
        painter.drawText(x + 5, TIME_BAR_HEIGHT * 7 / 8, QString::number(i));

        // Короткие черточки для четвертей
        for (int j = 1; j < 4; j++) {
            int quarterX = x + j * (tactDuration / 4);
            painter.drawLine(quarterX, 0, quarterX, GRID_LINE_HEIGHT);
        }
    }
}

void AudioTrackFrame::drawTrackGrid(QPainter &painter, int trackCount) {


    for (int i = 0; i < trackCount; i++) {
        int y = TIME_BAR_HEIGHT + i * TRACK_HEIGHT;

        for (int j = 0; j < trackTactCount; j++) {
            int x = j * tactDuration;

            //линии такта
            painter.setPen(ProjectConfiguration::clAudioTrackBoldMark);
            painter.drawLine(x, y, x, y + TRACK_HEIGHT);

            // Линии четвертей такта
            for (int k = 1; k < 4; k++) {
                int quarterX = x + k * (tactDuration / 4);
                painter.setPen(ProjectConfiguration::clAudioTrackMark);
                painter.drawLine(quarterX, y, quarterX, y + TRACK_HEIGHT);
            }
        }
    }
}

void AudioTrackFrame::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint mousePos = event->pos();
        int trackIndex = mousePos.y() / TRACK_HEIGHT;
        if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

        const AudioTrack &track = model->getTracks().at(trackIndex);
        for (int tokenIndex = 0; tokenIndex < track.getTokens().size(); ++tokenIndex) {
            const AudioToken &token = track.getTokens().at(tokenIndex);
            int x = token.startPosition;
            int w = token.relativeDuration;

            if (mousePos.x() > x && mousePos.x() < (x + w)) {
                isTokenDragging = true;
                draggedToken = new AudioToken(token); // Use the copy constructor
                draggedTokenIndex = tokenIndex;
                break;
            }
        }
    }
}

void AudioTrackFrame::mouseMoveEvent(QMouseEvent *event) {
    QPoint mousePos = event->pos();

    if (isTokenDragging) {
        draggedTokenMouseX = mousePos.x();
        draggedTokenMouseY = mousePos.y();

        // Привязка к четвертям такта
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            int quarterDuration = tactDuration / 4;
            draggedTokenMouseX = (draggedTokenMouseX / quarterDuration) * quarterDuration;
        }

        update();
    }
}

void AudioTrackFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (isTokenDragging) {
            isTokenDragging = false;

            int dropX = draggedTokenMouseX - (draggedToken->relativeDuration / 2);
            if (!(event->modifiers() & Qt::ShiftModifier)) {
                int quarterDuration = tactDuration / 4;
                dropX = (dropX / quarterDuration) * quarterDuration;
            }

            int dropTrack = (draggedTokenMouseY - TIME_BAR_HEIGHT) / TRACK_HEIGHT;

            model->moveToken(draggedToken->audioTrack, dropTrack, draggedTokenIndex, dropX);
            delete draggedToken;
            draggedToken = nullptr;
        }
        update();
    }
}

void AudioTrackFrame::resizeToFitContent() {
    if (!model) return;

    // Рассчитываем ширину и высоту на основе количества тактов и дорожек
    int width = trackTactCount * tactDuration;
    int height = model->rowCount() * TRACK_HEIGHT;

    // Устанавливаем минимальные размеры для trackFrame
    setMinimumWidth(width);
    setMinimumHeight(height);

    // Обновляем виджет
    updateGeometry();
    update();
}

void AudioTrackFrame::onTrackAdded() {
    resizeToFitContent(); // Пересчитать размеры
    update();             // Перерисовать содержимое
}


