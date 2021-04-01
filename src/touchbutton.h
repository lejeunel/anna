/*
 * Touch button for 'object in sight' button. The Qt button causes problems on
 * the Windows Surface tablet. Touch events are translated to mouse events. As
 * Windows mouse right click corresponds to the touch gesture of press and hold
 * it interferes with our button. However, we still want to use the button with
 * mouse left click on Mac and PC. There are several ways to get around this.
 * Here the ideas I had:
 * - Use touch events to trigger pressed() and released() signals, no longer
 *   problems with different gestures
 * - leave the mouse events in, so it is still possible to activate the button
 *   on Mac or PC
 * - filter QEvent::ContextMenu, as we don't want to enter the menu while
 *   pressing this button
 *
 * 23. March 2017, Severin Tobler
 */

#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include <QPushButton>
#include <QEvent>
#include <QTimer>

class TouchButton : public QPushButton {
public:
    TouchButton(QWidget *parent = Q_NULLPTR);
    TouchButton(const QString &text, QWidget *parent = Q_NULLPTR);
    TouchButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);

protected:
    bool event(QEvent *event) override;
};

#endif // TOUCHBUTTON_H
