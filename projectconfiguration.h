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

const int TIME_BAR_HEIGHT = 20;
const int GRID_LINE_HEIGHT = 4; // Высота короткой черты для четвертей такта

const double MIN_SCALE = 0.4;
const double MAX_SCALE = 10.0;

//AudioToken
const double TOKEN_HEADER_RELATIVE_HEIGHT = 0.35;

//ControlPanel
const float DEFAULT_CONTROL_PANEL_RELATIVE_HEIGHT = 0.1;
const int ADD_TRACK_BUTTON_WIDTH = 100;

//Colors
const QColor DARK_COLOR = QColor::fromRgb(0x1e, 0x1e, 0x1e);
const QColor DARK_GRAY_COLOR = QColor::fromRgb(0x2b, 0x2b, 0x2b);
const QColor WHITE_COLOR = QColor::fromRgb(0xff, 0xff, 0xff);
const QColor BLACK_COLOR = QColor::fromRgb(0x0e, 0x19, 0x21);
const QColor BLUE_COLOR = QColor::fromRgb(0x1a, 0x57, 0x5b);
const QColor SCROLLBAR_BACKGROUND_COLOR = QColor::fromRgb(0xf0, 0xf0, 0xf0);
const QColor LIGHT_GRAY_COLOR = QColor::fromRgb(0x3c, 0x49, 0x45);
const QColor LIGHT_BLUE_COLOR = QColor::fromRgb(0xac, 0xcf, 0xc7, 30);
const QColor LIGHT_BLUE_BOLD_COLOR = QColor::fromRgb(0xac, 0xcf, 0xc7, 120);

const QColor DARK_DRAGGED_TOKEN_COLOR = QColor::fromRgb(0xfd, 0x4c, 0x3c, 120);
const QColor LIGHT_DRAGGED_TOKEN_COLOR = QColor::fromRgb(0xf3, 0x7e, 0x63, 70);

const QColor DARK_ORANGE_TOKEN_COLOR = QColor::fromRgb(0xd8, 0x9b, 0x2b, 200);
const QColor LIGHT_ORANGE_TOKEN_COLOR = QColor::fromRgb(0xe8, 0xc2, 0x6f, 130);

//const QColor DARK_TEAL_TOKEN_COLOR = QColor::fromRgb(0x30, 0x6c, 0x6f, 200);
//const QColor LIGHT_TEAL_TOKEN_COLOR = QColor::fromRgb(0x7b, 0xbb, 0xab, 130);

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
    static QColor clTimeBar;
    static QColor clTimeBarMark;
    static QColor clAudioTrack;
    static QColor clAudioTrackMark;
    static QColor clAudioTrackBoldMark;
    static QColor clDefaultTokenHeader;
    static QColor clDefaultTokenMainContent;
    static QColor clTokenText;
};

#endif // PROJECTCONFIGURATION_H
