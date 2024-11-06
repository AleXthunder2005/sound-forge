#ifndef AUDIOFILELIST_H
#define AUDIOFILELIST_H

#include <QListView>
#include <QStandardItemModel>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class AudioFileList : public QListView {
    Q_OBJECT

public:
    AudioFileList(QWidget *parent = nullptr);
   // void loadAudioFiles(const QString &directoryPath);

protected:
   //void dragEnterEvent(QDragEnterEvent *event) override;
   // void dropEvent(QDropEvent *event) override;

private:
    QStandardItemModel *model;
};

#endif // AUDIOFILELIST_H
