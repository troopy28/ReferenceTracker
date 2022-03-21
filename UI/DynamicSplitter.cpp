#include "DynamicSplitter.h"

#include <QApplication>
#include <QDebug>
#include <algorithm>

// See https://forum.pythonguis.com/t/how-to-make-qsplitter-responds-to-double-clicking/480/2
// for the original idea behind the collapse system implemented in this file.

#pragma region ClickableSplitterHandle

ClickableSplitterHandle::ClickableSplitterHandle(const Qt::Orientation o, DynamicSplitter* parent)
	: QSplitterHandle(o, parent)
{

}

void ClickableSplitterHandle::mousePressEvent(QMouseEvent* event)
{
	const Qt::KeyboardModifiers pressedMods = QApplication::queryKeyboardModifiers();
	DynamicSplitter* splitter = dynamic_cast<DynamicSplitter*>(parent());
	if (splitter == nullptr)
		qWarning() << "Error : ClickableSplitterHandle's handle is not a ClickableSplitter.";
	else if (pressedMods & Qt::AltModifier)
		splitter->Rotate(); // If alt is clicked, then request a swap.
	else if (pressedMods & Qt::ControlModifier)
		splitter->Swap();
	else
		QSplitterHandle::mousePressEvent(event);
}

#pragma endregion



#pragma region ClickableSplitter

DynamicSplitter::DynamicSplitter(const Qt::Orientation o, QWidget* parent) :
	QSplitter(o, parent)
{
}

void DynamicSplitter::Rotate()
{
	const Qt::Orientation requestedOrientation = orientation() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;
	setOrientation(requestedOrientation);
}

void DynamicSplitter::Swap()
{
	const int widgetsCount = m_widgets.size();
	std::reverse(m_widgets.begin(), m_widgets.end());
	for(int i = 0; i < widgetsCount; i++)
	{
		insertWidget(i, m_widgets[i]);
	}
}

void DynamicSplitter::AddWidget(QWidget* widget)
{
	addWidget(widget);
	m_widgets.push_back(widget);
}


QSplitterHandle* DynamicSplitter::createHandle()
{
	return new ClickableSplitterHandle(orientation(), this);
}

#pragma endregion
