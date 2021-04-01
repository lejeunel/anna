#include "annotationlabel.h"
#include <QDebug>

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
AnnotationLabel::AnnotationLabel(QWidget * parent, Qt::WindowFlags f) : QLabel(parent, f) {
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAlignment(Qt::AlignCenter);
    setVisible(true);
    setPixmap(QPixmap(":/images/circle.svg"));
}

/*
 * ==============================================================================
 *                              public slots
 * ==============================================================================
 */
void AnnotationLabel::put(const QPoint &pos) {
    setGeometry(pos.x()-16, pos.y()-16, 32, 32);
    //setGeometry(200, 200, 32, 32);
    //qDebug() << pos.x() << "," << pos.y();
}
