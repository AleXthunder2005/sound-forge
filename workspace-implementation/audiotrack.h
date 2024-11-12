#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include <QList>
#include "audiotoken.h"

class AudioTrack
{
public:
    AudioTrack();

    void addToken(const AudioToken &token);
    const QList<AudioToken>& getTokens() const;
    QList<AudioToken> tokens;
};

#endif // AUDIOTRACK_H
