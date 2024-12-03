#include "audiotoken.h"
#include "../audiofilelinker.h"

#include <QPainter>
#include "../projectconfiguration.h"

int AudioToken::idCounter = 0;

AudioToken::AudioToken(int audiofileID, double startPosition, double duration, int audioTrack,
                       QColor headerColor,
                       QColor mainContentColor):
    audiofileID(audiofileID),
    startPositionView(startPosition),
    durationView(duration),
    audioTrack(audioTrack),
    relativeStartTimeView(0),
    relativeDurationView(duration),
    headerColor(headerColor),
    mainContentColor(mainContentColor)
{
    tokenID = idCounter++;
}
void AudioToken::drawToken(QPainter *painter, double scaleFactor) const {
    painter->setBrush(Qt::blue);
    int x = this->startPositionView * scaleFactor; // Масштабируем позицию
    int w = this->relativeDurationView * scaleFactor; // Масштабируем ширину
    int y = TIME_BAR_HEIGHT + this->audioTrack * TRACK_HEIGHT;

    int headerHeight = TRACK_HEIGHT * TOKEN_HEADER_RELATIVE_HEIGHT;
    int mainContentHeight = TRACK_HEIGHT * (1 - TOKEN_HEADER_RELATIVE_HEIGHT);

    // Верхняя часть токена с текстом
    painter->setPen(ProjectConfiguration::clTokenText);
    painter->setBrush(headerColor);
    painter->drawRect(x, y, w, headerHeight);
    painter->drawText(x + 5, y + TRACK_HEIGHT / 4, QString::number(audiofileID));

    // Нижняя часть для будущего отображения аудиосигнала
    painter->setBrush(mainContentColor);
    painter->drawRect(x, y + headerHeight, w, mainContentHeight);
}


void AudioToken::updateTokenRelativeStartTime(int newRelativeStartTime) {
    // Убедитесь, что новое значение не превышает границы
    if (newRelativeStartTime >= 0 && newRelativeStartTime < (relativeStartTimeView + relativeDurationView)) {
        relativeStartTimeView = newRelativeStartTime;
    }
}

void AudioToken::updateTokenDuration(double newDuration) {
    // Убедитесь, что новая длительность больше нуля
    if (newDuration > 0 && (relativeStartTimeView + newDuration) <= (startPositionView + durationView)) {
        relativeDurationView = newDuration;
    }
}
