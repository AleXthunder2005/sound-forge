#include "audiotrack.h"

AudioTrack::AudioTrack() {}

void AudioTrack::addToken(const AudioToken &token) {
    tokens.append(token);
}

const QList<AudioToken>& AudioTrack::getTokens() const {
    return tokens;
}
