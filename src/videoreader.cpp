#include "videoreader.h"

#include <QDebug>
#include "opencv2/opencv.hpp"

using namespace cv;

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
VideoReader::VideoReader(AnnotationDataManager * a) {
    setAnnotationDataManager(a);
}

bool VideoReader::load(QString filePathName) {
    qDebug() << "Opening video (VideoReader): " << filePathName;
    fileInfo.setFile(filePathName);
    cap = VideoCapture(filePathName.toStdString()); // open the default camera
    if(isOpened()) {
        emit nbFramesChanged(getNbFrames());
        nextFrame();
        return true;
    }
    return false;
}

bool VideoReader::isOpened() const {
    return cap.isOpened();
}

qint64 VideoReader::getCurrentFrameNumber() const {
    return round(cap.get(cv::CAP_PROP_POS_FRAMES))-1;
}

QSize VideoReader::getFrameSize() const {
    return QSize(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
}

QString VideoReader::getName() const {
    return fileInfo.fileName();
}

void VideoReader::setFrame(qint64 frameNumber) {
    if(cap.isOpened() && frameNumber>=0 && frameNumber<getNbFrames()) {
        cap.set(cv::CAP_PROP_POS_FRAMES, frameNumber);
        cap >> frame;
        qDebug() << "video reader frame " << frameNumber << " available";
        emit frameChanged(drawAllPoints(convertToQImage(frame)), frameNumber);
    }
    else
        qDebug() << "setFrame: requested frame outside range... do nothing";
}

/*
 * ==============================================================================
 *                              public slots
 * ==============================================================================
 */
void VideoReader::nextFrame() {
    if(!cap.isOpened())
        return;
    qint64 frameNumber = getCurrentFrameNumber()+1;
    if(frameNumber<getNbFrames()) {
        cv::Mat frame;
        cap >> frame;
        qDebug() << "video reader frame " << frameNumber << " available";
        emit frameChanged(drawAllPoints(convertToQImage(frame)), frameNumber);
        //emit frameChanged(convertToQImage(frame), frameNumber);

    }
    else
        qDebug() << "nextFrame: requested frame outside range... do nothing";
}

void VideoReader::previousFrame() {
    if(!cap.isOpened())
        return;
    qint64 previousFrameNumber = getCurrentFrameNumber()-1;
    setFrame(previousFrameNumber);
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
qint64 VideoReader::getNbFrames() const {
    return floor(cap.get(cv::CAP_PROP_FRAME_COUNT));
}

QImage VideoReader::convertToQImage(const cv::Mat &mat) {
    // from https://github.com/matoilic/ardoor/blob/master/Desktop/QtUtil.cpp
    // but with convertion to rgb: img.rgbSwapped();

    // 8-bits unsigned, NO. OF CHANNELS=1
    if (mat.type() == CV_8UC1) {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++) {
            colorTable.push_back(qRgb(i, i, i));
        }

        // Create QImage with same dimensions as input Mat
        QImage img((const uchar*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }

    // 8-bits unsigned, NO. OF CHANNELS=3
    if (mat.type() == CV_8UC3) {
        // Create QImage with same dimensions as input Mat
        QImage img((uchar*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }

    qDebug() << "can not transform this type of cv::Mat to QImage!";
    return QImage();
}



