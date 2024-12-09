#ifndef AUDIOTRACKDIALOG_H
#define AUDIOTRACKDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QScrollBar>
#include <QVector>
#include "projectconfiguration.h"
#include "workspace-implementation/audiotoken.h"
#include "workspace-implementation/audiotrack.h"
#include "workspace-implementation/workspacemodel.h"
#include "audiotrackframe.h"
#include "QWidget"

class AudioTrackDialog : public QDialog {
    Q_OBJECT

public:
    explicit AudioTrackDialog(int trackIndex, int tokenIndex, AudioFileLinker *linker, QWidget *parent = nullptr);
    //QList<AudioToken> getTokens() const;

protected:
    //void paintEvent(QPaintEvent *event) override;
    //bool eventFilter(QObject *object, QEvent *event) override;

private:
    // void drawTimeBar(QPainter &painter);
    // void handleMousePress(QMouseEvent *event);
    // void handleMouseMove(QMouseEvent *event);
    // void handleMouseRelease(QMouseEvent *event);
    void confirmChanges();

private:
    AudioTrack *dialogTrack;
    QList<AudioToken> dialogTokens;
    AudioTrackFrame* dialogTrackFrame;

    double dialogScaleFactor;
    int dialogTrackHeight;

    bool isDialogTokenDragging;
    AudioToken *dialogDraggedToken;
    int dialogDraggedTokenIndex;
    double dialogDraggedTokenDeltaX;
    double dialogDdraggedTokenStartX;
};

#endif // AUDIOTRACKDIALOG_H
