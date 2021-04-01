/*
 * Extents the IntuitiveSlider for selecting the speed in Hz as a double. This
 * is done by adding an additional layer, hence you can use the value range
 * from the QSlider to select the number of steps e.g. mySpeedSlider.setRange(0,99)
 * for 100 steps. Whenever the slider value is changed the values in Hz and
 * milliseconds are updated and emit a corresponding signal.
 *
 * TODO: unit test
 *
 * 29. March 2017, Severin Tobler
 */
#ifndef SPEEDSLIDER_H
#define SPEEDSLIDER_H

#include "intuitiveslider.h"

class SpeedSlider : public IntuitiveSlider {
    Q_OBJECT
public:
    SpeedSlider(QWidget *parent = Q_NULLPTR);
    SpeedSlider(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR);

    void setHzRange(double min, double max);
    void setHzMinimum(double value);
    void setHzMaximum(double value);
    double hzMinimum() const;
    double hzMaximum() const;
    double valueHz() const;
    int valueMS() const;

signals:
    void valueChangedMS(int value);
    void valueChangedHz(double value);

private slots:
    void valueChangedExtention(int value);

private:
    double hzMin = 1;
    double hzMax = 30;
    double hz;
    int ms;
};

#endif // SPEEDSLIDER_H
