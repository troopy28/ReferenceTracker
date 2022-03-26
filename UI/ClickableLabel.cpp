#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, const Qt::WindowFlags f)
    : QLabel(parent, f) {

}

void ClickableLabel::mousePressEvent(QMouseEvent*) {
    emit clicked();
}