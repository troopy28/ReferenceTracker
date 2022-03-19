#pragma once

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

}
