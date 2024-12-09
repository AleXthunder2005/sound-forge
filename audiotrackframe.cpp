#include "audiotrackframe.h"
#include "projectconfiguration.h"
#include "workspace-implementation/workspacemodel.h"
#include "audiofilelinker.h"
#include "controlpanel.h"
#include "audiotrackdialog.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QScrollBar>
#include <QMimeData>
#include <QDrag>
#include <QTimer>
#include <QDateTime>
#include <QAudioOutput>
#include <QBuffer>
#include <QMenu>
#include <QMenuBar>
#include <QDialog>

AudioTrackFrame::AudioTrackFrame(QWidget *parent, QScrollArea* scrollArea, AudioFileLinker *linker)
    : QFrame(parent),
    draggedToken(nullptr),
    isTokenDragging(false),
    isCurrTimeChanging(false),
    draggedTokenStartX(0),
    draggedTokenStartY(0),
    draggedTokenIndex(-1),
    currTime(0),
    currViewTime(0),
    startViewTime(0),
    trackTactCount(DEFAULT_TACT_COUNT),
    tactDuration(DEFAULT_TACT_DURATION),
    parentScrollArea(scrollArea),
    fileLinker(linker),
    scaleFactor(1.0),
    model(nullptr)
{
    setAcceptDrops(true);
    setMouseTracking(true);

    timer = new QTimer(this);
    totalTrack = new AudioTrack();

    connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &AudioTrackFrame::onVerticalScrollBarChanged);
    connect(this, &AudioTrackFrame::currTimeChanged, this, &AudioTrackFrame::onCurrTimeChanged);
    connect(this, &AudioTrackFrame::timeBarClicked, this, &AudioTrackFrame::onPauseClicked);
    connect(this, &AudioTrackFrame::trackChanged, this, &AudioTrackFrame::onTrackChanged);
    connect(timer, &QTimer::timeout, this, &AudioTrackFrame::updateCurrTime);
}

void AudioTrackFrame::setModel(WorkspaceModel *model) {
    this->model = model;
    resizeToFitContent();
    update();
}

WorkspaceModel* AudioTrackFrame::getModel() {
    return model;
}

void AudioTrackFrame::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);
    if (!model) return;

    QPainter painter(this);
    int trackWidth = trackTactCount * tactDuration * scaleFactor; // Масштабируем ширину трека

    // Рисуем дорожки
    for (int i = 0; i < model->rowCount(); i++) {
        AudioTrack *track = model->tracks[i];

        // Фон дорожки
        painter.setBrush((track->isSelected) ? ProjectConfiguration::clSelectedAudioTrack : ProjectConfiguration::clAudioTrack);
        painter.drawRect(0, TIME_BAR_HEIGHT + i * TRACK_HEIGHT, trackWidth, TRACK_HEIGHT);
    }

    // Рисуем сетку для дорожек (каждые 1/4 такта)
    drawTrackGrid(painter, model->rowCount());

    // Рисуем токены на дорожке
    for (int i = 0; i < model->rowCount(); i++) {
        AudioTrack *track = model->tracks[i];

        if (isTokenDragging && draggedToken->audioTrack == i) {
            for (int j = 0; j < track->tokens.size(); j++) {
                if (draggedTokenIndex != j) {
                    AudioToken token = track->tokens.at(j);
                    token.drawToken(&painter, scaleFactor, TRACK_HEIGHT); // Передаем масштаб
                }
            }
        } else {
            for (AudioToken token : track->tokens) {
                token.drawToken(&painter, scaleFactor, TRACK_HEIGHT); // Передаем масштаб
            }
        }

        // Рисуем перетаскиваемый токен
        if (isTokenDragging) {
            painter.setBrush(Qt::red);
            int x = draggedTokenStartX;
            int targetTrack = (draggedTokenStartY) / TRACK_HEIGHT;
            if (targetTrack >= 0 && targetTrack < model->rowCount() && x >= 0) {
                int headerHeight = TRACK_HEIGHT * TOKEN_HEADER_RELATIVE_HEIGHT;
                int mainContentHeight = TRACK_HEIGHT * (1 - TOKEN_HEADER_RELATIVE_HEIGHT);

                // Верхняя часть токена с текстом
                painter.setPen(ProjectConfiguration::clTokenText);
                painter.setBrush(DARK_DRAGGED_TOKEN_COLOR);
                painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT, draggedToken->relativeDurationView * scaleFactor, headerHeight);

                // Нижняя часть для будущего отображения аудиосигнала
                painter.setBrush(LIGHT_DRAGGED_TOKEN_COLOR);
                painter.drawRect(x, TIME_BAR_HEIGHT + targetTrack * TRACK_HEIGHT + headerHeight, draggedToken->relativeDurationView * scaleFactor, mainContentHeight);
            }
        }
    }

    // Рисуем временную шкалу
    drawTimeBar(painter, trackWidth);

    // Рисуем текущую позицию проигрывания
    painter.setPen(Qt::red);
    painter.drawLine(currViewTime * scaleFactor, 0, currViewTime * scaleFactor, height()); // Учет масштаба
}

void AudioTrackFrame::drawTimeBar(QPainter &painter, int width) {
    painter.setBrush(ProjectConfiguration::clTimeBar);

    int scrollOffset = parentScrollArea->verticalScrollBar()->value();
    painter.drawRect(0, scrollOffset, width, TIME_BAR_HEIGHT);

    painter.setPen(ProjectConfiguration::clTimeBarMark);

    // Рисуем такты и их номера
    for (int i = 0; i < trackTactCount; i++) {
        int x = i * tactDuration * scaleFactor; // Масштабируем такты

        // Длинная черта для такта
        painter.drawLine(x, scrollOffset, x, TIME_BAR_HEIGHT + scrollOffset);

        // Номер такта
        painter.drawText(x + 5, TIME_BAR_HEIGHT * 7 / 8 + scrollOffset, QString::number(i));

        // Короткие черточки для четвертей
        for (int j = 1; j < 4; j++) {
            int quarterX = x + j * (tactDuration / 4) * scaleFactor;
            painter.drawLine(quarterX, scrollOffset, quarterX, GRID_LINE_HEIGHT + scrollOffset);
        }
    }
}

void AudioTrackFrame::drawTrackGrid(QPainter &painter, int trackCount) {
    for (int i = 0; i < trackCount; i++) {
        int y = TIME_BAR_HEIGHT + i * TRACK_HEIGHT;

        for (int j = 0; j < trackTactCount; j++) {
            int x = j * tactDuration * scaleFactor; // Масштабируем такты

            // Линии такта
            painter.setPen(ProjectConfiguration::clAudioTrackBoldMark);
            painter.drawLine(x, y, x, y + TRACK_HEIGHT);

            // Линии четвертей такта
            for (int k = 1; k < 4; k++) {
                int quarterX = x + k * (tactDuration / 4) * scaleFactor;
                painter.setPen(ProjectConfiguration::clAudioTrackMark);
                painter.drawLine(quarterX, y, quarterX, y + TRACK_HEIGHT);
            }
        }
    }
}

void AudioTrackFrame::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint mousePos = event->pos();

        // Клик по таймбару
        int scrollOffset = parentScrollArea->verticalScrollBar()->value();
        if (mousePos.y() > scrollOffset && mousePos.y() < scrollOffset + TIME_BAR_HEIGHT) {
            currViewTime = mousePos.x() / scaleFactor;      // currTime при 1.0
            emit currTimeChanged();
            emit timeBarClicked();
            isCurrTimeChanging = true;
            update();
            return;
        }

        int xClick = mousePos.x();
        int yClick = mousePos.y() - TIME_BAR_HEIGHT;

        int trackIndex = yClick / TRACK_HEIGHT;
        if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

        AudioTrack *track = model->tracks[trackIndex];
        for (int tokenIndex = 0; tokenIndex < track->tokens.size(); ++tokenIndex) {
            AudioToken token = track->tokens[tokenIndex];
            double x = token.startPositionView * scaleFactor;
            double w = token.relativeDurationView * scaleFactor;

            if (xClick > x && xClick < (x + w)) {
                isTokenDragging = true;
                draggedToken = new AudioToken(token);
                draggedTokenDeltaX = xClick - token.startPositionView * scaleFactor;   // startPosition при 1.0
                draggedTokenDeltaY = yClick - token.audioTrack * TRACK_HEIGHT;

                draggedTokenStartX = xClick - draggedTokenDeltaX;    // startX и deltaX в больших координатах
                draggedTokenStartY = yClick - draggedTokenDeltaY;
                draggedTokenIndex = tokenIndex;
                break;
            }
        }
    } else if (event->button() == Qt::RightButton) {
        QPoint mousePos = event->pos();
        int yClick = mousePos.y() - TIME_BAR_HEIGHT;
        int trackIndex = yClick / TRACK_HEIGHT;

        if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

        AudioTrack *track = model->tracks[trackIndex];

        // Клик по токену
        for (int tokenIndex = 0; tokenIndex < track->tokens.size(); ++tokenIndex) {
            AudioToken token = track->tokens[tokenIndex];
            double x = token.startPositionView * scaleFactor;
            double w = token.relativeDurationView * scaleFactor;

            if (mousePos.x() > x && mousePos.x() < (x + w)) {
                showTokenContextMenu(mousePos, trackIndex, tokenIndex);
                return;
            }
        }

        // Клик по дорожке (не по токену)
        showTrackContextMenu(mousePos, trackIndex);
    }
}

void AudioTrackFrame::mouseMoveEvent(QMouseEvent *event) {
    QPoint mousePos = event->pos();

    if (isCurrTimeChanging) {
        if (mousePos.x() > 0 && mousePos.x() < trackTactCount * tactDuration * scaleFactor) {
            currViewTime = mousePos.x() / scaleFactor; // Учет масштаба                                //currTime при 1.0
            emit currTimeChanged();
            update();
            return;
        }
    } else if (isTokenDragging) {
        draggedTokenStartX = (mousePos.x() - draggedTokenDeltaX);
        draggedTokenStartY = mousePos.y() - TIME_BAR_HEIGHT;

        // Проверки и корректировки
        if (draggedTokenStartX < 0) draggedTokenStartX = 0;
        if (draggedTokenStartY < 0) draggedTokenStartY = 0;

        int maxStartX = (trackTactCount * tactDuration - draggedToken->relativeDurationView) * scaleFactor; //maxStartX в больших
        if (draggedTokenStartX > maxStartX)
            draggedTokenStartX = maxStartX;

        int maxStartY = (model->rowCount() - 1) * TRACK_HEIGHT;
        if (draggedTokenStartY > maxStartY)
            draggedTokenStartY = maxStartY;

        // Привязка к четвертям такта
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = (tactDuration * scaleFactor / 4);
            draggedTokenStartX = round(draggedTokenStartX / quarterDuration) * quarterDuration;
        }

        update();
    }
}

void AudioTrackFrame::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (isTokenDragging) {
            isTokenDragging = false;

            int dropX = draggedTokenStartX;
            if (!(event->modifiers() & Qt::ShiftModifier)) {
                double quarterDuration = tactDuration * scaleFactor / 4;
                dropX = round(draggedTokenStartX / quarterDuration) * quarterDuration;  //dropX в больших координатах
            }

            int dropTrackIndex = (draggedTokenStartY) / TRACK_HEIGHT;
            int oldTrackIndex = draggedToken->audioTrack;

            model->moveToken(oldTrackIndex, dropTrackIndex, draggedTokenIndex, dropX / scaleFactor);
            emit trackChanged(oldTrackIndex);
            emit trackChanged(dropTrackIndex);


            delete draggedToken;
            draggedToken = nullptr;
        }
        else if (isCurrTimeChanging) isCurrTimeChanging = false;

        update();
    }
}

void AudioTrackFrame::showTrackContextMenu(const QPoint &pos, int trackIndex) {
    QMenu contextMenu(this);

    QAction *deleteAction = contextMenu.addAction("Delete audiotrack");
    connect(deleteAction, &QAction::triggered, this, [=]() {
        deleteTrack(trackIndex);
    });

    // Устанавливаем флаг выбранной дорожки
    AudioTrack *currTrack = model->tracks[trackIndex];
    currTrack->isSelected = true;

    // Сбрасываем флаг, когда меню закрывается
    connect(&contextMenu, &QMenu::aboutToHide, this, [=]() {
        currTrack->isSelected = false;
    });

    contextMenu.exec(mapToGlobal(pos));
}

void AudioTrackFrame::showTokenContextMenu(const QPoint &pos, int trackIndex, int tokenIndex) {
    QMenu contextMenu(this);

    AudioToken *currToken = &model->tracks[trackIndex]->tokens[tokenIndex];
    currToken->setSelectedFlag();

    // Сбрасываем флаг, когда меню закрывается
    connect(&contextMenu, &QMenu::aboutToHide, this, [currToken]() {
        currToken->resetSelectedFlag();
    });

    QAction *deleteAction = contextMenu.addAction("Delete token");
    connect(deleteAction, &QAction::triggered, this, [=]() {
        deleteToken(trackIndex, tokenIndex);
    });

    QAction *editAction = contextMenu.addAction("Edit token");
    connect(editAction, &QAction::triggered, this, [=]() {
        openEditTokenWindow(trackIndex, tokenIndex);
    });

    contextMenu.exec(mapToGlobal(pos));
}

void AudioTrackFrame::deleteTrack(int trackIndex) {
    if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

    delete model->tracks[trackIndex];
    model->tracks.removeAt(trackIndex);

    update(); // Перерисовать интерфейс
}

void AudioTrackFrame::deleteToken(int trackIndex, int tokenIndex) {
    if (trackIndex < 0 || trackIndex >= model->rowCount()) return;

    AudioTrack *track = model->tracks[trackIndex];
    if (tokenIndex < 0 || tokenIndex >= track->tokens.size()) return;

    track->tokens.removeAt(tokenIndex);
    track->isTrackChanged = true;

    update(); // Перерисовать интерфейс
}

void AudioTrackFrame::resizeToFitContent() {
    if (!model) return;

    // Рассчитываем ширину и высоту на основе количества тактов и дорожек
    int width = trackTactCount * tactDuration * scaleFactor;
    int height = model->rowCount() * TRACK_HEIGHT;

    // Устанавливаем минимальные размеры для trackFrame
    setMinimumWidth(width);
    setMinimumHeight(height);

    // Обновляем виджет
    updateGeometry();
    update();
}

void AudioTrackFrame::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
        scaleFactor *= factor;
        scaleFactor = qBound(MIN_SCALE, scaleFactor, MAX_SCALE); // Ограничиваем масштаб
        resizeToFitContent();
        update();
    } else {
        QFrame::wheelEvent(event);
    }
}

//drag and drop из списка:
void AudioTrackFrame::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) { // Проверяем MIME-тип
        event->acceptProposedAction();
        isTokenDragging = true;

        QPoint mousePos = event->position().toPoint();
        int xClick = mousePos.x();
        int yClick = mousePos.y() - TIME_BAR_HEIGHT;

        int dropTrack = yClick / TRACK_HEIGHT;

        int uniqueAudiofileID = QString::fromUtf8(event->mimeData()->data("application/x-audiofile")).toInt();

        draggedToken = new AudioToken(uniqueAudiofileID, xClick / scaleFactor, AudioFileLinker::calculateDuration(uniqueAudiofileID), dropTrack);

        draggedTokenDeltaX = 0;   //startPosition при 1.0
        draggedTokenDeltaY = TRACK_HEIGHT / 2;

        draggedTokenStartX = xClick - draggedTokenDeltaX;    //startX и deltaX в больших координатах
        draggedTokenStartY = yClick - draggedTokenDeltaY;
        draggedTokenIndex = -1;
    }
}

void AudioTrackFrame::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        QPoint mousePos = event->position().toPoint();
        draggedTokenStartX = mousePos.x();
        draggedTokenStartY = mousePos.y() - TIME_BAR_HEIGHT;

        // Ограничиваем перетаскиваемые координаты
        if (draggedTokenStartX < 0) draggedTokenStartX = 0;
        if (draggedTokenStartY < 0) draggedTokenStartY = 0;

        int maxStartX = trackTactCount * tactDuration * scaleFactor;
        if (draggedTokenStartX > maxStartX)
            draggedTokenStartX = maxStartX;

        int maxStartY = (model->rowCount() - 1) * TRACK_HEIGHT;
        if (draggedTokenStartY > maxStartY)
            draggedTokenStartY = maxStartY;

        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = (tactDuration * scaleFactor / 4);
            draggedTokenStartX = round(draggedTokenStartX / quarterDuration) * quarterDuration;
        }

        update(); // Перерисовываем временный токен
        event->acceptProposedAction();
    }
}

void AudioTrackFrame::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-audiofile")) {
        int dropX = draggedTokenStartX;
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = tactDuration * scaleFactor / 4;
            dropX = round(draggedTokenStartX / quarterDuration) * quarterDuration;  //dropX в больших координатах
        }
        int dropTrack = (draggedTokenStartY) / TRACK_HEIGHT;

        int uniqueAudiofileID = QString::fromUtf8(event->mimeData()->data("application/x-audiofile")).toInt();

        // Привязка к четвертям такта
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            double quarterDuration = tactDuration * scaleFactor / 4;
            dropX = round(dropX / quarterDuration) * quarterDuration;
        }

        AudioToken *newToken = new AudioToken(uniqueAudiofileID, dropX / scaleFactor, AudioFileLinker::calculateDuration(uniqueAudiofileID), dropTrack);
        AudioTrack *track = model->tracks[dropTrack];
        track->addToken(*newToken);
        emit trackChanged(dropTrack);

        delete(draggedToken);
        draggedToken = nullptr;
        isTokenDragging = false;

        event->acceptProposedAction();
        update();
    }
}

void AudioTrackFrame::onTrackAdded() {
    resizeToFitContent(); // Пересчитать размеры
    update();             // Перерисовать содержимое
}

void AudioTrackFrame::onPlayClicked() {
    AudioTrack *currTrack;
    QByteArray res;

    totalTrack = model->tracks[0];
    totalTrack->processAudioTrack();
    for (int i = 1; i < model->tracks.size(); i++) {
        currTrack = model->tracks[i];
        if (currTrack->tokens.isEmpty()) continue;

        if (currTrack->isTrackChanged)
            currTrack->processAudioTrack();

        res = mixWavFiles(totalTrack->trackData, currTrack->trackData);
        totalTrack->trackData = &res;
    }
    currTime = currViewTime * MS_TO_PX;
    totalTrack->playTrack(currTime);

    connect(totalTrack, &AudioTrack::audioFileFinished, this, &AudioTrackFrame::onAudioFileFinished);
    update();

    startTime = QDateTime::currentMSecsSinceEpoch(); // запоминаем время начала
    startViewTime = currViewTime;
    timer->start(1); // срабатывает каждые 1 мс
}

void AudioTrackFrame::onPauseClicked() {
    if (timer && (timer->isActive())) {
        timer->stop();
        startViewTime = currViewTime;

        for (AudioTrack *track: model->tracks) {
            track->pauseTrack();
        }
        totalTrack->pauseTrack();
    }
    update();
}

void AudioTrackFrame::updateCurrTime() {
    int elapsedTime = QDateTime::currentMSecsSinceEpoch() - startTime; // прошедшее время
    currTime += elapsedTime;
    currViewTime = round((startViewTime * MS_TO_PX + elapsedTime) / MS_TO_PX);
    update(); // перерисовываем содержимое
}

void AudioTrackFrame::onVerticalScrollBarChanged() {
    update();
}

void AudioTrackFrame::onCurrTimeChanged() {
    startViewTime = currViewTime;
    currTime = currViewTime * MS_TO_PX;
    startTime = QDateTime::currentMSecsSinceEpoch();
}

void AudioTrackFrame::onTrackChanged(int index){
    model->tracks[index]->setTrackChangeFlag();
}

QByteArray AudioTrackFrame::mixWavFiles(QByteArray *wav1, QByteArray *wav2) {
    // Проверяем, что оба файла имеют минимум заголовок
    if (wav1->size() < 44 || wav2->size() < 44) {
        qWarning() << "Invalid WAV files: too small!";
        return QByteArray();
    }

    // Извлекаем заголовки
    QByteArray header1 = wav1->left(44);
    QByteArray header2 = wav2->left(44);

    // Проверяем, что параметры WAV-файлов совпадают
    if (header1.mid(22, 2) != header2.mid(22, 2) ||  // Количество каналов
        header1.mid(24, 4) != header2.mid(24, 4) ||  // Частота дискретизации
        header1.mid(34, 2) != header2.mid(34, 2)) {  // Битность
        qWarning() << "WAV files have different formats!";
        return QByteArray();
    }

    // Извлекаем параметры WAV
    int16_t numChannels = qFromLittleEndian<int16_t>(reinterpret_cast<const uchar *>(header1.data() + 22));
    int32_t sampleRate = qFromLittleEndian<int32_t>(reinterpret_cast<const uchar *>(header1.data() + 24));
    int16_t bitsPerSample = qFromLittleEndian<int16_t>(reinterpret_cast<const uchar *>(header1.data() + 34));
    int16_t bytesPerSample = bitsPerSample / 8;

    if (bitsPerSample != 16) {
        qWarning() << "Only 16-bit WAV files are supported!";
        return QByteArray();
    }

    // Извлекаем аудиоданные
    QByteArray data1 = wav1->mid(44);
    QByteArray data2 = wav2->mid(44);

    // Определяем длину аудиоданных
    int maxLength = qMax(data1.size(), data2.size());

    // Дополняем меньший файл нулями до длины большего
    if (data1.size() < maxLength) {
        data1.append(QByteArray(maxLength - data1.size(), 0));
    }
    if (data2.size() < maxLength) {
        data2.append(QByteArray(maxLength - data2.size(), 0));
    }

    // Смешиваем аудиоданные
    QByteArray mixedData;
    mixedData.resize(maxLength);

    for (int i = 0; i < maxLength; i += bytesPerSample) {
        // Берем сэмплы как 16-битные значения
        int16_t sample1 = qFromLittleEndian<int16_t>(reinterpret_cast<const uchar *>(data1.data() + i));
        int16_t sample2 = qFromLittleEndian<int16_t>(reinterpret_cast<const uchar *>(data2.data() + i));

        // Смешиваем сэмплы
        int32_t mixedSample = sample1 + sample2;

        // Ограничиваем диапазон значения
        if (mixedSample > INT16_MAX) mixedSample = INT16_MAX;
        if (mixedSample < INT16_MIN) mixedSample = INT16_MIN;

        // Добавляем смешанный сэмпл в результирующий массив
        qToLittleEndian<int16_t>(static_cast<int16_t>(mixedSample), reinterpret_cast<uchar *>(mixedData.data() + i));
    }

    // Создаем новый заголовок WAV
    QByteArray newHeader = header1;
    int32_t newDataSize = mixedData.size();
    int32_t newFileSize = 36 + newDataSize;

    // Обновляем размер файла в заголовке
    qToLittleEndian<int32_t>(newFileSize, reinterpret_cast<uchar *>(newHeader.data() + 4));
    // Обновляем размер аудиоданных в заголовке
    qToLittleEndian<int32_t>(newDataSize, reinterpret_cast<uchar *>(newHeader.data() + 40));

    // Соединяем заголовок и данные
    QByteArray result = newHeader + mixedData;
    return result;
}

void AudioTrackFrame::onAudioFileFinished() {
    if (!totalTrack->tokens.isEmpty()) {
        timer->stop();
        currTime = 0;
        currViewTime = 0;
        startTime = QDateTime::currentMSecsSinceEpoch(); // запоминаем время начала
        startViewTime = currViewTime;
        timer->start(1); // срабатывает каждые 1 мс
        update();
    }
}

void AudioTrackFrame::openEditTokenWindow(int trackIndex, int tokenIndex) {
    AudioTrackDialog dialog(trackIndex, tokenIndex, fileLinker, this);
    if (dialog.exec() == QDialog::Accepted) {
        //QList<AudioToken> updatedTokens = dialog.getTokens();
        // Обновляем модель с новыми токенами
        //model->tracks[trackIndex]->tokens = updatedTokens;
        update();
    }
}
