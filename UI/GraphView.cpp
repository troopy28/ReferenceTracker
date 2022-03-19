#include "GraphView.h"

#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QTime>

GraphView::GraphView(Data::Document& document, QWidget* parent) :
	QWidget(parent),
	m_document(document),
	m_headerPixmap(width(), height()),
	m_curvesPixmap(),
	m_requireRedraw(true)
{
	connect(&m_document.GetVideo(), &Data::Video::FrameChanged, this, &GraphView::MovePlayhead);
	connect(&m_document.GetVideo(), &Data::Video::VideoLoaded, this, &GraphView::ForceRedraw);
}

void GraphView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	DrawHeader(painter);
	DrawCurves(painter);
	DrawPlayhead(painter);

	m_requireRedraw = false;
	// painter.setPen(Qt::NoPen);
	// painter.setBrush(QBrush(Qt::red));
	// painter.drawLine(QLineF(10, 0, 10, 100));
}

void GraphView::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	ForceRedraw();
}

void GraphView::MovePlayhead(int target)
{
	repaint();
	// todo: smooth transition from the current frame to the target frame. but later.
}

void GraphView::ForceRedraw()
{
	m_requireRedraw = true;
	repaint();
}

void GraphView::DrawHeader(QPainter& widgetPainter)
{
	static constexpr int headerHeight = 41;
	static constexpr int graduationsHeight = 15;
	static constexpr int smallGraduationHeight = graduationsHeight / 2;
	static constexpr int minimumMinigraduationSeparation = 3;
	static constexpr int minimumTextSeparation = 50;
	static constexpr QColor dark(35, 35, 35);
	static constexpr QColor normal(42, 42, 42);
	static constexpr QColor light(64, 64, 64);

	if(!m_requireRedraw)
	{
		widgetPainter.drawPixmap(0, 0, width(), height(), m_headerPixmap);
		return;
	}

	m_headerPixmap = m_headerPixmap.scaled(width(), height());
	m_headerPixmap.fill(Qt::transparent);
	QPainter pixmapPainter(&m_headerPixmap);

	QBrush brush;
	brush.setColor(normal);
	brush.setStyle(Qt::SolidPattern);
	pixmapPainter.setBrush(brush); // Brush is used to fill shapes.

	// 1. Draw the darker border.
	pixmapPainter.setPen(QPen(dark, 1));
	pixmapPainter.drawLine(0, headerHeight, width(), headerHeight);

	// 2. Draw the graduations.
	const Data::Video& video = m_document.GetVideo();
	pixmapPainter.setPen(light);

	int lastTextDrawX = -1000;
	int lastDrawnMinigraduation = -1000;
	int graduationIndex = 0;
	for (int frame = 0; frame < video.GetFrameCount(); frame++)
	{
		const int xPos = frameToControlPos(frame);
		if (xPos > width()) // Stop drawing when getting out of the control.
			break;

		// Small graduation, in the header.
		if(xPos - lastDrawnMinigraduation > minimumMinigraduationSeparation)
		{
			pixmapPainter.drawLine(xPos, 0, xPos, (graduationIndex & 1) == 0 ? graduationsHeight : smallGraduationHeight); // x % n^2 <=> x & (n^2 - 1) .... which MSVC apparently doesn't optimize automatically here.
			lastDrawnMinigraduation = xPos;
			graduationIndex++;
		}

		// Big graduation every second. Display the time too.
		if (frame % 24 == 0) // todo: make this 24 a parameter.
		{
			const int time = static_cast<int>(static_cast<float>(frame) / static_cast<float>(video.GetFrameRate()));
			const QString timeStr = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
			const QSize textSize = QFontMetrics(pixmapPainter.font()).size(Qt::TextSingleLine, timeStr);
			const int textXpos = xPos - textSize.width() / 2;
			if(textXpos - lastTextDrawX > minimumTextSeparation)
			{
				pixmapPainter.drawText(textXpos, headerHeight - textSize.height() + 3, timeStr);
				lastTextDrawX = xPos + textSize.width() / 2;
				pixmapPainter.drawLine(xPos, headerHeight, xPos, height());
			}
		}
	}

	widgetPainter.drawPixmap(0, 0, width(), height(), m_headerPixmap);
}

void GraphView::DrawCurves(QPainter& painter)
{
	// todo
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
