#include "TypeSafeSettings.h"

TypeSafeSettings::TypeSafeSettings() :
	m_settings("maxime_casas", "reference_tracker")
{
}

void TypeSafeSettings::SetMinimizedWidth(const int minimizedWidth)
{
	m_settings.setValue(MINIMZED_WIDTH, minimizedWidth);
}

int TypeSafeSettings::GetMinimizedWidth() const
{
	return m_settings.value(MINIMZED_WIDTH, 1280).toInt();
}

void TypeSafeSettings::SetMinimizeHeight(const int minimizedHeight)
{
	m_settings.setValue(MINIMZED_HEIGHT, minimizedHeight);
}

int TypeSafeSettings::GetMinimizedHeight() const
{
	return m_settings.value(MINIMZED_HEIGHT, 720).toInt();
}

void TypeSafeSettings::SetMaximized(const bool maximized)
{
	m_settings.setValue(IS_MAXIMIZED, maximized);
}

bool TypeSafeSettings::IsMaximized() const
{
	return m_settings.value(IS_MAXIMIZED, false).toBool();
}
