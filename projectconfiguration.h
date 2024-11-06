#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include <QColor>

//MainWindow
const int MIN_MAIN_WINDOW_WIDTH = 600;
const int MIN_MAIN_WINDOW_HEIGHT = 400;

//AudioList
const float DEFAULT_AUDIO_LIST_RELATIVE_WIDTH = 0.3;

//AudioTrackFrame

//ControlPanel
const float DEFAULT_CONTROL_PANEL_RELATIVE_HEIGHT = 0.1;

//Colors
const QColor DARK_COLOR = QColor::fromRgb(0x1e, 0x1e, 0x1e);
const QColor DARK_GRAY_COLOR = QColor::fromRgb(0x2b, 0x2b, 0x2b);
const QColor WHITE_COLOR = QColor::fromRgb(0xff, 0xff, 0xff);
const QColor BLACK_COLOR = QColor::fromRgb(0x0a, 0x09, 0x0d);
const QColor BLUE_COLOR = QColor::fromRgb(0x1a, 0x57, 0x5b);

class ProjectConfiguration
{
public:
    ProjectConfiguration();

    static QColor clAudioTrackFrame;
    static QColor clSidePanel;
    static QColor clSidePanelText;
    static QColor clControlPanel;
    static QColor clSidePanelSelectedItem;
    static QColor clBackground;
    static QColor clDarkButton;
    static QColor clDarkButtonSelected;
    static QColor clMenuBarItemSelected;
};

#endif // PROJECTCONFIGURATION_H
