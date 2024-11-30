#include "audiotoken.h"
#include "../audiofilelinker.h"

#include <QPainter>
#include "../projectconfiguration.h"

int AudioToken::idCounter = 0;

AudioToken::AudioToken(int audiofileID, double startPosition, double duration, int audioTrack):
    audiofileID(audiofileID),
    startPosition(startPosition),
    duration(duration),
    audioTrack(audioTrack),
    relativeStartTime(0),
    relativeDuration(duration)
{

    tokenID = idCounter++;
}

void AudioToken::drawToken (QPainter *painter) const{
    painter->setBrush(Qt::blue);  // Цвет для токена

    int x = this->startPosition;  // Здесь нужно учитывать масштабирование
    int w = this->relativeDuration;       // Здесь нужно учитывать масштабирование
    int track = this->audioTrack;

    painter->drawRect(x, TIME_BAR_HEIGHT + track * TRACK_HEIGHT, w, TRACK_HEIGHT);
}


void AudioToken::updateTokenRelativeStartTime(int newRelativeStartTime) {
    // Убедитесь, что новое значение не превышает границы
    if (newRelativeStartTime >= 0 && newRelativeStartTime < (relativeStartTime + relativeDuration)) {
        relativeStartTime = newRelativeStartTime;
    }
}

void AudioToken::updateTokenDuration(double newDuration) {
    // Убедитесь, что новая длительность больше нуля
    if (newDuration > 0 && (relativeStartTime + newDuration) <= (startPosition + duration)) {
        relativeDuration = newDuration;
    }
}
