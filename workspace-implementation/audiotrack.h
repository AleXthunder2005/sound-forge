#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include <QList>
#include "audiotoken.h"

class AudioTrack
{
public:
    AudioTrack();

    void addToken(AudioToken &token);
    QList<AudioToken>& getTokens();
    QList<AudioToken> tokens;
};

#endif // AUDIOTRACK_H
