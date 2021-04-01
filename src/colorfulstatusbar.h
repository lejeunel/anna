/*
 * Extends 'QStatusBar' by adding a 'setStyleSheet' slot. This slot allows to
 * change the stylesheet of the statusbar momentarily.
 *
 * 18. April 2017, Severin Tobler
 */
#ifndef COLORFULSTATUSBAR_H
#define COLORFULSTATUSBAR_H

#include <QStatusBar>
#include <QTimer>

class ColorfulStatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit ColorfulStatusBar(QWidget *parent = Q_NULLPTR);

public slots:
    void setStyleSheet(const QString &styleSheet, int timeout = 0); // timeout in ms

protected slots:
    void resetStyleSheet();

private:
    QTimer *timer;
};

#endif // COLORFULSTATUSBAR_H
