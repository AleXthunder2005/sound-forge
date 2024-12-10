#include "audiotoken.h"


#include <QPainter>
#include "../projectconfiguration.h"
#include "../audiofilelinker.h"

int AudioToken::idCounter = 0;

AudioToken::AudioToken(int audiofileID, double startPosition, double duration, int audioTrack,
                       AudioFileLinker *linker,
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
    isSelected(false),
    linker(linker)
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
    painter->drawText(x + 5, y + headerHeight / 4 * 3, QString::number(audiofileID));

    // Нижняя часть для отображения аудиосигнала
    painter->setBrush(isSelected ? ProjectConfiguration::clSelectedAudioTokenMainContent : mainContentColor);
    painter->drawRect(x, y + headerHeight, w, mainContentHeight);

    // Рисуем аналоговый сигнал
    if (linker) {
        const QByteArray &audioData = linker->audioFiles[audiofileID]->audioData;
        if (audioData.size() > 44) { // Убедимся, что есть данные после заголовка
            const int16_t *samples = reinterpret_cast<const int16_t*>(audioData.constData() + 44); // Пропускаем 44 байта заголовка
            int sampleCount = (audioData.size() - 44) / sizeof(int16_t); // Количество выборок
            double durationInMs = this->relativeDurationView * MS_TO_PX; // Длительность токена в миллисекундах
            double startInMs = this->relativeStartTimeView * MS_TO_PX; // Начальное время токена в миллисекундах
            double samplesPerMs = sampleCount / linker->audioFiles[audiofileID]->durationMs; // Количество выборок на миллисекунду

            int startSample = static_cast<int>(startInMs * samplesPerMs); // Номер начальной выборки
            int sampleRange = static_cast<int>(durationInMs * samplesPerMs); // Количество выборок, охватываемых токеном

            if (startSample + sampleRange > sampleCount) {
                sampleRange = sampleCount - startSample; // Ограничиваем выборки, чтобы не выходить за пределы массива
            }

            // Пропорция между шириной токена и количеством выборок
            double samplesPerPixel = static_cast<double>(sampleRange) / w;

            // Рисуем сигнал
            painter->setPen(Qt::white); // Цвет линии сигнала
            int centerY = y + headerHeight + mainContentHeight / 2; // Центр нижней части токена

            int stepWidth = 3; // Увеличиваем ширину одной ступеньки (в пикселях)
            double amplitudeMultiplier = 1.5; // Коэффициент для увеличения высоты сигнала

            for (int i = 0; i < (w - 1); i += stepWidth) {
                int sampleIndex = startSample + static_cast<int>(i * samplesPerPixel);
                if (sampleIndex < startSample + sampleRange) {
                    int16_t sampleValue = samples[sampleIndex]; // Значение выборки
                    double normalizedAmplitude = static_cast<double>(sampleValue) / INT16_MAX; // Нормализуем амплитуду
                    int amplitudeHeight = static_cast<int>(normalizedAmplitude * (mainContentHeight / 2) * amplitudeMultiplier); // Увеличиваем высоту сигнала

                    // Рисуем полосу (увеличенную ступеньку)
                    painter->drawRect(x + i, centerY - amplitudeHeight, stepWidth, amplitudeHeight * 2); // Полоса вместо линии
                }
            }
        }
    }
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
