#pragma once

#include <QSettings>

class TypeSafeSettings
{
public:
	TypeSafeSettings();

	void SetMinimizedWidth(int minimizedWidth);
	_NODISCARD int GetMinimizedWidth() const;

	void SetMinimizeHeight(int minimizedHeight);
	_NODISCARD int GetMinimizedHeight() const;

	void SetMaximized(bool maximized);
	_NODISCARD bool IsMaximized() const;

	void AddRecentVideo(const QString& path);
	_NODISCARD QStringList GetRecentVideos() const;

private:
	void EnsureRecentVideosExist();

	static const inline QString MINIMZED_WIDTH = "MINIMZED_WIDTH";
	static const inline QString MINIMZED_HEIGHT = "MINIMZED_HEIGHT";
	static const inline QString IS_MAXIMIZED = "IS_MAXIMIZED";
	static const inline QString RECENT_VIDEOS = "RECENT_VIDEOS";
	QSettings m_settings;
};