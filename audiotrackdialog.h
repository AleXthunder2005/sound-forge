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
    explicit AudioTrackDialog(WorkspaceModel *baseModel, int trackIndex, int tokenIndex, AudioFileLinker *linker, QWidget *parent = nullptr);
    QList<AudioToken> dialogTokens;

private:
    void confirmChanges();
    void cancelChanges();

private:
    AudioTrackFrame* dialogTrackFrame;
    WorkspaceModel* baseModel;
    AudioToken *editingToken;
    AudioTrack *dialogTrack;

public slots:
    void onSliceButtonClicked();
};

#endif // AUDIOTRACKDIALOG_H
