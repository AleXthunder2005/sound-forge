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
    int x = this->startPositionView * scaleFactor; // Масштабируем позицию
    int w = this->relativeDurationView * scaleFactor; // Масштабируем ширину
    int y = TIME_BAR_HEIGHT + this->audioTrack * trackHeight;

    int headerHeight = TRACK_HEIGHT * TOKEN_HEADER_RELATIVE_HEIGHT;
    int mainContentHeight = trackHeight - headerHeight;

    // Верхняя часть токена с текстом
    painter->setPen(ProjectConfiguration::clTokenText);
    painter->setBrush(isSelected ? ProjectConfiguration::clSelectedAudioTokenHeader : headerColor);
    painter->drawRect(x, y, w, headerHeight);
    painter->drawText(x + 5, y + headerHeight /4 *3, QString::number(audiofileID));

    // Нижняя часть для будущего отображения аудиосигнала
    painter->setBrush(isSelected ? ProjectConfiguration::clSelectedAudioTokenMainContent : mainContentColor);
    painter->drawRect(x, y + headerHeight, w, mainContentHeight);
}


void AudioToken::updateTokenRelativeStartPosition(int newRelativeStartTimeView) {
    // Убедитесь, что новое значение не превышает границы
    if (newRelativeStartTimeView >= 0 && newRelativeStartTimeView < (relativeStartTimeView + relativeDurationView)) {
        relativeStartTimeView = newRelativeStartTimeView;
    }
}

void AudioToken::updateTokenRelativeDuration(double newDuration) {
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
