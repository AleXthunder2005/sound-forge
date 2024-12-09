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
    double startPositionView;     //начало фрагмента на аудиодорожке в px
    double durationView;           //длительность фрагмента  в px
    double relativeStartTimeView; //начало активной части фрагмента относительно всего фрагмента  в px
    double relativeDurationView;   //длительность активной части фрагмента  в px
    int audioTrack;           //номер аудиодорожки
    bool isSelected;
    static int idCounter;

    void drawToken(QPainter *painter, double scaleFactor, const int trackHeight) const;
    void updateTokenRelativeStartPosition(int newStartTime);
    void updateTokenRelativeDuration(double newDuration);
    void resetSelectedFlag();
    void setSelectedFlag();
private:
    QColor headerColor;
    QColor mainContentColor;

};

#endif // AUDIOTOKEN_H
