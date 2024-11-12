#include "workspacemodel.h"

WorkspaceModel::WorkspaceModel(QObject *parent)
    : QAbstractListModel(parent) {}

int WorkspaceModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return tracks.size();
}

QVariant WorkspaceModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= tracks.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return QVariant("Audio Track " + QString::number(index.row()));
    }

    return QVariant();
}

void WorkspaceModel::addTrack(AudioTrack track) {
    beginInsertRows(QModelIndex(), tracks.size(), tracks.size());
    tracks.append(track);
    endInsertRows();
}

const QList<AudioTrack>& WorkspaceModel::getTracks() const {
    return tracks;
}


void WorkspaceModel::moveToken(int oldTrackIndex, int newTrackIndex, int tokenIndex, double newStartPosition) {
    if (oldTrackIndex < 0 || oldTrackIndex >= tracks.size() ||
        newTrackIndex < 0 || newTrackIndex >= tracks.size()) {
        return; // Неверные индексы
    }

    // Извлекаем токен из старой дорожки
    AudioToken token = tracks[oldTrackIndex].tokens.takeAt(tokenIndex);

    // Обновляем поля токена
    token.audioTrack = newTrackIndex;
    token.startPosition = newStartPosition;

    // Добавляем токен в новую дорожку
    tracks[newTrackIndex].addToken(token);

    // Уведомляем об изменениях
    emit dataChanged(index(oldTrackIndex), index(oldTrackIndex));
    emit dataChanged(index(newTrackIndex), index(newTrackIndex));
}
