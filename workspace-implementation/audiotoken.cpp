#include "audiotoken.h"
#include "../audiofilelinker.h"

#include <QPainter>
#include "../projectconfiguration.h"

AudioToken::AudioToken(int id, double startPosition, double duration, int audioTrack)
    : ID(id), startPosition(startPosition), duration(duration), audioTrack(audioTrack),
    relativeStartTime(0), relativeDuration(duration) {}

void AudioToken::drawToken(QPainter *painter) {
    painter->setBrush(Qt::blue);  // Цвет для токена

    int x = this->startPosition;  // Здесь нужно учитывать масштабирование
    int w = this->duration;       // Здесь нужно учитывать масштабирование
    int track = this->audioTrack;

    painter->drawRect(x, track * TRACK_HEIGHT, w, TRACK_HEIGHT);
}


void AudioToken::updateTokenStartTime(int newStartTime) {
    // Убедитесь, что новое значение не превышает границы
    if (newStartTime >= 0 && newStartTime < (relativeStartTime + relativeDuration)) {
        relativeStartTime = newStartTime;
    }
}

void AudioToken::updateTokenDuration(double newDuration) {
    // Убедитесь, что новая длительность больше нуля
    if (newDuration > 0 && (relativeStartTime + newDuration) <= (startPosition + duration)) {
        relativeDuration = newDuration;
    }
}
