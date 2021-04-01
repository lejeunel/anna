/*
 * This slider jumps to where you pressed. Which differs from the default Qt
 * behavior: moving by page step in that direction.
 *
 * http://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump
 *
 * 29. March 2017, Severin Tobler
 */
#ifndef INTUITIVESLIDER_H
#define INTUITIVESLIDER_H

#include <QSlider>
#include <QProxyStyle>

class IntuitiveSlider : public QSlider {
    Q_OBJECT
public:
    IntuitiveSlider(QWidget *parent = Q_NULLPTR);
    IntuitiveSlider(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR);

private:
    class JumpStyle : public QProxyStyle {
    public:
        using QProxyStyle::QProxyStyle;

        int styleHint(QStyle::StyleHint hint, const QStyleOption* option = 0,
                      const QWidget* widget = 0, QStyleHintReturn* returnData = 0) const;
    };
};

#endif // INTUITIVESLIDER_H
