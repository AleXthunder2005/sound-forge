#ifndef WORKSPACEMODEL_H
#define WORKSPACEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include "audiotrack.h"

class WorkspaceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit WorkspaceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addTrack(AudioTrack *track);
    void moveToken(int oldTrackIndex, int newTrackIndex, int tokenIndex, double newStartPosition);



    QList<AudioTrack*> &getTracks();
    QList<AudioTrack*> tracks;

};

#endif // WORKSPACEMODEL_H
