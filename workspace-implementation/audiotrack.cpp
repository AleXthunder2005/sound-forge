#include "audiotrack.h"
#include <QList>
#include <QByteArray>
#include <QMap>
#include <algorithm>
#include <cstring>
#include <QDebug>
#include "../audiofilelinker.h"
#include "../audiotrackframe.h"

AudioTrack::AudioTrack(QObject *parent) : QObject(parent)
{
    trackData = new QByteArray;
    isTrackChanged = false;
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    buffer = new QBuffer;
    isSelected = false;
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &AudioTrack::onMediaStatusChanged);
}

void AudioTrack::addToken(AudioToken &token) {
    tokens.append(token);
}

QList<AudioToken>& AudioTrack::getTokens() {
    return tokens;
}

QByteArray createWavHeader(int dataSize) {
    QByteArray header(WAV_HEADER_SIZE, 0);
    char *headerData = header.data();

    // RIFF заголовок
    std::memcpy(headerData, "RIFF", 4);
    int chunkSize = dataSize + 36; // Общий размер файла - 8 байт
    std::memcpy(headerData + 4, &chunkSize, 4);
    std::memcpy(headerData + 8, "WAVE", 4);

    // fmt подзаголовок
    std::memcpy(headerData + 12, "fmt ", 4);
    int subchunk1Size = 16; // Размер подзаголовка (16 для PCM)
    std::memcpy(headerData + 16, &subchunk1Size, 4);
    short audioFormat = 1; // PCM формат
    std::memcpy(headerData + 20, &audioFormat, 2);
    std::memcpy(headerData + 22, &CHANNELS, 2);
    std::memcpy(headerData + 24, &SAMPLE_RATE, 4);
    int byteRate = SAMPLE_RATE * BLOCK_ALIGN;
    std::memcpy(headerData + 28, &byteRate, 4);
    std::memcpy(headerData + 32, &BLOCK_ALIGN, 2);
    std::memcpy(headerData + 34, &BITS_PER_SAMPLE, 2);

    // data подзаголовок
    std::memcpy(headerData + 36, "data", 4);
    std::memcpy(headerData + 40, &dataSize, 4);

    return header;
}

// Функция для обработки аудиодорожки
void AudioTrack::processAudioTrack() {
    // 1. Сортировка токенов по возрастанию startPositionView
    std::sort(tokens.begin(), tokens.end(),
              [](const AudioToken &a, const AudioToken &b) {
                  return a.startPositionView < b.startPositionView;
              });

    // 2. Объявляем результирующий массив для данных
    QByteArray *combinedAudioData = new QByteArray();

    // Изначально заполняем массив заголовком WAV
    combinedAudioData->append(createWavHeader(0)); // Заголовок обновим позже

    // Переменная для отслеживания текущей позиции в байтах
    int currentBytePos = 0;

    for (const AudioToken &token : tokens) {
        // Получаем аудиоданные из карты файлов
        QByteArray audioFileData = AudioFileLinker::audioFiles[token.audiofileID]->audioData;

        QByteArray audioData = audioFileData.mid(WAV_HEADER_SIZE);

        // Рассчитываем смещение и длину активной части в байтах
        int startByte = static_cast<int>(token.relativeStartTimeView * MS_TO_PX * MS_TO_BYTES);
        int durationBytes = static_cast<int>(token.relativeDurationView * MS_TO_PX * MS_TO_BYTES);

        // Извлекаем активную часть аудиоданных токена
        QByteArray tokenAudioData = audioData.mid(startByte, durationBytes);

        // Рассчитываем позицию начала токена в результирующем массиве
        int tokenStartPos = static_cast<int>(token.startPositionView  * MS_TO_PX * MS_TO_BYTES);

        // Если между текущей позицией и началом токена есть промежуток, заполняем его тишиной
        if (currentBytePos < tokenStartPos) {
            int silenceBytes = tokenStartPos - currentBytePos;
            silenceBytes += (BLOCK_ALIGN - (silenceBytes % BLOCK_ALIGN)) % BLOCK_ALIGN; // Дополняем до кратного BLOCK_ALIGN
            combinedAudioData->append(QByteArray(silenceBytes, 0));
            currentBytePos = tokenStartPos;
        }

        // Проверяем кратность размера данных BLOCK_ALIGN
        if (tokenAudioData.size() % BLOCK_ALIGN != 0) {
            int padding = BLOCK_ALIGN - (tokenAudioData.size() % BLOCK_ALIGN);
            tokenAudioData.append(QByteArray(padding, 0)); // Дополняем до кратного BLOCK_ALIGN
        }

        // Добавляем данные токена в общий массив
        combinedAudioData->append(tokenAudioData);
        currentBytePos += tokenAudioData.size();
    }

    // Обновляем размер data в заголовке WAV
    int dataSize = combinedAudioData->size() - WAV_HEADER_SIZE;
    QByteArray header = createWavHeader(dataSize);
    std::memcpy(combinedAudioData->data(), header.data(), WAV_HEADER_SIZE);

    trackData = combinedAudioData;
    isTrackChanged = false;
}

void AudioTrack::playTrack(qint64 startTime) {
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(80);
    if (buffer->isOpen())
        buffer->close();
    player->setSourceDevice(nullptr);
    buffer->setData(*trackData);

    buffer->open(QIODevice::ReadOnly);
    player->setSourceDevice(buffer);

    player->play();

    // Подключение к сигналу stateChanged
    connect(player, &QMediaPlayer::playbackStateChanged, this, [this, startTime](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::PlayingState) {
            player->setPosition(startTime);
        }
    });

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this, startTime](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            player->pause();

            connect(player, &QMediaPlayer::playbackStateChanged, this, [this, startTime](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::PlayingState) {
                    player->setPosition(0);
                }
            });

            emit audioFileFinished();
            player->play();
        }
    });
}

void AudioTrack::pauseTrack(){
    if (player->isPlaying())
        player->stop();
}

void AudioTrack::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        //buffer->close();
        //player->deleteLater();
        //audioOutput->deleteLater();
        //buffer->deleteLater(); // Освобождаем QBuffer
    }
}


void AudioTrack::setTrackChangeFlag(){
    isTrackChanged = true;
}

void AudioTrack::clearData() {
    QByteArray emptyHeader = createWavHeader(0);
    *trackData = emptyHeader;
}
