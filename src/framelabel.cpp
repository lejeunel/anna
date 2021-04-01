#include "framelabel.h"
#include <QDebug>
/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
FrameLabel::FrameLabel(QWidget * parent, Qt::WindowFlags f)
    : QLabel(parent, f)
    , representation(FIT_TO_WINDOW)
    , timerMouse(new QTimer(this))
    , timerTouch(new QTimer(this)) {
    setScaledContents(false);
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    timerMouse->setSingleShot(false);
    timerMouse->setInterval(30);
    timerTouch->setSingleShot(false);
    timerTouch->setInterval(30);

    setAttribute(Qt::WA_AcceptTouchEvents);
}

QPoint FrameLabel::getFramePosition() const {
    int dx = (frameRect().size().width() - imageSize.width())/2;
    int dy = (frameRect().size().height() - imageSize.height())/2;
    QPoint imagePosition = mapToGlobal(frameRect().topLeft());
    imagePosition.setX(imagePosition.x() + dx);
    imagePosition.setY(imagePosition.y() + dy);
    return imagePosition;
}

QSize FrameLabel::getFrameSize() const {
    return imageSize;
}


/*
 * ==============================================================================
 *                              public slots
 * ==============================================================================
 */
void FrameLabel::resizeEvent(QResizeEvent * /*event*/) {
    drawImage();
}

QPointF FrameLabel::globalPosToImCoord(QPoint pos){

    QPoint imagePosition = getFramePosition();
    QSize frameSize = getFrameSize();
    QSize imageSize = image.size();
    int height = imageSize.height();
    int width = imageSize.width();

    double x = (pos.x()-imagePosition.x())/(double)frameSize.width();
    double y = (pos.y()-imagePosition.y())/(double)frameSize.height();

    x = x*double(width);
    y = y*double(height);

    return QPointF(x, y);
}

void FrameLabel::drawCrosshair() {
    // Draws circle around click
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor("red");
    pen.setWidth(3);
    painter.setPen(pen);

    QPointF center = globalPosToImCoord(lastMousePos);
    painter.drawEllipse(center, 5, 5);

    drawImage();

}

void FrameLabel::setImage(const QImage &newFrame, qint64 frameIndex) {
    image = QPixmap::fromImage(newFrame);
    image_idx = frameIndex;
    if(mousePressed)
        drawCrosshair();
    else
        drawImage();

}

void FrameLabel::fitToWindow() {
    representation = FIT_TO_WINDOW;
    drawImage();
}

void FrameLabel::stretch() {
    representation = STRETCH;
    drawImage();
}

void FrameLabel::fixedNative() {
    representation = FIXED_NATIVE;
    drawImage();
}


/*
 * ==============================================================================
 *                              protected
 * ==============================================================================
 */
void FrameLabel::mouseMoveEvent(QMouseEvent *event) {
    lastMousePos = event->globalPos();
}

void FrameLabel::mousePressEvent(QMouseEvent *event) {
    lastMousePos = event->globalPos();
    timerMouse->start();
    connect(timerMouse, &QTimer::timeout, this, &FrameLabel::emitMousePosition, Qt::UniqueConnection);
    mousePressed = true;
    emitMousePosition();
}

void FrameLabel::mouseReleaseEvent(QMouseEvent *) {
    disconnect(timerMouse, 0, this, 0);
    timerMouse->stop();
    mousePressed = false;
}

bool FrameLabel::event(QEvent *event) {
    if(event->type() == QEvent::TouchBegin ||
            event->type() == QEvent::TouchUpdate ||
            event->type() == QEvent::TouchEnd ||
            event->type() == QEvent::TouchCancel) {
        for(QTouchEvent::TouchPoint pointf : static_cast<QTouchEvent*>(event)->touchPoints()) {
            if(pointf.state() == Qt::TouchPointPressed || pointf.state() == Qt::TouchPointMoved) {
                QPoint point(qRound(pointf.screenPos().x()), qRound(pointf.screenPos().y()));
                touchPoints.insert(pointf.id(), point);
                if(pointf.state() == Qt::TouchPointPressed)
                    emit markedPosition(point);
            }
            if(pointf.state() == Qt::TouchPointReleased) {
                touchPoints.remove(pointf.id());
            }
        }

        if(event->type() == QEvent::TouchBegin) {
            connect(timerTouch, &QTimer::timeout, this, &FrameLabel::emitTouchPositions, Qt::UniqueConnection);
            timerTouch->start();
        }
        if(event->type() == QEvent::TouchEnd) {
            disconnect(timerTouch, 0, this, 0);
            timerTouch->stop();
        }
        return true;
    }

    return QLabel::event(event);
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
void FrameLabel::emitMousePosition() {
    emit markedPosition(lastMousePos);
    setImage(image.toImage(), image_idx);
}

void FrameLabel::emitTouchPositions() {
    QMapIterator<int, QPoint> i(touchPoints);
    while (i.hasNext()) {
        i.next();
        emit markedPosition(i.value());
    }
}

void FrameLabel::drawImage() {
    if(image.isNull())
        return;
    QPixmap pix;
    if(representation == FIT_TO_WINDOW)
        pix = image.scaled(frameRect().size(), Qt::KeepAspectRatio);
    else if(representation == STRETCH)
        pix = image.scaled(frameRect().size());
    else    // representation == FIXED_NATIVE
        pix = image;
    imageSize = pix.size();
    setPixmap(pix);
}

