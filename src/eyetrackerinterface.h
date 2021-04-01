/*
 * Eye tracker Interface. As soon as the tracker has new data the signal
 * 'newGazeData' should be emitted. To implement a new tracker inhert from the
 * class 'EyeTrackerInterface'. For an example take a look at the
 * 'EyeTribeTracker' class. For more information on QT's signals check out:
 * http://doc.qt.io/qt-5/signalsandslots.html
 * 18. February 2017, Severin Tobler
 */
#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QMetaType>


struct TrackingData {
    int x, y;             // gaze coordinates in pixels
    bool isFixated;
};
Q_DECLARE_METATYPE(TrackingData)


class EyeTrackerInterface : public QObject {
    Q_OBJECT
public:
    EyeTrackerInterface() {
        qRegisterMetaType<TrackingData>();
    }
    virtual ~EyeTrackerInterface() { }
    virtual void connectTracker() = 0;
    virtual void disconnectTracker() = 0;
    virtual bool isConnected() const = 0;

signals:
    void newAnnotationData(const TrackingData &data);
};


#endif // EYETRACKERINTERFACE_H
