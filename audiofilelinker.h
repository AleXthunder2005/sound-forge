#ifndef AUDIOFILELINKER_H
#define AUDIOFILELINKER_H

#include <QList>
#include "audiofileobject.h"

class AudioFileLinker
{
public:
    static QList<AudioFileObject> audioFiles;
    AudioFileLinker();

    int appendAudioFile(QString filePath);
    int getLastID();
    QString getFilePathByID (int ID);
    static double calculateDuration(int ID);
};

#endif // AUDIOFILELINKER_H
