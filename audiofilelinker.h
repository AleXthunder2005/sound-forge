#ifndef AUDIOFILELINKER_H
#define AUDIOFILELINKER_H

#include <QList>
#include <QString>
#include "audiofileobject.h"

class AudioFileLinker
{
public:
    static QList<AudioFileObject*> audioFiles; // Используем указатели
    AudioFileLinker();

    int appendAudioFile(const QString &filePath);
    int getLastID() const;
    QString getFilePathByID(int ID) const;
    static double calculateDuration(int ID);
};

#endif // AUDIOFILELINKER_H
