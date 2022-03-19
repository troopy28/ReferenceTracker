#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}