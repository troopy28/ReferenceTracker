#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>

#include "../Data/Document.h"
#include "../Tracking/TrackingManager.h"

class GraphView : public QWidget
{
	Q_OBJECT

public:
	explicit GraphView(Data::Document& document, Tracking::TrackingManager& trackingManager, QWidget* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* evt) override;
	void resizeEvent(QResizeEvent* evt) override;
	void mousePressEvent(QMouseEvent* evt) override;
	void mouseMoveEvent(QMouseEvent* evt) override;
	void mouseReleaseEvent(QMouseEvent* evt) override;

private:
	void MovePlayheadToFrame(int frame, bool instantaneous = false);
	void SmoothPlayheadMove(double x);

	void ForceRedraw();
	void DrawHeader(QPainter& widgetPainter);
	void DrawCurves(QPainter& widgetPainter);
	void DrawPlayhead(QPainter& painter) const;

	_NODISCARD int frameToControlPos(int frame) const;
	_NODISCARD int controlPosToFrame(int controlPos) const;

	Data::Document& m_document;
	Tracking::TrackingManager& m_trackingManager;
	QPixmap m_headerPixmap;
	QPixmap m_curvesPixmap;
	bool m_requireRedraw;
	bool m_requireCurveRedraw;
	int m_playheadPosition;
	bool m_movingPlayhead;
	int m_targetPlayheadPosition;
	int m_originalPlayheadPosition;
};
