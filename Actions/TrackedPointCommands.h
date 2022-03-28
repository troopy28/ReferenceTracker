#pragma once

#include "../common.h"
#include <QLayout>
#include <QUndoCommand>
#include "../Data/Document.h"

class TrackedPointsList;

namespace Actions
{
	class CreateTrackedPointCommand final : public QUndoCommand
	{
	public:
		explicit CreateTrackedPointCommand(Data::Document& document);
		void redo() override;
		void undo() override;

	private:
		Data::Document& m_document;
	};

	class RemoveTrackedPointCommand final : public QUndoCommand
	{
	public:
		explicit RemoveTrackedPointCommand(Data::Document& document, int pointIndex);
		void redo() override;
		void undo() override;

	private:
		Data::Document& m_document;
		std::unique_ptr<Data::TrackedPoint> m_point;
	};

}
