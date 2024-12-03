#ifndef AUDIOTOKEN_H
#define AUDIOTOKEN_H

#include <QPainter>
#include "../projectconfiguration.h"
class AudioToken
{
public:
    AudioToken(int audiofileID, double startPosition, double duration, int audioTrack,
               QColor headerColor = ProjectConfiguration::clDefaultTokenHeader,
               QColor mainContentColor = ProjectConfiguration::clDefaultTokenMainContent);
    int tokenID;
    int audiofileID;          //ID аудиофайла
    double startPositionView;     //начало фрагмента на аудиодорожке
    double durationView;           //длительность фрагмента
    double relativeStartTimeView; //начало активной части фрагмента относительно всего фрагмента
    double relativeDurationView;   //длительность активной части фрагмента
    int audioTrack;           //номер аудиодорожки

    static int idCounter;

    void drawToken(QPainter *painter, double scaleFactor) const;
    void updateTokenRelativeStartTime(int newStartTime);
    void updateTokenDuration(double newDuration);

private:
    QColor headerColor;
    QColor mainContentColor;
};

#endif // AUDIOTOKEN_H
