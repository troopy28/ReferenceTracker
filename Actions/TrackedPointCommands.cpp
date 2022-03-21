#include "TrackedPointCommands.h"

namespace Actions
{
	CreateTrackedPointCommand::CreateTrackedPointCommand(Data::Document& document) :
		m_document(document)
	{
	}

	void CreateTrackedPointCommand::redo()
	{
		m_document.CreateTrackedPoint();
		m_document.MarkDirty();
	}


	void CreateTrackedPointCommand::undo()
	{
		m_document.RemoveTrackedPoint(m_document.GetTrackedPoints().size() - 1);
		m_document.MarkDirty();
	}
}