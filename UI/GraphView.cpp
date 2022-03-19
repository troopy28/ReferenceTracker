#include "GraphView.h"

#include <QPainter>
#include <QPainterPath>
#include <QTime>

GraphView::GraphView(Data::Document& document, QWidget* parent) :
	QWidget(parent),
	m_document(document)
{
	connect(&m_document.GetVideo(), &Data::Video::FrameChanged, this, &GraphView::MovePlayhead);
}

void GraphView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	DrawHeader(painter);
	DrawCurves(painter);
	DrawPlayhead(painter);

	// painter.setPen(Qt::NoPen);
	// painter.setBrush(QBrush(Qt::red));
	// painter.drawLine(QLineF(10, 0, 10, 100));
}

void GraphView::MovePlayhead(int target)
{
	repaint();
	// todo: smooth transition from the current frame to the target frame. but later.
}

void GraphView::DrawHeader(QPainter& painter) const
{
	static constexpr int headerHeight = 41;
	static constexpr int graduationsHeight = 15;
	static constexpr int smallGraduationHeight = graduationsHeight / 2;
	static constexpr QColor dark(35, 35, 35);
	static constexpr QColor normal(42, 42, 42);
	static constexpr QColor light(64, 64, 64);

	painter.save();

	QBrush brush;
	brush.setColor(normal);
	brush.setStyle(Qt::SolidPattern);
	painter.setBrush(brush); // Brush is used to fill shapes.

	// 1. Draw the darker border.
	painter.setPen(QPen(dark, 1));
	painter.drawLine(0, headerHeight, width(), headerHeight);

	// 2. Draw the graduations.
	const Data::Video& video = m_document.GetVideo();
	painter.setPen(light);
	for (int frame = 0; frame < video.GetFrameCount(); frame++)
	{
		const int xPos = frameToControlPos(frame);
		if (xPos > width()) // Stop drawing when getting out of the control.
			break;

		// Small graduation, in the header.
		painter.drawLine(xPos, 0, xPos, (frame & 1) == 0 ? graduationsHeight : smallGraduationHeight); // x % n^2 <=> x & (n^2 - 1) .... which MSVC apparently doesn't optimize automatically here.

		// Big graduation every second. Display the time too.
		if (frame % 24 == 0) // todo: make this 24 a parameter.
		{
			const int time = static_cast<int>(static_cast<float>(frame) / static_cast<float>(video.GetFrameRate()));
			const QString timeStr = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
			const QSize textSize = QFontMetrics(painter.font()).size(Qt::TextSingleLine, timeStr);
			painter.drawText(xPos - textSize.width() / 2, headerHeight - textSize.height() + 3, timeStr);
			painter.drawLine(xPos, headerHeight, xPos, height());
		}
	}

	painter.restore();
}

void GraphView::DrawCurves(QPainter& painter)
{
}

void GraphView::DrawPlayhead(QPainter& painter) const
{
	static constexpr int halfWidth = 7;
	static constexpr int h = 8;
	static constexpr int hTip = 12;
	static constexpr QColor playheadColor(230, 75, 61);

	const int currentFrame = m_document.GetVideo().GetCurrentFrameIndex();
	const int position = frameToControlPos(currentFrame);

	painter.save();

	QBrush brush;
	brush.setColor(playheadColor);
	brush.setStyle(Qt::SolidPattern);

	QPainterPath path;
	QPolygon poly;
	poly
		<< QPoint(-halfWidth, 0)
		<< QPoint(halfWidth, 0)
		<< QPoint(halfWidth, h)
		<< QPoint(0, hTip)
		<< QPoint(-halfWidth, h);
	path.addPolygon(poly);


	painter.setBrush(brush); // Brush is used to fill shapes.
	painter.setPen(playheadColor); // Pen is used to draw lines.
	painter.translate(position, 0);
	painter.fillPath(path, brush);
	painter.drawLine(0, hTip, 0, height());

	painter.restore();
}

int GraphView::frameToControlPos(const int frame) const
{
	return m_document.GetVideo().IsLoaded()
		? static_cast<int>(static_cast<float>(width() * (frame - 1)) / static_cast<float>(m_document.GetVideo().GetFrameCount() - 2))
		: 1;
}
