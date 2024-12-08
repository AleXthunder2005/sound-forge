#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include <QList>
#include "audiotoken.h"
#include <QByteArray>
#include <QMediaPlayer>
#include <QBuffer>
#include <QAudioOutput>

class AudioTrack : public QObject
{
    Q_OBJECT

public:
    explicit AudioTrack(QObject *parent = nullptr);

    // Удаляем конструктор копирования и оператор присваивания
    AudioTrack(const AudioTrack&) = delete;
    AudioTrack& operator=(const AudioTrack&) = delete;

    void addToken(AudioToken &token);
    QList<AudioToken>& getTokens();
    QList<AudioToken> tokens;
    QByteArray *trackData;
    QBuffer *buffer;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    bool isTrackChanged;
    void processAudioTrack();
    void playTrack(qint64 startTime);
    void pauseTrack();
    void setTrackChangeFlag();
    void clearData();

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

signals:
    void audioFileFinished();

};

#endif // AUDIOTRACK_H
