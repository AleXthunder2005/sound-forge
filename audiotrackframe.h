#ifndef AUDIOTRACKFRAME_H
#define AUDIOTRACKFRAME_H

#include <QFrame>
#include "workspace-implementation/audiotrack.h"
#include "workspace-implementation/workspacemodel.h"

class AudioTrackFrame : public QFrame {
    Q_OBJECT

public:
    bool draggingToken;
    int draggedTokenIndex;
    int draggedTrackIndex;
    double draggedTokenDuration;
    int mouseX;
    int mouseY;

    explicit AudioTrackFrame(QWidget *parent = nullptr);
    void setModel(WorkspaceModel *model);
    void mousePressEvent(QMouseEvent *event) override;
    //void dragEnterEvent(QDragEnterEvent *event) override;
    //void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;


private:
    WorkspaceModel *model;
};


#endif // AUDIOTRACKFRAME_H
