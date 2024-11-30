#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>

class ControlPanel : public QFrame {
    Q_OBJECT

public:
    ControlPanel(QWidget *parent = nullptr);

private:
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *addTrackButton;

private slots:
    void onAddTrackButtonClicked(); // Slot for button click

signals:
    void trackAdded();
};

#endif // CONTROLPANEL_H
