#ifndef AUDIOTRACKFRAME_H
#define AUDIOTRACKFRAME_H

#include <QFrame>
#include "workspace-implementation/audiotoken.h"
#include "workspace-implementation/audiotrack.h"
#include "workspace-implementation/workspacemodel.h"
#include <QScrollArea>

class AudioTrackFrame : public QFrame {
    Q_OBJECT

public:
    AudioToken *draggedToken;
    bool isTokenDragging;
    int draggedTokenMouseX;
    int draggedTokenMouseY;
    int draggedTokenIndex;

    int trackTactCount;
    int tactDuration;

    explicit AudioTrackFrame(QWidget *parent = nullptr);
    void setModel(WorkspaceModel *model);
    WorkspaceModel* getModel();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateViewportSize();
    static WorkspaceModel *model;
    int horizontalOffset() const;
    int verticalOffset() const;

public slots:
    void onTrackAdded(); // Slot to handle track addition

};


#endif // AUDIOTRACKFRAME_H
