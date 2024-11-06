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
};

#endif // CONTROLPANEL_H
