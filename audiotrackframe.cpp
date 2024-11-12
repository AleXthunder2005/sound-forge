#include "audiotrackframe.h"
#include "projectconfiguration.h"
#include "workspace-implementation/workspacemodel.h"
#include <QPainter>
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>
#include <QMouseEvent>

AudioTrackFrame::AudioTrackFrame(QWidget *parent)
    : QFrame(parent), model(nullptr), draggingToken(false), draggedTokenIndex(-1), draggedTrackIndex(-1), mouseX(0), draggedTokenDuration(0) {
    setAcceptDrops(true);
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
        if (draggingToken && draggedTrackIndex == i) {
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
    }

    // Если токен перетаскивается, рисуем его в текущей позиции
    if (draggingToken) {
        painter.setBrush(Qt::red); // Цвет для перемещаемого токена
        int x = mouseX - (draggedTokenDuration / 2); // Центруем токен по курсору
        int newTrack = mouseY / TRACK_HEIGHT;

        if (newTrack < 0 || newTrack >= model->rowCount() || x < 0) {
            return; // Выход за пределы
        }

        painter.drawRect(x, newTrack * TRACK_HEIGHT, draggedTokenDuration, TRACK_HEIGHT);
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
            int x = token.startPosition; // Позиция начала токена
            int w = token.duration;       // Длительность токена

            draggingToken = false;
            if (mousePos.x() > x + DELTA && mousePos.x() < (x + w - DELTA)) {
                // Начинаем перетаскивание
                draggingToken = true;
                draggedTokenIndex = tokenIndex;
                draggedTrackIndex = trackIndex;
                draggedTokenDuration = token.duration;
                break; // Токен найден, выходим из цикла
            }
        }
    }
}

void AudioTrackFrame::mouseMoveEvent(QMouseEvent *event) {
    QPoint mousePos = event->pos();
    int trackIndex = mousePos.y() / TRACK_HEIGHT;
    if (trackIndex < 0 || trackIndex >= model->rowCount() || mousePos.x() < 0) {
        unsetCursor(); // Сбрасываем курсор, если вышли за пределы
        return; // Выход за пределы
    }

    if (draggingToken) {
        mouseX = mousePos.x(); // Обновляем позицию курсора
        mouseY = mousePos.y();
        update(); // Перерисовываем фрейм
    }
    else {
        const AudioTrack &track = model->getTracks().at(trackIndex);
        const QList<AudioToken> &tokens = track.getTokens();

        // Проверка на нахождение курсора на краях токена
        bool onLeftEdge = false;
        bool onRightEdge = false;
        AudioToken* resizingToken = nullptr; // Используем указатель на токен

        // for (const AudioToken &token : tokens) {
        //     int tokenX = token.startPosition;
        //     double tokenWidth = token.duration;

        //     if (mouseX >= tokenX - DELTA && mouseX <= tokenX + DELTA) {
        //         onLeftEdge = true;
        //         resizingToken = const_cast<AudioToken*>(&token); // Сохраняем указатель на токен
        //         break;
        //     } else if (mouseX >= tokenX + tokenWidth - DELTA && mouseX <= tokenX + tokenWidth + DELTA) {
        //         onRightEdge = true;
        //         resizingToken = const_cast<AudioToken*>(&token); // Сохраняем указатель на токен
        //         break;
        //     }
        // }

        // bool isResizing = (onLeftEdge || onRightEdge);
        // if (isResizing) {
        //     setCursor(Qt::SizeHorCursor); // Изменяем курсор на стрелки
        // } else {
        //     unsetCursor(); // Сбрасываем курсор, если не на краях
        // }

        // // Логика изменения размера токена
        // if (!draggingToken && isResizing) {
        //     if (onLeftEdge) {
        //         // Изменяем relativeStartTime
        //         double newStartTime = mouseX; // Новая позиция начала
        //         resizingToken->updateTokenStartTime(newStartTime);
        //     } else if (onRightEdge) {
        //         // Изменяем relativeDuration
        //         double newDuration = mouseX - resizingToken->startPosition; // Новая длительность
        //         resizingToken->updateTokenDuration(newDuration);
        //     }
        //     update();
        // }
    }

}


void AudioTrackFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && draggingToken) {
        draggingToken = false; // Завершаем перетаскивание
        //unsetCursor(); // Сбрасываем курсор

        // Обработка сброса токена в новую позицию
        //QPoint dropPos = event->pos();
        //int dropX = dropPos.x() - (draggedTokenDuration / 2); // Центруем токен по курсору
        //int dropTrack = dropPos.y() / TRACK_HEIGHT;

        //if (dropTrack < 0 || dropTrack >= model->rowCount() || dropX < 0) {
        //    return; // Выход за пределы
        //}

        int dropX = mouseX - (draggedTokenDuration / 2);
        int dropTrack = mouseY / TRACK_HEIGHT;

        // Обновите модель, перемещая токен
        model->moveToken(draggedTrackIndex, dropTrack, draggedTokenIndex, dropX);

        update();
    }
}
