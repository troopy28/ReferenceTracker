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
	}


	void CreateTrackedPointCommand::undo()
	{
		m_document.RemoveTrackedPoint(m_document.GetTrackedPoints().size() - 1);
	}
}