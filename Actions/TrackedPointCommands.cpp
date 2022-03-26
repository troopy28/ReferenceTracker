#include "TrackedPointCommands.h"

namespace Actions
{
#pragma region CreateTrackedPointCommand

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
		m_document.RemoveTrackedPoint(static_cast<int>(m_document.GetTrackedPoints().size()) - 1);
		m_document.MarkDirty();
	}


#pragma endregion

#pragma region CreateTrackedPointCommand

	RemoveTrackedPointCommand::RemoveTrackedPointCommand(Data::Document& document, const int pointIndex) :
		m_document(document),
		m_point(m_document.GetTrackedPoint(pointIndex).GetCopy())
	{
	}

	void RemoveTrackedPointCommand::redo()
	{
		m_document.RemoveTrackedPoint(m_point->GetPointIndex());
	}

	void RemoveTrackedPointCommand::undo()
	{
		m_document.InsertTrackedPoint(std::move(m_point));
	}


#pragma endregion
}
