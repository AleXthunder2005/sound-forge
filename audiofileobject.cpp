#include "AudioFileObject.h"
#include <QFile>
#include <QAudioDecoder>

AudioFileObject::AudioFileObject(QObject *parent)
    : QObject(parent), canAccess(false), isEmpty(true), durationMs(0), sampleRate(0), bitRate(0), channels(0)
{
}

AudioFileObject::AudioFileObject(const QString &filePath, QObject *parent)
    : AudioFileObject(parent)
{
    loadAudioFile(filePath);
}

bool AudioFileObject::loadAudioFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "File does not exist:" << filePath;
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    audioData = file.readAll(); // Load data into memory
    audioFilePath = filePath;
    canAccess = true;
    isEmpty = audioData.isEmpty();
    file.close();

    analyzeAudio(); // Analyzing audio
    return true;
}

void AudioFileObject::analyzeAudio()
{
    if (!canAccess || audioData.isEmpty()) {
        qWarning() << "Audio data is empty or inaccessible.";
        return;
    }

    mediaPlayer.setSource(QUrl::fromLocalFile(audioFilePath));
    connect(&mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &AudioFileObject::onMediaStatusChanged);
    mediaPlayer.play(); // Start playback to load the media
}

void AudioFileObject::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        durationMs = mediaPlayer.duration(); // Duration in milliseconds
        bitRate = mediaPlayer.metaData().value(QMediaMetaData::AudioBitRate).toInt();
        //channels = mediaPlayer.metaData().value(QMediaMetaData::AudioChannelCount).toInt();
        mediaPlayer.stop(); // Stop playback after fetching the metadata
    }
    // else if (status == QMediaPlayer::Error) {
    //     qWarning() << "Error loading media:" << mediaPlayer.errorString();
    // }
}
