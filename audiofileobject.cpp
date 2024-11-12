#include "audiofileobject.h"


AudioFileObject::AudioFileObject() {
    isEmpty = true;
    canAccess = false;
}

AudioFileObject::AudioFileObject(QString filePath) {
    audioFilePath = filePath;
    canAccess = true;
    isEmpty = false;
}
