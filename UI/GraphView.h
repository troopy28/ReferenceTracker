#pragma once

#include <QWidget>
#include <QVBoxLayout>

#include "../Data/Document.h"

class GraphView : public QWidget
{
	Q_OBJECT

public:
	explicit GraphView(Data::Document& document, QWidget* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	void MovePlayhead(int target);

	void DrawHeader(QPainter& painter) const;
	void DrawCurves(QPainter& painter);
	void DrawPlayhead(QPainter& painter) const;

	_NODISCARD int frameToControlPos(int frame) const;

	Data::Document& m_document;
};
