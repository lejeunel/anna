#include "mainwindow.h"
#include "eyetribetracker.h"
#include "videoreader.h"
#include "volumereader.h"
#include <QtWidgets>
#include <QDebug>

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
MainWindow::MainWindow()
    : tracker(new EyeTribeTracker)
    , frameLabel(new FrameLabel)
    , annotationLabel(new AnnotationLabel(frameLabel))
    , timer(new QTimer(this))
    , objectVisible(false) {

    setStatusBar(new ColorfulStatusBar(this));

    frameLabel->setBackgroundRole(QPalette::Base);
    setCentralWidget(frameLabel);

    // order matters, if a file is loaded the first reader that support the format is chosen
    frameReaders.push_back(new VolumeReader(&annotationData));
    frameReaders.push_back(new VideoReader(&annotationData));
    currentFrameReader = frameReaders[0];

    for(auto frameReader : frameReaders) {
        connect(frameReader, &FrameReaderInterface::frameChanged, frameLabel, &FrameLabel::setImage);
        connect(frameReader, &FrameReaderInterface::frameChanged, this, &MainWindow::updatePosition);
        connect(frameReader, &FrameReaderInterface::nbFramesChanged, this, &MainWindow::updateDuration);
    }
    connect(tracker, &EyeTrackerInterface::newAnnotationData, this, &MainWindow::newTrackerData);
    connect(&annotationData, &AnnotationDataManager::modified, this, &MainWindow::isModified);

    setAcceptDrops(true);

    createActions();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    updateActions();
    connectTracker();
}

bool MainWindow::loadFile(const QString &fileName) {
    for(auto frameReader : frameReaders)
        if(frameReader->load(fileName)) {
            if(currentFrameReader != frameReader)
                switchCurrentFrameReader(frameReader);
            break;
        }
    if(!currentFrameReader->isOpened()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    QFileInfo fileInfo(fileName);
    qDebug() << "file name: " << fileInfo.baseName() << "; path: " << fileInfo.filePath();
    annotationData.deleteData();
    annotationData.addVideoData(fileInfo.fileName(), currentFrameReader->getFrameSize());

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message, 2000);

    //Set recording to true by default
    recording();
    updateActions();
    return true;
}


/*
 * ==============================================================================
 *                              private slots
 * ==============================================================================
 */
void MainWindow::newTrackerData(const TrackingData &data) {
//    qDebug() << "new tracker data: " << data.avgX << ", " << data.avgY;
    QPoint imagePosition = frameLabel->getFramePosition();
    QSize imageSize = frameLabel->getFrameSize();

    double x = (data.x-imagePosition.x())/(double)imageSize.width();
    double y = (data.y-imagePosition.y())/(double)imageSize.height();

    if(x<0 || y<0 || x>1 || y>1)
        return;

    ((ColorfulStatusBar*)statusBar())->setStyleSheet("background-color: rgb(37, 87, 146);", 40);

    CsvData data2save;

    data2save.frame = currentFrameReader->getCurrentFrameNumber();
    data2save.visible = objectVisible;

    data2save.timestamp = QDateTime::currentMSecsSinceEpoch();
    data2save.x = x;
    data2save.y = y;
    data2save.isFixated = data.isFixated;

    emit saveAnnotationData(data2save);

    QPoint pos = QPoint(data.x, data.y);
    if(visualizeAnnotationAct->isChecked()) {
        emit updateAnnotationPoint(frameLabel->mapFromGlobal(pos));
        annotationLabel->setVisible(true);
    }
    else
        annotationLabel->setVisible(false);
}

void MainWindow::newManualData(const QPoint &pos) {
    QPoint imagePosition = frameLabel->getFramePosition();
    QSize imageSize = frameLabel->getFrameSize();

    double x = (pos.x()-imagePosition.x())/(double)imageSize.width();
    double y = (pos.y()-imagePosition.y())/(double)imageSize.height();

    if(x<0 || y<0 || x>1 || y>1)
        return;

    ((ColorfulStatusBar*)statusBar())->setStyleSheet("background-color: rgb(37, 87, 146);", 40);

    CsvData data2save;

    data2save.frame = currentFrameReader->getCurrentFrameNumber();
    data2save.visible = true;

    data2save.timestamp = QDateTime::currentMSecsSinceEpoch();
    data2save.x = x;
    data2save.y = y;
    data2save.isFixated = true;

    emit saveAnnotationData(data2save);

    if(visualizeAnnotationAct->isChecked()) {
        emit updateAnnotationPoint(frameLabel->mapFromGlobal(QPoint(x, y)));
        annotationLabel->setVisible(true);
    }
    else
        annotationLabel->setVisible(false);
}

void MainWindow::onSpeedSliderValueChanged(double var){
    speedLabel->setText( QString("%1").arg(var));
}

void MainWindow::isModified() {
    saveAct->setEnabled(true);
    saveAsAct->setEnabled(true);
}

static void initializeVideoFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode,
                                      QString proposedFileName="") {
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList videosLocations = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
        dialog.setDirectory(videosLocations.isEmpty() ? QDir::currentPath() : videosLocations.last());
    }
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave) {
        dialog.selectFile(proposedFileName);
        dialog.setDefaultSuffix("csv");
    }
}

void MainWindow::open() {
    if(maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if(!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save() {
    QString currentFile = annotationData.getCurrentFile();
    if(currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveData(currentFile);
    }
}

bool MainWindow::saveAs() {
    QString proposedFileName = "annotationData_"+QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")+".csv";

    QFileDialog dialog(this, tr("Save File As"));
    initializeVideoFileDialog(dialog, QFileDialog::AcceptSave, proposedFileName);

    if(dialog.exec() != QDialog::Accepted || !saveData(dialog.selectedFiles().first()))
        return false;
    updateActions();
    return true;
}

void MainWindow::newFile() {
    if(maybeSave()) {
        annotationData.deleteData();
        if(currentFrameReader->isOpened())
            annotationData.addVideoData(currentFrameReader->getName(), currentFrameReader->getFrameSize());
    }
}

void MainWindow::visualizeAnnotation() {
    if(visualizeAnnotationAct->isChecked()){
        qDebug() << "visualizeAnnotationAct is checked!";
        annotationLabel->setVisible(true);
        connect(this, &MainWindow::updateAnnotationPoint, annotationLabel, &AnnotationLabel::put);
    }
    else {
        qDebug() << "visualizeAnnotationAct is not checked!";
        disconnect(this, 0, annotationLabel, 0);
        annotationLabel->setVisible(false);
    }
    updateActions();
}

void MainWindow::connectTracker() {
    tracker->connectTracker();
    if(tracker->isConnected())
        statusBar()->showMessage(tr("Eye tracker successfully connected"));
    else
        statusBar()->showMessage(tr("Failed to connect eye tracker"));
    updateActions();
}

void MainWindow::disconnectTracker() {
    tracker->disconnectTracker();
    if(!tracker->isConnected()) {
        statusBar()->showMessage(tr("Eye tracker disconnected"));
        annotationLabel->setVisible(false);
    }
    updateActions();
}

void MainWindow::recording() {
    if(recordingAct->isChecked()) {
        connect(this, &MainWindow::saveAnnotationData, &annotationData, &AnnotationDataManager::newData);
        connect(frameLabel, &FrameLabel::markedPosition, this, &MainWindow::newManualData);
        statusBar()->showMessage(tr("Recording"));
    }
    else {
        disconnect(this, 0, &annotationData, 0);
        disconnect(frameLabel, 0, this, 0);
        statusBar()->showMessage(tr("Not recording"));
    }
    updateActions();
}

void MainWindow::updatePosition(const QImage &, qint64 frameIndex) {
    positionSlider->setValue(frameIndex);
    positionLabel->setText(QStringLiteral("%1/%2").arg(frameIndex).arg(positionSlider->maximum()));
}

void MainWindow::updateDuration(qint64 duration) {
    qint64 maxFrameIdx = duration-1;
    positionSlider->setRange(0, maxFrameIdx);
    positionSlider->setEnabled(maxFrameIdx > 0);
    positionSlider->setSingleStep(1);
    positionSlider->setPageStep(ceil(maxFrameIdx/10));
    qDebug() <<"duration: " << maxFrameIdx;
}

void MainWindow::setPosition(int position) {
    // avoid seeking when the slider value change is triggered from updatePosition()
    if (currentFrameReader->getCurrentFrameNumber() != position) {
        currentFrameReader->setFrame(position);
    }
}

void MainWindow::play() {
    if(playAct->isChecked()) {
        if(reverseAct->isChecked())
            connect(timer, &QTimer::timeout, currentFrameReader, &FrameReaderInterface::previousFrame);
        else
            connect(timer, &QTimer::timeout, currentFrameReader, &FrameReaderInterface::nextFrame);
        timer->start(speedSlider->valueMS());
    }
    else {
        disconnect(timer, 0, currentFrameReader, 0);
        timer->stop();
    }
    updateActions();
}

void MainWindow::reverse() {
    if(playAct->isChecked()) {
        disconnect(timer, 0, currentFrameReader, 0);
        play();
    }
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About anna"),
                       QString("<p>anna 1.0</p>")+
                       "<p>Created on "+__DATE__+"</p>"+
                       "<p>ARTORG Center Ophthalmology Group, University of Bern</p>"+
                       "<p>No warranty; you can do with it whatever you want.</p>");
}

void MainWindow::rtfm() {
    QMessageBox info;
    info.setText(tr("<b>Brief Manual</b>"));
    info.setInformativeText(tr("<p><b>Introduction: </b>"
                               "This is a brief introduction to get you started, for a more in depth manual "
                               "refer to the user manual. <i>anna</i> is a simple annotation app that allows you to "
                               "annotate points belonging to a single object in videos as well as 2D or 3D "
                               "images. To start, load a video or image by drag and drop it on the application, "
                               "alternatively just open the file conventionally. As soon as you hit the record "
                               "button in the toolbar you can start annotate, either using mouse, touch or by "
                               "simply looking at it using a gaze tracker. Whenever something is recorded the "
                               "statusbar turns blue. While annotating you can flip through or just play the "
                               "video/image.</p>"
                               "<p><b>Annotate with mouse: </b>"
                               "As long as the left mouse button is pressed within the image the mouse position "
                               "is recorded.</p>"
                               "<p><b>Annotate with touch: </b>"
                               "As long as you touch the image the touched locations are recorded. If the device "
                               "supports multi-touch, multiple locations can be recorded at the same time.</p>"
                               "<p><b>Annotate with gaze tracker: </b>"
                               "Currently only the <i>EyeTribe</i> gaze tracker is supported. In order to connect to the "
                               "eye tracker start an <i>EyeTribe</i> server and calibrate the tracker with the software "
                               "provided by <i>EyeTribe</i>. Then connect <i>anna</i> by selecting Tracker->connect in the "
                               "menubar. Your current gaze position should now be indicated by a red circle within "
                               "<i>anna</i>. The gaze position is recorded as long as you press the 'Object in Sight' "
                               "button or spacebar.</p>"));
    info.exec();
}

void MainWindow::setVisible() {
    objectVisible = true;
    if(recordingAct->isChecked() && !tracker->isConnected()) {
        statusBar()->showMessage(tr("Gaze tracker is not connected!"));
    }
}

void MainWindow::setNotVisible() {
    objectVisible = false;
}


/*
 * ==============================================================================
 *                              protected
 * ==============================================================================
 */
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space)
        setVisible();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space)
        setNotVisible();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e) {
    const QUrl url = e->mimeData()->urls()[0];
    QString fileName = url.toLocalFile();
    qDebug() << "Dropped file:" << fileName;
    if(maybeSave())
        loadFile(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
bool MainWindow::maybeSave() {
    if (!annotationData.newContentSinceSave())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("New annotation data has been acquired, which has not been saved.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool MainWindow::saveData(const QString &fileName) {
    if(!annotationData.saveData(fileName)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message, 2000);
    return true;
}

void MainWindow::switchCurrentFrameReader(FrameReaderInterface *newFrameReader){
    disconnect(timer, 0, currentFrameReader, 0);
    disconnect(previousFrameAct, 0, currentFrameReader, 0);
    disconnect(nextFrameAct, 0, currentFrameReader, 0);

    currentFrameReader = newFrameReader;

    if(playAct->isChecked()) {
        if(reverseAct->isChecked())
            connect(timer, &QTimer::timeout, currentFrameReader, &FrameReaderInterface::previousFrame);
        else
            connect(timer, &QTimer::timeout, currentFrameReader, &FrameReaderInterface::nextFrame);
    }
    connect(previousFrameAct, &QAction::triggered, currentFrameReader, &FrameReaderInterface::previousFrame);
    connect(nextFrameAct, &QAction::triggered, currentFrameReader, &FrameReaderInterface::nextFrame);
}

void MainWindow::createActions() {
    // Menu bar
    QMenu *fileMenu = menuBar()->addMenu(tr("File"));

    openAct = new QAction(tr("Open video"), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    saveAct = new QAction(tr("Save Annotations"), this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setEnabled(false);
    fileMenu->addAction(saveAct);

    saveAsAct = new QAction(tr("Save Annotations As"), this);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    saveAct->setShortcut(QKeySequence::SaveAs);
    saveAct->setEnabled(false);
    fileMenu->addAction(saveAsAct);

    newFileAct = new QAction(tr("Delete current Annotations"), this);
    connect(newFileAct, &QAction::triggered, this, &MainWindow::newFile);
    saveAct->setShortcut(QKeySequence::New);
    saveAct->setEnabled(false);
    fileMenu->addAction(newFileAct);

    exitAct = new QAction(tr("Exit"), this);
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
    saveAct->setShortcut(QKeySequence::Quit);
    saveAct->setEnabled(false);
    fileMenu->addAction(exitAct);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));

    fitToWindowAct = new QAction(tr("Fit to Window"), frameLabel);
    connect(fitToWindowAct, &QAction::triggered, frameLabel, &FrameLabel::fitToWindow);
    fitToWindowAct->setShortcut(tr("Ctrl+1"));
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setStatusTip(tr("Shows full video frame at largest possible size, preserves video ratio"));
    viewMenu->addAction(fitToWindowAct);

    stretchAct = new QAction(tr("Stretch Window"), frameLabel);
    connect(stretchAct, &QAction::triggered, frameLabel, &FrameLabel::stretch);
    stretchAct->setShortcut(tr("Ctrl+2"));
    stretchAct->setCheckable(true);
    stretchAct->setStatusTip(tr("Shows full video frame filling the whole window, ignores video ratio"));
    viewMenu->addAction(stretchAct);

    fixedNativeAct = new QAction(tr("Fixed Native"), frameLabel);
    connect(fixedNativeAct, &QAction::triggered, frameLabel, &FrameLabel::fixedNative);
    fixedNativeAct->setShortcut(tr("Ctrl+3"));
    fixedNativeAct->setCheckable(true);
    fixedNativeAct->setStatusTip(tr("Shows video frame at native resolution, might not show everything"));
    viewMenu->addAction(fixedNativeAct);

    QActionGroup *representationGroup = new QActionGroup(this);
    representationGroup->addAction(fitToWindowAct);
    representationGroup->addAction(stretchAct);
    representationGroup->addAction(fixedNativeAct);
    fitToWindowAct->setChecked(true);

    viewMenu->addSeparator();

    visualizeAnnotationAct = new QAction(tr("Visualize Annotations Points"), frameLabel);
    visualizeAnnotationAct->setCheckable(true);
    visualizeAnnotationAct->setChecked(true);
    visualizeAnnotationAct->setEnabled(true);
    connect(this, &MainWindow::updateAnnotationPoint, annotationLabel, &AnnotationLabel::put);
    connect(visualizeAnnotationAct, &QAction::triggered, this, &MainWindow::visualizeAnnotation);
    visualizeAnnotationAct->setShortcut(tr("Ctrl+v"));
    visualizeAnnotationAct->setStatusTip(tr("Toogles visualization of current annotation points, visualized only within video frame"));
    viewMenu->addAction(visualizeAnnotationAct);


    reverseAct = new QAction(tr("Reverse Slide Show"), frameLabel);
    connect(reverseAct, &QAction::triggered, this, &MainWindow::reverse);
    reverseAct->setShortcut(tr("Ctrl+T"));
    reverseAct->setCheckable(true);
    reverseAct->setStatusTip(tr("If the slide show is started, it is played in reverse"));
    viewMenu->addAction(reverseAct);

    connectTrackerAct = new QAction(tr("Connect"), this);
    connect(connectTrackerAct, &QAction::triggered, this, &MainWindow::connectTracker);
    connectTrackerAct->setCheckable(true);
    connectTrackerAct->setEnabled(true);
    viewMenu->addAction(connectTrackerAct);


    disconnectTrackerAct = new QAction(tr("Disconnect"), this);
    connect(disconnectTrackerAct, &QAction::triggered, this, &MainWindow::disconnectTracker);
    disconnectTrackerAct->setCheckable(true);
    disconnectTrackerAct->setChecked(true);
    disconnectTrackerAct->setEnabled(false);
    viewMenu->addAction(disconnectTrackerAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));

    aboutAct = new QAction(tr("About"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAct);

    rtfmAct = new QAction(tr("Help"), this);
    connect(rtfmAct, &QAction::triggered, this, &MainWindow::rtfm);
    helpMenu->addAction(rtfmAct);

    // Tool bar
    QToolBar *recordToolBar = addToolBar(tr("Recording"));

    QIcon recordIcon = QIcon(":/images/mediaRecord.svg");
    recordIcon.addFile(":/images/mediaRecordDisabled.svg", QSize(), QIcon::Disabled);
    recordIcon.addFile(":/images/mediaRecording.svg", QSize(), QIcon::Normal, QIcon::On);


    recordingAct = new QAction(tr("Start/stop recording"), this);
    connect(recordingAct, &QAction::triggered, this, &MainWindow::recording);
    recordingAct->setEnabled(true);
    recordingAct->setCheckable(true);
    recordingAct->setChecked(true);
    recordingAct->setShortcut(tr("R"));
    recordingAct->setStatusTip(tr("Start/stop recording"));
    recordToolBar->addAction(recordingAct);

    objectVisibleButton = new TouchButton("Object \n in Sight", recordToolBar);
    objectVisibleButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    objectVisibleButton->setEnabled(false);
    recordToolBar->addWidget(objectVisibleButton);
    connect(objectVisibleButton, &QAbstractButton::pressed, this, &MainWindow::setVisible);
    connect(objectVisibleButton, &QAbstractButton::released, this, &MainWindow::setNotVisible);


    navigateToolBar = addToolBar(tr("Navigation"));

    QIcon previousFrameIcon = QIcon(":/images/mediaRewind.svg");
    previousFrameIcon.addFile(":/images/mediaRewindDisabled.svg", QSize(), QIcon::Disabled);
    previousFrameAct = new QAction(previousFrameIcon, tr("Previous frame"), this);
    previousFrameAct->setShortcut(Qt::Key_Left);
    connect(previousFrameAct, &QAction::triggered, currentFrameReader, &FrameReaderInterface::previousFrame);
    navigateToolBar->addAction(previousFrameAct);


    QIcon nextFrameIcon = QIcon(":/images/mediaFastForward.svg");
    nextFrameIcon.addFile(":/images/mediaFastForwardDisabled.svg", QSize(), QIcon::Disabled);
    nextFrameAct = new QAction(nextFrameIcon, tr("Next frame"), this);
    nextFrameAct->setShortcut(Qt::Key_Right);
    connect(nextFrameAct, &QAction::triggered, currentFrameReader, &FrameReaderInterface::nextFrame);
    navigateToolBar->addAction(nextFrameAct);

    positionSlider = new IntuitiveSlider(Qt::Horizontal, this);
    positionSlider->setEnabled(false);
    positionSlider->setToolTip(tr("Seek"));
    positionSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(positionSlider, &QAbstractSlider::valueChanged, this, &MainWindow::setPosition);
    navigateToolBar->addWidget(positionSlider);
    connect(navigateToolBar, &QToolBar::orientationChanged, positionSlider, &QAbstractSlider::setOrientation);

    positionLabel = new QLabel(tr("0/0"), this);
    positionLabel->setMinimumWidth(80);
    positionLabel->setAlignment(Qt::AlignCenter);
    navigateToolBar->addWidget(positionLabel);

    playToolBar = addToolBar(tr("Play"));
    playToolBar->setEnabled(false);

    speedSlider = new SpeedSlider(Qt::Horizontal, this);
    speedSlider->setToolTip(tr("play speed"));
    speedSlider->setHzRange(0.5, 30);
    speedSlider->setRange(0, 99);
    speedSlider->setSingleStep(1);
    speedSlider->setPageStep(10);
    speedSlider->setValue(99);
    speedSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(speedSlider, &SpeedSlider::valueChangedMS, timer, QOverload<int>::of(&QTimer::setInterval));
    //connect(speedSlider, &SpeedSlider::valueChangedMS, timer, &QTimer::setInterval);
    connect(playToolBar, &QToolBar::orientationChanged, speedSlider, &SpeedSlider::setOrientation);

    speedLabel = new QLabel(tr("30"), this);
    speedLabel->setMinimumWidth(30);
    speedLabel->setAlignment(Qt::AlignCenter);
    //connect(speedSlider, &SpeedSlider::valueChangedHz, speedLabel, QOverload<double>::of(&QLabel::setNum));
    connect(speedSlider, &SpeedSlider::valueChangedHz, this, &MainWindow::onSpeedSliderValueChanged);

    QIcon playIcon = QIcon(":/images/mediaPlay.svg");
    playIcon.addFile(":/images/mediaPlayDisabled.svg", QSize(), QIcon::Disabled);
    playAct = new QAction(playIcon, tr("Play"), this);
    playAct->setShortcut(Qt::Key_Space);
    playAct->setCheckable(true);
    playAct->setStatusTip(tr("play/pause automatic slide show"));
    connect(playAct, &QAction::triggered, this, &MainWindow::play);
    navigateToolBar->addAction(playAct);

    QLabel* speedLabelStatic = new QLabel(tr("speed (Hz):"));
    speedLabelStatic->setAlignment(Qt::AlignCenter);
    playToolBar->addWidget(speedLabelStatic);
    playToolBar->addWidget(speedLabel);
    playToolBar->addWidget(speedSlider);

    const qreal ICONSIDELENGTH = 1;   // length of an icons side in inch
    QSize iconSize(this->logicalDpiX()*ICONSIDELENGTH, this->logicalDpiY()*ICONSIDELENGTH);
    playToolBar->setIconSize(iconSize);
    recordToolBar->setIconSize(iconSize);
    addToolBar(Qt::LeftToolBarArea, playToolBar);
    addToolBar(Qt::RightToolBarArea, recordToolBar);
}

void MainWindow::updateActions() {
    saveAct->setEnabled(annotationData.newContentSinceSave());
    saveAsAct->setEnabled(annotationData.newContentSinceSave());

    bool trackerConnected = tracker->isConnected();
    connectTrackerAct->setEnabled(!trackerConnected);
    connectTrackerAct->setChecked(trackerConnected);
    disconnectTrackerAct->setEnabled(trackerConnected);
    disconnectTrackerAct->setChecked(!trackerConnected);

    recordingAct->setEnabled(currentFrameReader->isOpened());
    objectVisibleButton->setEnabled(trackerConnected);

    navigateToolBar->setEnabled(currentFrameReader->isOpened());
    playToolBar->setEnabled(currentFrameReader->isOpened());
}



