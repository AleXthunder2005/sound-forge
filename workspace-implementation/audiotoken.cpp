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
    mainContentColor(mainContentColor),
    isSelected(false)
{
    tokenID = idCounter++;
}
void AudioToken::drawToken(QPainter *painter, double scaleFactor, const int trackHeight) const {
    painter->setBrush(Qt::blue);
    int x = this->startPositionView * scaleFactor; // Масштабируем позицию
    int w = this->relativeDurationView * scaleFactor; // Масштабируем ширину
    int y = TIME_BAR_HEIGHT + this->audioTrack * TRACK_HEIGHT;

    int headerHeight = trackHeight * TOKEN_HEADER_RELATIVE_HEIGHT;
    int mainContentHeight = trackHeight * (1 - TOKEN_HEADER_RELATIVE_HEIGHT);

    // Верхняя часть токена с текстом
    painter->setPen(ProjectConfiguration::clTokenText);
    painter->setBrush(isSelected ? ProjectConfiguration::clSelectedAudioTokenHeader : headerColor);
    painter->drawRect(x, y, w, headerHeight);
    painter->drawText(x + 5, y + trackHeight / 4, QString::number(audiofileID));

    // Нижняя часть для будущего отображения аудиосигнала
    painter->setBrush(isSelected ? ProjectConfiguration::clSelectedAudioTokenMain : mainContentColor);
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

void AudioToken::resetSelectedFlag() {
    isSelected = false;
}

void AudioToken::setSelectedFlag() {
    isSelected = true;
}
