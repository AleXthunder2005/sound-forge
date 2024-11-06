#include "audiofilelist.h"
#include "projectconfiguration.h"

#include <QHeaderView>
#include <QFileInfo>
#include <QMimeData>

AudioFileList::AudioFileList(QWidget *parent) : QListView(parent), model(new QStandardItemModel(this)) {
    setModel(model);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);

    // Установка стилей для выделения объектов
    setStyleSheet("QListView { background-color: " + ProjectConfiguration::clSidePanel.name() + "; color:  " + ProjectConfiguration::clSidePanelText.name() + "; }"
                  "QListView::item:selected { background-color: " + ProjectConfiguration::clSidePanelSelectedItem.name() + "; }");

    // Загрузка аудиофайлов
    //loadAudioFiles("F:/Projects/SoundForge/AudioFiles");
}

// void AudioFileList::loadAudioFiles(const QString &directoryPath) {
//     QDir dir(directoryPath);
//     QStringList filters;
//     filters << "*.mp3" << "*.wav" << "*.ogg";  // Добавьте другие форматы, если нужно
//     QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files);

//     model->clear();  // Очистка модели перед загрузкой новых файлов
//     for (const QFileInfo &fileInfo : fileInfoList) {
//         QStandardItem *item = new QStandardItem(fileInfo.fileName());
//         item->setData(fileInfo.absoluteFilePath(), Qt::UserRole); // Сохраняем полный путь к файлу
//         model->appendRow(item);
//     }
// }

// void AudioFileList::dragEnterEvent(QDragEnterEvent *event) {
//     if (event->mimeData()->hasFormat("application/vnd.text.list")) {
//         event->acceptProposedAction();
//     }
// }

// void AudioFileList::dropEvent(QDropEvent *event) {
//     if (event->mimeData()->hasFormat("application/vnd.text.list")) {
//         const QMimeData *mimeData = event->mimeData();
//         QList<QUrl> urls = mimeData->urls();

//         if (!urls.isEmpty()) {
//             for (const QUrl &url : urls) {
//                 QString fileName = url.toLocalFile();
//                 QFileInfo fileInfo(fileName);
//                 if (fileInfo.exists() && fileInfo.isFile()) {
//                     QStandardItem *item = new QStandardItem(fileInfo.fileName());
//                     item->setData(fileInfo.absoluteFilePath(), Qt::UserRole);
//                     model->appendRow(item);
//                 }
//             }
//             event->acceptProposedAction();
//         }
//     }
// }
