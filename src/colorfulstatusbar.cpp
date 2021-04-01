#include "colorfulstatusbar.h"

ColorfulStatusBar::ColorfulStatusBar(QWidget *parent) : QStatusBar(parent), timer(new QTimer()) {
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &ColorfulStatusBar::resetStyleSheet);
}

void ColorfulStatusBar::setStyleSheet(const QString &styleSheet, int timeout) {
    QStatusBar::setStyleSheet(styleSheet);
    timer->start(timeout);
}

void ColorfulStatusBar::resetStyleSheet() {
    QStatusBar::setStyleSheet("");
}
