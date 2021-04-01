#include "intuitiveslider.h"

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
IntuitiveSlider::IntuitiveSlider(QWidget *parent) : QSlider(parent) {
    setStyle(new JumpStyle(this->style()));
}

IntuitiveSlider::IntuitiveSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent) {
    setStyle(new JumpStyle(this->style()));
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
int IntuitiveSlider::JumpStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option,
                                          const QWidget *widget, QStyleHintReturn *returnData) const {
   if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
       return (Qt::LeftButton | Qt::MidButton | Qt::RightButton);
   return QProxyStyle::styleHint(hint, option, widget, returnData);
}
