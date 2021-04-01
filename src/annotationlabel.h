/*
 * Basically a 'x', used to indicates the gaze position.
 * 18. February 2017, Severin Tobler
 */
#ifndef ANNOTATIONLABEL_H
#define ANNOTATIONLABEL_H

#include <QLabel>
#include "framereaderinterface.h"

class AnnotationLabel : public QLabel {
    Q_OBJECT

    public:
        AnnotationLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);

    public slots:
        void put(const QPoint &pos);
};

#endif // ANNOTATIONLABEL_H
