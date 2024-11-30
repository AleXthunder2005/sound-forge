#ifndef AUDIOTOKEN_H
#define AUDIOTOKEN_H

#include <QPainter>
class AudioToken
{
public:
    AudioToken(int audiofileID, double startPosition, double duration, int audioTrack);
    int tokenID;
    int audiofileID;          //ID аудиофайла
    double startPosition;     //начало фрагмента на аудиодорожке
    double duration;           //длительность фрагмента
    double relativeStartTime; //начало активной части фрагмента относительно всего фрагмента
    double relativeDuration;   //длительность активной части фрагмента
    int audioTrack;           //номер аудиодорожки

    static int idCounter;

    void drawToken(QPainter *painter) const;
    void updateTokenRelativeStartTime(int newStartTime);
    void updateTokenDuration(double newDuration);
};

#endif // AUDIOTOKEN_H
