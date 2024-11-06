#include "audiotrackframe.h"
#include "projectconfiguration.h"

AudioTrackFrame::AudioTrackFrame(QWidget *parent): QFrame(parent) {
    setStyleSheet("QFrame { background-color: " + ProjectConfiguration::clAudioTrackFrame.name() + "; }");
}
