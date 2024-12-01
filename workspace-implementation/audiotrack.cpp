#include "audiotrack.h"

AudioTrack::AudioTrack() {}

void AudioTrack::addToken(AudioToken &token) {
    tokens.append(token);
}

QList<AudioToken>& AudioTrack::getTokens() {
    return tokens;
}
