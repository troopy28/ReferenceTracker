#pragma once

#include "../common.h"
#include <QSplitter>
#include <QStackedLayout>

class DynamicSplitter;

class ClickableSplitterHandle : public QSplitterHandle
{
	Q_OBJECT

public:
	explicit ClickableSplitterHandle(Qt::Orientation o, DynamicSplitter* parent = nullptr);
	~ClickableSplitterHandle() override = default;
	Q_DISABLE_COPY_MOVE(ClickableSplitterHandle);

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
	~DynamicSplitter() override = default;
	Q_DISABLE_COPY_MOVE(DynamicSplitter);
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
