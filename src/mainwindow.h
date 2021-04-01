#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include "framereaderinterface.h"
#include "eyetrackerinterface.h"
#include "annotationdatamanager.h"
#include "framelabel.h"
#include "annotationlabel.h"
#include <QLabel>
#include <QObject>
#include <QTimer>
#include "intuitiveslider.h"
#include "speedslider.h"
#include "touchbutton.h"
#include "colorfulstatusbar.h"
#include <stdlib.h>


QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QSlider;
class QSpinBox;
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    bool loadFile(const QString &fileName);

private slots:
    void newTrackerData(const TrackingData &data);  // data from eye tracker
    void newManualData(const QPoint &pos);  // user clicks on object
    void isModified();
    void open();
    bool save();
    bool saveAs();
    void newFile();
    void visualizeAnnotation();
    void connectTracker();
    void disconnectTracker();
    void recording();
    void updatePosition(const QImage &newFrame, qint64 frameIndex);
    void updateDuration(qint64 duration);
    void setPosition(int position);
    void play();
    void reverse();
    void about();
    void rtfm();
    void setVisible();
    void setNotVisible();

signals:
    void saveAnnotationData(const CsvData &data);
    //void saveAnnotationData(const CsvData &data);
    void updateAnnotationPoint(const QPoint &pos);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool maybeSave();
    bool saveData(const QString &fileName);
    void switchCurrentFrameReader(FrameReaderInterface *newFrameReader);
    void createActions();
    void updateActions();
    void onSpeedSliderValueChanged(double var);

    std::vector<FrameReaderInterface *> frameReaders;
    FrameReaderInterface *currentFrameReader;
    EyeTrackerInterface *tracker;
    AnnotationDataManager annotationData;
    FrameLabel *frameLabel;
    AnnotationLabel *annotationLabel;
    IntuitiveSlider *positionSlider;
    QLabel *positionLabel;
    QLabel* speedLabel;
    SpeedSlider *speedSlider;
    QToolBar *navigateToolBar;
    QToolBar *playToolBar;
    QTimer *timer;
    TouchButton *objectVisibleButton;
    bool objectVisible;

    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *visualizeAnnotationAct;
    QAction *connectTrackerAct;
    QAction *disconnectTrackerAct;
    QAction *recordingAct;
    QAction *previousFrameAct;
    QAction *nextFrameAct;
    QAction *playAct;
    QAction *reverseAct;
    QAction *newFileAct;
    QAction *exitAct;
    QAction *fitToWindowAct;
    QAction *stretchAct;
    QAction *fixedNativeAct;
    QAction *aboutAct;
    QAction *rtfmAct;
};

#endif // MAINWINDOW_H
