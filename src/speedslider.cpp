#include "speedslider.h"
#include <QtMath>
#include <QDebug>

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
SpeedSlider::SpeedSlider(QWidget *parent) : IntuitiveSlider(parent) {
    connect(this, &SpeedSlider::valueChanged, this, &SpeedSlider::valueChangedExtention);
}

SpeedSlider::SpeedSlider(Qt::Orientation orientation, QWidget *parent) : IntuitiveSlider(orientation, parent) {
    connect(this, &SpeedSlider::valueChanged, this, &SpeedSlider::valueChangedExtention);
}

void SpeedSlider::setHzRange(double min, double max){
    setHzMaximum(max);
    setHzMinimum(min);
}

void SpeedSlider::setHzMinimum(double value) {
    if(value > 0)
        hzMin = value;
    else
        hzMin = 0.000001;
    if(hzMin > hzMax)
        hzMax = hzMin;
}

void SpeedSlider::setHzMaximum(double value) {
    if(value > 0)
        hzMax = value;
    else
        hzMax = 0.000001;
    if(hzMin > hzMax)
        hzMin = hzMax;
}

double SpeedSlider::hzMinimum() const {
    return hzMin;
}

double SpeedSlider::hzMaximum() const {
    return hzMax;
}

double SpeedSlider::valueHz() const {
    return hz;
}

int SpeedSlider::valueMS() const {
    return ms;
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
void SpeedSlider::valueChangedExtention(int value) {
    int sliderMin = minimum();
    int sliderMax = maximum();
    if(sliderMin != sliderMax && hzMax != hzMin) {
        double valueNorm =  (value-sliderMin)/double(sliderMax-sliderMin);
        hz = valueNorm*(hzMax-hzMin) + hzMin;
    }
    else {
        hz = (hzMax+hzMin)/2;
    }
    ms = qRound(1000/hz);
    emit valueChangedHz(hz);
    emit valueChangedMS(ms);
//    qDebug() << value << "; " << hz << "Hz; " << ms << "ms";
}
