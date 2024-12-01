#include "audiofilelist.h"
#include "projectconfiguration.h"
#include "audiofilelinker.h"

#include <QHeaderView>
#include <QFileInfo>
#include <QMimeData>
#include <QDrag>
#include <QStandardItemModel>
#include <QDir>

AudioFileList::AudioFileList(QWidget *parent) : QListView(parent) {
    model = new QStandardItemModel(this);
    setModel(model);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    // Установка стилей для выделения объектов
    setStyleSheet("QListView { background-color: " + ProjectConfiguration::clSidePanel.name() + "; color:  " + ProjectConfiguration::clSidePanelText.name() + "; }"
                  + "QListView::item { height: " + QString::number(DEFAULT_LIST_ITEM_HEIGHT) + "px; }" // Увеличение высоты элементов
                  + "QListView::item:selected { background-color: " + ProjectConfiguration::clSidePanelItemSelected.name() + "; }"
                  + "QScrollBar:vertical { border: none; background:  " + ProjectConfiguration::clScrollbarBackround.name() + "; width: " + QString::number(DEFAULT_SCROLLBAR_SIZE) + "px; }"
                  + "QScrollBar::handle:vertical { background:  " + ProjectConfiguration::clScrollbar.name() + ";}"
                  + "QScrollBar:horizontal { border: none; background:  " + ProjectConfiguration::clScrollbarBackround.name() + "; height: " + QString::number(DEFAULT_SCROLLBAR_SIZE) + "px; }"
                  + "QScrollBar::handle:horizontal { background:  " + ProjectConfiguration::clScrollbar.name() + ";}");

    audioFileLinker = new AudioFileLinker();
    loadAudioFiles(AUDIO_RESOURCES_PATH);
}

void AudioFileList::loadAudioFiles(const QString &directoryPath) {
    QDir dir(directoryPath);
    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.ogg";
    QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files);

    model->clear();  // Очистка модели перед загрузкой новых файлов
    for (const QFileInfo &fileInfo : fileInfoList) {
        QStandardItem *item = new QStandardItem(QIcon::fromTheme(MP3_ICON_PATH), fileInfo.fileName());

        audioFileLinker->appendAudioFile(fileInfo.absoluteFilePath());
        int currID = audioFileLinker->getLastID();
        item->setData(currID, Qt::UserRole);

        model->appendRow(item);
    }
}
void AudioFileList::startDrag(Qt::DropActions supportedActions) {
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        QMimeData *mimeData = new QMimeData;
        // Получаем уникальный ID из данных модели
        int uniqueID = index.data(Qt::UserRole).toInt();
        // Устанавливаем ID в MimeData
        mimeData->setData("application/x-audiofile", QByteArray::number(uniqueID));
        // Создаем QDrag и устанавливаем MimeData
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(MP3_ICON_PATH)); // Добавляем иконку для визуализации
        drag->exec(supportedActions);
    }
}

void AudioFileList::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        event->acceptProposedAction();
    }
}

void AudioFileList::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        event->acceptProposedAction();
    }
}

void AudioFileList::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        QModelIndex targetIndex = indexAt(event->position().toPoint());
        QModelIndex sourceIndex = currentIndex();

        if (sourceIndex.isValid()) {
            if (model) {
                QStandardItem *item = model->takeItem(sourceIndex.row());
                if (item) {
                    // Вставляем элемент на новую позицию
                    if (targetIndex.isValid()) {
                        model->insertRow(targetIndex.row(), item);
                    } else {
                        model->appendRow(item);
                    }
                }
            }
        }
        removeEmptyRows();
        event->acceptProposedAction();
    }
}

void AudioFileList::removeEmptyRows() {
    if (model) {
        for (int row = model->rowCount() - 1; row >= 0; --row) {
            if (model->item(row) == nullptr) {
                model->removeRow(row);
            }
        }
    }
}
