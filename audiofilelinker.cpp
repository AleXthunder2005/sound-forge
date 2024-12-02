#include "audiofilelinker.h"
#include "audiofileobject.h"
#include "projectconfiguration.h"

QList<AudioFileObject*> AudioFileLinker::audioFiles; // Инициализация статического поля

AudioFileLinker::AudioFileLinker() {}

int AudioFileLinker::appendAudioFile(const QString &filePath) {
    AudioFileObject *newItem = new AudioFileObject(filePath);
    audioFiles.append(newItem);
    return getLastID();
}

int AudioFileLinker::getLastID() const {
    return audioFiles.size() - 1;
}

QString AudioFileLinker::getFilePathByID(int ID) const {
    if (ID >= 0 && ID < audioFiles.size()) {
        AudioFileObject *item = audioFiles.at(ID);
        if (item && !item->isEmpty && item->canAccess) {
            return item->audioFilePath;
        }
    }
    return "";
}

double AudioFileLinker::calculateDuration(int ID) {
    if (ID >= 0 && ID < audioFiles.size()) {
        AudioFileObject *item = audioFiles.at(ID);
        return item->durationMs / MS_TO_PX;
    }
    return 0.0; // Возвращаем 0, если ID невалиден
}
