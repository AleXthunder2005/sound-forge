#ifndef AUDIOFILEOBJECT_H
#define AUDIOFILEOBJECT_H

#include <QString>

class AudioFileObject
{
public:
    AudioFileObject();
    AudioFileObject(QString filePath);
    bool canAccess;
    bool isEmpty;
    QString audioFilePath;
};

#endif // AUDIOFILEOBJECT_H
