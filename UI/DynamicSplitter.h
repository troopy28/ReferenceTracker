#pragma once

#include <QSplitter>
#include <QStackedLayout>

class DynamicSplitter;

class ClickableSplitterHandle : public QSplitterHandle
{
	Q_OBJECT

public:
	explicit ClickableSplitterHandle(Qt::Orientation o, DynamicSplitter* parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent* event) override;
};

/**
 * \brief Custom splitter class. Used only to override the handle.
 */
class DynamicSplitter : public QSplitter
{
	Q_OBJECT

public:
	explicit DynamicSplitter(Qt::Orientation o, QWidget* parent = nullptr);
	void Rotate();
	void Swap();
	void Collapse(int collapsedIndex);

	void AddWidget(QWidget* widget);

protected:
	/**
	 * \brief Override this function to use our own ClickableSplitterHandle instead
	 * of the default QSplitterHandle.
	 * \return The handle.
	 */
	QSplitterHandle* createHandle() override;

private:
	QList<QWidget*> m_widgets;
};
