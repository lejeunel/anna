/*
 * Displays the video frame. Keeps track of position and size of the video frame within this label.
 * 18. February 2017, Severin Tobler
 */
#ifndef FRAMELABEL_H
#define FRAMELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QTimer>
#include <QPainter>
#include <QPen>

class FrameLabel : public QLabel {
    Q_OBJECT
public:
    FrameLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);

    QPoint getFramePosition() const;
    QSize getFrameSize() const;

public slots:
    void resizeEvent(QResizeEvent *event) override;
    void setImage(const QImage &newFrame, qint64 frameIndex);
    void drawCrosshair();
    void fitToWindow();
    void stretch();
    void fixedNative();

signals:
    void markedPosition(const QPoint &pos);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;

private slots:
    void emitMousePosition();
    void emitTouchPositions();

private:
    void drawImage();
    QPointF FrameLabel::globalPosToImCoord(QPoint);
    QPixmap image;
    qint64 image_idx;
    QSize imageSize;
    QTimer *timerMouse;
    QTimer *timerTouch;
    QPoint lastMousePos;
    QMap<int, QPoint> touchPoints;
    bool mousePressed;

    enum Representation {FIT_TO_WINDOW, STRETCH, FIXED_NATIVE} representation;
};

#endif // FRAMELABEL_H
