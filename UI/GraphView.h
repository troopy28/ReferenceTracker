#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>

#include "../Data/Document.h"

class GraphView : public QWidget
{
	Q_OBJECT

public:
	explicit GraphView(Data::Document& document, QWidget* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	void MovePlayhead(int target);

	void ForceRedraw();
	void DrawHeader(QPainter& widgetPainter);
	void DrawCurves(QPainter& painter);
	void DrawPlayhead(QPainter& painter) const;

	_NODISCARD int frameToControlPos(int frame) const;

	Data::Document& m_document;
	QPixmap m_headerPixmap;
	QPixmap m_curvesPixmap;
	bool m_requireRedraw;
};
