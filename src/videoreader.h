/*
 * Reads frames from a video file. Uses OpenCV for video reading, as it was the
 * easiest way to read a video frame by frame. Since OpenCV is a pain in the
 * butt, it has been used as sparse as possible. For more info about OpenCV:
 * http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html
 * 18. February 2017, Severin Tobler
 */
#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include "framereaderinterface.h"
#include <QFileInfo>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

class VideoReader : public FrameReaderInterface {
public:
    VideoReader(AnnotationDataManager *a);
    bool load(QString filePathName) override;
    bool isOpened() const override;
    qint64 getCurrentFrameNumber() const override;
    QSize getFrameSize() const override;
    QString getName() const override;
    void setFrame(qint64 frameNumber) override;

public slots:
    void nextFrame() override;
    void previousFrame() override;

private:
    qint64 getNbFrames() const;
    QImage convertToQImage(const cv::Mat &mat);

    cv::Mat frame;
    cv::VideoCapture cap;
    QFileInfo fileInfo;
};

#endif // VIDEOREADER_H
