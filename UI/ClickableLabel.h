#pragma once

// Completely copypasted from: https://wiki.qt.io/Clickable_QLabel

#include "../common.h"
#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel() override = default;
    Q_DISABLE_COPY_MOVE(ClickableLabel);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;

};