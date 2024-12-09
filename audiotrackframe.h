#ifndef AUDIOTRACKFRAME_H
#define AUDIOTRACKFRAME_H

#include <QFrame>
#include "workspace-implementation/audiotoken.h"
#include "workspace-implementation/audiotrack.h"
#include "workspace-implementation/workspacemodel.h"
#include <QScrollArea>
#include "audiofilelinker.h"

class AudioTrackFrame : public QFrame {
    Q_OBJECT

public:
    AudioToken *draggedToken;
    bool isTokenDragging;
    bool isCurrTimeChanging;

    double draggedTokenStartX;      //такие значения храним при масштабе 1.0
    double draggedTokenStartY;
    int draggedTokenIndex;
    double draggedTokenDeltaX; //расстояние от startPosition до точки захвата
    double draggedTokenDeltaY; //расстояние от startPosition до точки захвата
    WorkspaceModel *model;
    int trackTactCount;
    double tactDuration;

    explicit AudioTrackFrame(QWidget *parent = nullptr, QScrollArea *scrollArea = nullptr, AudioFileLinker *linker = nullptr);
    void setModel(WorkspaceModel *model);
    WorkspaceModel* getModel();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeToFitContent();
    void wheelEvent(QWheelEvent *event) override;

    void showTrackContextMenu(const QPoint &pos, int trackIndex);
    void showTokenContextMenu(const QPoint &pos, int trackIndex, int tokenIndex);
    void deleteTrack(int trackIndex);
    void deleteToken(int trackIndex, int tokenIndex);
    void openEditTokenWindow(int trackIndex, int tokenIndex);



    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void playAudioTokens(); // Метод для запуска воспроизведения


protected:
    void drawTimeBar(QPainter &painter, int width);
    void drawTrackGrid(QPainter &painter, int trackCount);

public slots:
    void onTrackAdded(); // Slot to handle track addition
    void onPlayClicked();
    void onPauseClicked();
    void onVerticalScrollBarChanged();
    void onCurrTimeChanged();
    void onTrackChanged(int index);
    void onAudioFileFinished();

signals:
    void currTimeChanged();
    void timeBarClicked();
    void trackChanged(int index);

private:
    QScrollArea* parentScrollArea;
    AudioFileLinker *fileLinker;
    double scaleFactor; // Масштабирование
    QTimer *timer;    // таймер для отслеживания времени
    int startTime;    // время начала воспроизведения в миллисекундах
    qint64 currTime;
    int currViewTime;
    int startViewTime;

    AudioTrack *totalTrack;

    void updateCurrTime(); // метод для обновления currTime
    QByteArray mixWavFiles(QByteArray *wav1, QByteArray *wav2);
};

#endif // AUDIOTRACKFRAME_H
