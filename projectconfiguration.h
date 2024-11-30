#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include <QColor>
#include <QString>

//MainWindow
const int MIN_MAIN_WINDOW_WIDTH = 600;
const int MIN_MAIN_WINDOW_HEIGHT = 400;

//AudioList
const float DEFAULT_AUDIO_LIST_RELATIVE_WIDTH = 0.3;
const int DEFAULT_LIST_ITEM_HEIGHT = 25;
const int DEFAULT_SCROLLBAR_SIZE = 10;
const QString MP3_ICON_PATH = "F:/Projects/SoundForge/icons/mp3-icon.ico";
const QString AUDIO_RESOURCES_PATH = "F:/Projects/SoundForge/AudioFiles";

//AudioTrackFrame
const int TRACK_HEIGHT = 50;
const int DELTA = 2;
const int DEFAULT_TACT_DURATION = 50;
const int DEFAULT_TACT_COUNT = 100;


//ControlPanel
const float DEFAULT_CONTROL_PANEL_RELATIVE_HEIGHT = 0.1;
const int ADD_TRACK_BUTTON_WIDTH = 100;

//Colors
const QColor DARK_COLOR = QColor::fromRgb(0x1e, 0x1e, 0x1e);
const QColor DARK_GRAY_COLOR = QColor::fromRgb(0x2b, 0x2b, 0x2b);
const QColor WHITE_COLOR = QColor::fromRgb(0xff, 0xff, 0xff);
const QColor BLACK_COLOR = QColor::fromRgb(0x0e, 0x19, 0x21);
const QColor BLUE_COLOR = QColor::fromRgb(0x1a, 0x57, 0x5b);
//const QColor GRAY_SCROLLBAR_COLOR = QColor::fromRgb(0xf0, 0xf0, 0xf0);
const QColor SCROLLBAR_BACKGROUND_COLOR = QColor::fromRgb(0xf0, 0xf0, 0xf0);

class ProjectConfiguration
{
public:
    ProjectConfiguration();

    static QColor clAudioTrackFrame;
    static QColor clSidePanel;
    static QColor clSidePanelText;
    static QColor clControlPanel;
    static QColor clSidePanelItemSelected;
    static QColor clBackground;
    static QColor clDarkButton;
    static QColor clDarkButtonSelected;
    static QColor clMenuBarItemSelected;
    static QColor clScrollbarBackround;
    static QColor clScrollbar;
};

#endif // PROJECTCONFIGURATION_H
