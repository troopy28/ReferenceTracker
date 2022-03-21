#include "GraphView.h"

#include <qevent.h>
#include <QTimeLine>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QDebug>

template<typename T>
T Abs(const T a)
{
	return a < T(0) ? -a : a;
}

template<typename T>
T Lerp(const T a, const T b, const double f)
{
	return static_cast<T>(static_cast<double>(a) + f * static_cast<double>(b - a));
}

GraphView::GraphView(Data::Document& document, QWidget* parent) :
	QWidget(parent),
	m_document(document),
	m_headerPixmap(width(), height()),
	m_curvesPixmap(),
	m_requireRedraw(true),
	m_playheadPosition(0),
	m_movingPlayhead(false),
	m_targetPlayheadPosition(0),
	m_originalPlayheadPosition(0)
{
	connect(&m_document.GetVideo(), &Data::Video::FrameChanged, this, &GraphView::MovePlayheadToFrame);
	connect(&m_document.GetVideo(), &Data::Video::VideoLoaded, this, &GraphView::ForceRedraw);
}



void GraphView::resizeEvent(QResizeEvent* evt)
{
	QWidget::resizeEvent(evt);
	ForceRedraw();
}

void GraphView::mousePressEvent(QMouseEvent* evt)
{
	constexpr int selectionArea = 3;

	if (!m_document.GetVideo().IsLoaded())
		return;

	const int clickedPos = static_cast<int>(evt->localPos().x());
	if (Abs(clickedPos - m_playheadPosition) <= selectionArea)
	{
		m_movingPlayhead = true;
		this->setCursor(Qt::SplitHCursor);
		repaint();
	}
}

void GraphView::mouseMoveEvent(QMouseEvent* evt)
{
	if (!m_movingPlayhead)
		return;

	Data::Video& video = m_document.GetVideo();

	m_playheadPosition = std::clamp(static_cast<int>(evt->localPos().x()), 0, width());
	const int correspondingFrame = controlPosToFrame(m_playheadPosition);
	if (correspondingFrame != video.GetCurrentFrameIndex())
		video.ReadFrameAtIndex(correspondingFrame);
	repaint();
}

void GraphView::mouseReleaseEvent(QMouseEvent* evt)
{
	if (!m_movingPlayhead)
		return;

	m_movingPlayhead = false;
	this->setCursor(Qt::ArrowCursor);
	MovePlayheadToFrame(m_document.GetVideo().GetCurrentFrameIndex()); // Put the frame indicator at an actual, integer frame position.
}

void GraphView::MovePlayheadToFrame(const int frame, const bool instantaneous)
{
	// If the frame is changed by the user moving the cursor, do nothing.
	if (m_movingPlayhead)
		return;

	// Below: the frame is changed by something external to this control (video player for instance).


	// Instantaneous playhead displacement.
	if (instantaneous)
	{
		m_targetPlayheadPosition = frameToControlPos(frame);
		m_playheadPosition = m_targetPlayheadPosition;
		m_originalPlayheadPosition = m_targetPlayheadPosition;
		repaint();
		return;
	}

	// Start a smooth playhead motion.

	m_originalPlayheadPosition = m_playheadPosition;
	m_targetPlayheadPosition = frameToControlPos(frame);
	QTimeLine* anim = new QTimeLine(1000 / m_document.GetVideo().GetFrameRate(), this);
	anim->setUpdateInterval(100 / m_document.GetVideo().GetFrameRate());
	connect(anim, &QTimeLine::valueChanged, this, &GraphView::SmoothPlayheadMove);
	connect(anim, &QTimeLine::finished, this, [frame, this] {MovePlayheadToFrame(frame, true); });
	anim->start();

	repaint();
	// todo: smooth transition from the current frame to the target frame. but later.
}

void GraphView::SmoothPlayheadMove(const double x)
{
	m_playheadPosition = Lerp(m_originalPlayheadPosition, m_targetPlayheadPosition, x);
	repaint();
}

#pragma region Drawing

void GraphView::paintEvent(QPaintEvent* evt)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	DrawHeader(painter);
	DrawCurves(painter);
	DrawPlayhead(painter);

	m_requireRedraw = false;
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

	if (!m_requireRedraw)
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
		if (xPos - lastDrawnMinigraduation > minimumMinigraduationSeparation)
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
			if (textXpos - lastTextDrawX > minimumTextSeparation)
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
	static constexpr QColor unselectedPlayheadColor(230, 75, 61);
	static constexpr QColor selectedPlayheadColor(255, 100, 70);

	painter.save();

	const QColor color = m_movingPlayhead ? selectedPlayheadColor : unselectedPlayheadColor;

	QBrush brush;
	brush.setColor(color);
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
	painter.setPen(color); // Pen is used to draw lines.
	painter.translate(m_playheadPosition, 0);
	painter.fillPath(path, brush);
	painter.drawLine(0, hTip, 0, height());

	painter.restore();
}

#pragma endregion

int GraphView::frameToControlPos(const int frame) const
{
	return m_document.GetVideo().IsLoaded()
		? static_cast<int>(static_cast<float>(width() * (frame)) / static_cast<float>(m_document.GetVideo().GetFrameCount() - 1))
		: 1;
}

int GraphView::controlPosToFrame(const int controlPos) const
{
	return m_document.GetVideo().IsLoaded()
		? static_cast<int>(static_cast<float>((m_document.GetVideo().GetFrameCount() - 1) * controlPos) / static_cast<float>(width()))
		: 0;
}
