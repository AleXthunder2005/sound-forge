#include "mainwindow.h"
#include "audiofilelist.h"
#include "audiotrackframe.h"
#include "controlpanel.h"
#include "projectconfiguration.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setStyleSheet("background-color: " + ProjectConfiguration::clBackground.name() + ";");
    this->setMinimumSize(MIN_MAIN_WINDOW_WIDTH, MIN_MAIN_WINDOW_HEIGHT);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    QVBoxLayout *sidePanelLayout = new QVBoxLayout();
    QVBoxLayout *workspaceLayout = new QVBoxLayout();

    // Добавляем горизонтальный layout в основной вертикальный layout
    mainLayout->addLayout(sidePanelLayout);
    mainLayout->addLayout(workspaceLayout);

    AudioFileLinker *linker = new AudioFileLinker();
    // Боковая панель для списка аудиофайлов
    AudioFileList *audioList = new AudioFileList(this, linker);
    audioList->setFixedWidth(this->width() * DEFAULT_AUDIO_LIST_RELATIVE_WIDTH);
    sidePanelLayout->addWidget(audioList);



    // Оборачиваем trackFrame в QScrollArea
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea-> setStyleSheet("QFrame { background-color: " + ProjectConfiguration::clSidePanel.name() + "; color:  " + ProjectConfiguration::clSidePanelText.name() + "; }"
                             + "QScrollBar:vertical { border: none; background:  " + ProjectConfiguration::clScrollbarBackround.name() + "; width: " + QString::number(DEFAULT_SCROLLBAR_SIZE) + "px; }"
                             + "QScrollBar::handle:vertical { background:  " + ProjectConfiguration::clScrollbar.name() + ";}"
                             + "QScrollBar:horizontal { border: none; background:  " + ProjectConfiguration::clScrollbarBackround.name() + "; height: " + QString::number(DEFAULT_SCROLLBAR_SIZE) + "px; }"
                             + "QScrollBar::handle:horizontal { background:  " + ProjectConfiguration::clScrollbar.name() + ";}");

    // Фрейм для отображения аудиодорожек
    AudioTrackFrame *trackFrame = new AudioTrackFrame(this, scrollArea, linker);

    // Устанавливаем размеры trackFrame вручную, чтобы учитывать длину дорожек
    trackFrame->setMinimumWidth(1000);
    trackFrame->setMinimumHeight(400);

    scrollArea->setWidget(trackFrame);
    scrollArea->setWidgetResizable(true); // Позволяет динамически изменять размеры

    workspaceLayout->addWidget(scrollArea);

    WorkspaceModel *defaultModel = new WorkspaceModel();

    // Создаем аудиодорожку и добавляем токены
    AudioTrack *track1 = new AudioTrack();
    //track1->addToken(AudioToken(1, 0.0, 50.0, 0));
    //track1->addToken(AudioToken(2, 60.0, 40.0, 0));

    defaultModel->addTrack(track1);
    trackFrame->setModel(defaultModel);

    trackFrame->show();


    // Панель управления внизу
    ControlPanel *controlPanel = new ControlPanel(trackFrame, this);
    controlPanel->setFixedHeight(this->height() * DEFAULT_CONTROL_PANEL_RELATIVE_HEIGHT);
    workspaceLayout->addWidget(controlPanel);

    // Настройка меню
    QMenuBar *menuBar = this->menuBar();

    QMenu *fileMenu = menuBar->addMenu("File");
    //fileMenu->addAction("Open", this, &MainWindow::openFile);
    //fileMenu->addAction("Exit", this, &QApplication::quit);

    menuBar->addMenu("Settings");
    menuBar->addMenu("About");
    menuBar->addMenu("Help");

    menuBar->setStyleSheet("QMenuBar { background-color: " + ProjectConfiguration::clSidePanel.name() + "; color: " + ProjectConfiguration::clSidePanelText.name() + "; }" +
                           "QMenuBar::item:selected { background-color: " + ProjectConfiguration::clMenuBarItemSelected.name() + "; }" +
                           "QMenuBar::item:pressed { background-color: " + ProjectConfiguration::clMenuBarItemSelected.name() + "; }");
    // Установка размера окна
    this->setWindowState(Qt::WindowMaximized);

    // Установка растягивания для trackFrame и fileListView
    trackFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    audioList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    //connect(controlPanel, &ControlPanel::trackAdded, trackFrame, &AudioTrackFrame::update);
    connect(controlPanel, &ControlPanel::trackAdded, trackFrame, &AudioTrackFrame::onTrackAdded);
    connect(controlPanel, &ControlPanel::playClicked, trackFrame, &AudioTrackFrame::onPlayClicked);
    connect(controlPanel, &ControlPanel::pauseClicked, trackFrame, &AudioTrackFrame::onPauseClicked);

}
