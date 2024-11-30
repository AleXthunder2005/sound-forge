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
    QFrame ::paintEvent(event);
    if (!model) return;
    QPainter painter(this);
    int trackWidth = trackTactCount * tactDuration;

    for (int i = 0; i < model->rowCount(); i++) {
        const AudioTrack &track = model->getTracks().at(i);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(0, i * TRACK_HEIGHT, trackWidth, TRACK_HEIGHT);

        if (isTokenDragging && draggedToken->audioTrack == i) {
            for (int j = 0; j < track.getTokens().size(); j++) {
                if (draggedTokenIndex != j) {
                    AudioToken token = track.getTokens().at(j);
                    token.drawToken(&painter);
                }
            }
        }
        else {
            for (AudioToken token : track.getTokens()) {
                token.drawToken(&painter);
            }
        }

        // Draw the token in its new position if dragging
        if (isTokenDragging) {
            painter.setBrush(Qt::red);
            int x = draggedTokenMouseX - (draggedToken->relativeDuration / 2);
            int targetTrack = draggedTokenMouseY / TRACK_HEIGHT;
            if (targetTrack >= 0 && targetTrack < model->rowCount() && x >= 0) {
                painter.drawRect(x, targetTrack * TRACK_HEIGHT, draggedToken->relativeDuration, TRACK_HEIGHT);
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
    int trackIndex = mousePos.y() / TRACK_HEIGHT;
        if (trackIndex < 0 || trackIndex >= model->rowCount() || (draggedToken && mousePos.x() < draggedToken->relativeDuration / 2)) {
        return;
    }

    if (isTokenDragging) {
        draggedTokenMouseX = mousePos.x();
        draggedTokenMouseY = mousePos.y();
        update();
    }
}

void AudioTrackFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (isTokenDragging) {
                        isTokenDragging = false;
            int dropX = draggedTokenMouseX - (draggedToken->relativeDuration / 2);
            int dropTrack = draggedTokenMouseY / TRACK_HEIGHT;

            model->moveToken(draggedToken->audioTrack, dropTrack, draggedTokenIndex, dropX);
            delete draggedToken; // Clean up
            draggedToken = nullptr; // Avoid dangling pointer

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


