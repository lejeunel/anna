#include "touchbutton.h"
#include <QDebug>

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
TouchButton::TouchButton(QWidget *parent) : QPushButton(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
}

TouchButton::TouchButton(const QString &text, QWidget *parent)
        : QPushButton(text, parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
}

TouchButton::TouchButton(const QIcon &icon, const QString &text, QWidget *parent)
        : QPushButton(icon, text, parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
}


/*
 * ==============================================================================
 *                              protected
 * ==============================================================================
 */
bool TouchButton::event(QEvent *event) {
    /*
    if(event->type() != QEvent::StyleAnimationUpdate && event->type() != QEvent::Paint && event->type() != QEvent::HoverMove)
        qDebug() << event->type();
    */
    if(event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel) {
        emit released();
        return true;
    } else if(event->type() == QEvent::TouchBegin) {
        emit pressed();
        return true;
    } else if(event->type() == QEvent::ContextMenu) {
        return true;
    }
    return QPushButton::event(event);
}

