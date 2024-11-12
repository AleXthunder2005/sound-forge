#include "audiofilelinker.h"
#include "audiofileobject.h"

AudioFileLinker::AudioFileLinker() {}

QList<AudioFileObject> AudioFileLinker::audioFiles;   //инициализация поля

int AudioFileLinker::appendAudioFile(QString filePath) {
    AudioFileObject *newItem = new AudioFileObject(filePath);
    AudioFileLinker::audioFiles.append(*newItem);
    return AudioFileLinker::getLastID();
}

int AudioFileLinker::getLastID(){
    return AudioFileLinker::audioFiles.size()-1;
}

QString AudioFileLinker::getFilePathByID (int ID) {
    AudioFileObject item = AudioFileLinker::audioFiles.value(ID);
    if(!item.isEmpty && item.canAccess)
        return item.audioFilePath;

    return "";
}

double AudioFileLinker::calculateDuration(int ID) {
    return 5;  //return this->getAudioFileByID(ID)->четотипо длительности
}
