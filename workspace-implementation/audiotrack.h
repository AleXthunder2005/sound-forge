#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include <QList>
#include "audiotoken.h"
#include <QByteArray>

class AudioTrack
{
public:
    AudioTrack();

    void addToken(AudioToken &token);
    QList<AudioToken>& getTokens();
    QList<AudioToken> tokens;
    QByteArray *trackData;
    QByteArray *processAudioTrack();
};

#endif // AUDIOTRACK_H
