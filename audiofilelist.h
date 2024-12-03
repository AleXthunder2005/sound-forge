#ifndef AUDIOFILELIST_H
#define AUDIOFILELIST_H

#include <QListView>
#include <QStandardItemModel>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "audiofilelinker.h"

class AudioFileList : public QListView {
    Q_OBJECT

public:
    AudioFileList(QWidget *parent = nullptr, AudioFileLinker *linker = nullptr);
    AudioFileLinker *audioFileLinker;
    void loadAudioFiles(const QString &directoryPath);

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;


private:
    QStandardItemModel *model;
    void removeEmptyRows();
};

#endif // AUDIOFILELIST_H
