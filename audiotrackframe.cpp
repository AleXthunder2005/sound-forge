#include "audiotrackframe.h"
#include "projectconfiguration.h"
#include "workspace-implementation/workspacemodel.h"
#include <QPainter>
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>
#include <QMouseEvent>

AudioTrackFrame::AudioTrackFrame(QWidget *parent): QFrame(parent), model(nullptr),
    draggedToken(0, 0.0, 0.0, -1), // Инициализация draggedToken
    resizedToken(0, 0.0, 0.0, -1), // Инициализация resizedToken
    isTokenDragging(false),
    draggedTokenMouseX(0),
    draggedTokenMouseY(0),
    isTokenResizingOnLeft(false),
    isTokenResizingOnRight(false)
{
    setAcceptDrops(true);
    setMouseTracking(true); //включить слежение за мышью
}

void AudioTrackFrame::setModel(WorkspaceModel *model) {
    this->model = model;
    update();  // Перерисовываем фрейм при установке модели
}

void AudioTrackFrame::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    if (!model) return;

    QPainter painter(this);

    for (int i = 0; i < model->rowCount(); i++) {
        const AudioTrack &track = model->getTracks().at(i);

        // Рисуем прямоугольник для аудиодорожки
        painter.setBrush(Qt::lightGray);
        painter.drawRect(0, i * TRACK_HEIGHT, width(), TRACK_HEIGHT);

        // Рисуем токены на аудиодорожке
        if (isTokenDragging && draggedToken.audioTrack == i) {
            for (int j = 0; j < track.getTokens().size(); j++) {
                if (draggedToken.audioTrack != j) {
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
    }

    // Если токен перетаскивается, рисуем его в текущей позиции
    if (isTokenDragging) {
        painter.setBrush(Qt::red); // Цвет для перемещаемого токена
        int x = draggedTokenMouseX - (draggedToken.relativeDuration / 2); // Центруем токен по курсору
        int newTrack = draggedTokenMouseY / TRACK_HEIGHT;

        if (newTrack < 0 || newTrack >= model->rowCount() || x < 0) {
            return; // Выход за пределы
        }

        painter.drawRect(x, newTrack * TRACK_HEIGHT, draggedToken.relativeDuration, TRACK_HEIGHT);
    }
}

void AudioTrackFrame::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint mousePos = event->pos();
        int trackIndex = mousePos.y() / TRACK_HEIGHT;

        if (trackIndex < 0 || trackIndex >= model->rowCount()) {
            return; // Выход за пределы
        }

        const AudioTrack &track = model->getTracks().at(trackIndex);

        for (int tokenIndex = 0; tokenIndex < track.getTokens().size(); ++tokenIndex) {
            const AudioToken &token = track.getTokens().at(tokenIndex);
            int x = token.startPosition + token.relativeStartTime; // Позиция начала токена
            int w = token.relativeDuration;       // Длительность токена

            if (mousePos.x() > x + DELTA && mousePos.x() < (x + w - DELTA)) {
                // Начинаем перетаскивание
                isTokenDragging = true;
                draggedToken = track.getTokens().at(tokenIndex);
                draggedTokenIndex = tokenIndex;
                break; // Токен найден, выходим из цикла
            }
            else {
                isTokenResizingOnLeft = (mousePos.x() >= x - DELTA && mousePos.x() <= x + DELTA);
                if (!isTokenResizingOnLeft)
                    isTokenResizingOnRight = (mousePos.x() >= x + w - DELTA && mousePos.x() <= x + w + DELTA);

                if (isTokenResizingOnLeft || isTokenResizingOnRight) {
                    resizedToken = track.getTokens().at(tokenIndex); //Сохраняем токен
                }
            }
        }
    }
}

void AudioTrackFrame::mouseMoveEvent(QMouseEvent *event) {
    QPoint mousePos = event->pos();
    int trackIndex = mousePos.y() / TRACK_HEIGHT;
    if (trackIndex < 0 || trackIndex >= model->rowCount() || (mousePos.x() - draggedToken.relativeDuration/2) < 0) {
        unsetCursor(); // Сбрасываем курсор, если вышли за пределы
        return; // Выход за пределы
    }

    if (isTokenDragging) {
        draggedTokenMouseX = mousePos.x(); // Обновляем позицию курсора
        draggedTokenMouseY = mousePos.y();
        update(); // Перерисовываем фрейм
    }
    else {
        const AudioTrack &track = model->getTracks().at(trackIndex);
        const QList<AudioToken> &tokens = track.getTokens();

        // Проверка на нахождение курсора на краях токена
        bool isTokenEdge;
        for (const AudioToken &token : tokens) {
            int tokenX = token.startPosition + token.relativeStartTime;
            double tokenWidth = token.relativeDuration;

            isTokenEdge = (mousePos.x() >= tokenX - DELTA && mousePos.x() <= tokenX + DELTA) || (mousePos.x() >= tokenX + tokenWidth - DELTA && mousePos.x() <= tokenX + tokenWidth + DELTA);
            if (isTokenEdge) break;
        }

        // Изменяем курсор на стрелки
        if (isTokenEdge) {
            setCursor(Qt::SizeHorCursor);
        } else
            unsetCursor(); // Сбрасываем курсор, если не на краях

        if (isTokenResizingOnLeft) {
            AudioToken resizingToken = resizedToken;
            double newStartTime = mousePos.x(); // Новая позиция начала
            resizingToken.updateTokenStartTime(newStartTime);
        }
        else if (isTokenResizingOnRight) {
            AudioToken resizingToken = resizedToken;
            double newDuration = mousePos.x() - resizingToken.startPosition; // Новая длительность
            resizingToken.updateTokenDuration(newDuration);
        }
    }
}


void AudioTrackFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isTokenDragging) {
        isTokenDragging = false; // Завершаем перетаскивание
        isTokenResizingOnLeft = false;
        isTokenResizingOnRight = false;

        int dropX = draggedTokenMouseX - (draggedToken.relativeDuration / 2);
        int dropTrack = draggedTokenMouseY / TRACK_HEIGHT;

        // Обновите модель, перемещая токен
        model->moveToken(draggedToken.audioTrack, dropTrack, draggedTokenIndex, dropX);

        update();
    }
}
