#include "TypeSafeSettings.h"
#include <QFile>

TypeSafeSettings::TypeSafeSettings() :
	m_settings("maxime_casas", "reference_tracker")
{
	EnsureRecentVideosExist();
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

void TypeSafeSettings::AddRecentVideo(const QString& path)
{
	QStringList recentVids = GetRecentVideos();
	if (recentVids.contains(path))
		recentVids.removeAll(path);
	recentVids.insert(0, path);
	m_settings.setValue(RECENT_VIDEOS, recentVids);
}

QStringList TypeSafeSettings::GetRecentVideos() const
{
	return m_settings.value(RECENT_VIDEOS, QStringList()).toStringList();
}

void TypeSafeSettings::EnsureRecentVideosExist()
{
	QStringList recentVids = GetRecentVideos();
	for(const QString& path : recentVids)
	{
		if (!QFile::exists(path))
			recentVids.removeAll(path);
	}
	m_settings.setValue(RECENT_VIDEOS, recentVids);
}
