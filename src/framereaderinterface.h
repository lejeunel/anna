/*
 * Interface for the frame reader. This interface has been implemented in case
 * one wants to extend the application to also read other 3D data (eg oct, mri
 * or ct volumes). In order to do so, inheart from the 'FrameReaderInterface'
 * class. For an example check out the 'VideoReader' or 'VolumeReader' class.
 * Then add your new frame reader to the other frame readers in the constructor
 * of mainwindow.cpp.
 * For more information on QT's signals and slots check out:
 * http://doc.qt.io/qt-5/signalsandslots.html
 * 18. February 2017, Severin Tobler
 */
#ifndef FRAMEREADERINTERFACE_H
#define FRAMEREADERINTERFACE_H

#include <QObject>
#include <QPainter>
#include <QImage>
#include <QDebug>
#include "annotationdatamanager.h"
#include <opencv2/opencv.hpp>

using namespace cv;

class FrameReaderInterface : public QObject {
    Q_OBJECT

private:
    AnnotationDataManager * annotation_manager;
    QImage * curr_frame;
    qint64 curr_frame_idx;

public:
    virtual ~FrameReaderInterface() { }
    virtual bool load(QString filePathName) = 0;    // returns true if file has successfully been loaded
    virtual bool isOpened() const = 0;
    virtual qint64 getCurrentFrameNumber() const = 0;   // 0-based index
    virtual QSize getFrameSize() const = 0;
    virtual QString getName() const = 0;    // name of the file (appears in the tracker file)
    virtual void setFrame(qint64 frameNumber) = 0; // jump to frame 'frameNumber'
    void setAnnotationDataManager(AnnotationDataManager* a){
        annotation_manager = a;
    }

    QImage drawAllPoints(QImage frame_in){

        QPainter painter(&frame_in);

        painter.setRenderHint(QPainter::Antialiasing, true);
        QPen pen(Qt::green, 2);
        painter.setPen(pen);
        QBrush brush(Qt::green);
        painter.setBrush(brush);

        for(const CsvData& data : annotation_manager->getData(getCurrentFrameNumber())){

            QSize s = frame_in.size();
            int height = s.height();
            int width = s.width();

            double x = data.x*(double)width;
            double y = data.y*(double)height;
            QPointF center = QPointF(x, y);
            painter.drawEllipse(center, 4.0, 4.0);
        }

        return frame_in;
    }

public slots:
    virtual void nextFrame() = 0;
    virtual void previousFrame() = 0;

signals:
    void frameChanged(const QImage &newFrame, qint64 frameIndex);
    void nbFramesChanged(qint64 nbFrames);  // total number of frames changed
};


#endif // FRAMEREADERINTERFACE_H
