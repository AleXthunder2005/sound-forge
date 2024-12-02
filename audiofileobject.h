#ifndef AUDIOFILEOBJECT_H
#define AUDIOFILEOBJECT_H

#include <QString>
#include <QByteArray>
#include <QAudioFormat>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QObject>

class AudioFileObject : public QObject {
    Q_OBJECT

public:
    // Конструкторы
    explicit AudioFileObject(QObject *parent = nullptr);
    AudioFileObject(const QString &filePath, QObject *parent = nullptr);

    // Методы
    bool loadAudioFile(const QString &filePath); // Загрузка аудио
    void analyzeAudio();                         // Анализ аудиофайла
    void printAudioInfo() const;                 // Вывод информации об аудио

    // Поля
    bool canAccess;                  // Флаг, доступен ли файл
    bool isEmpty;                    // Флаг, пустой ли объект
    QString audioFilePath;           // Путь к аудиофайлу
    QByteArray audioData;            // Указатель на данные аудиофайла
    qint64 durationMs;               // Длительность аудио в миллисекундах
    int sampleRate;                  // Частота дискретизации (Гц)
    int bitRate;                     // Битрейт (бит/с)
    int channels;                    // Количество каналов (моно/стерео)

private:
    QMediaPlayer mediaPlayer;

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
};

#endif // AUDIOFILEOBJECT_H
