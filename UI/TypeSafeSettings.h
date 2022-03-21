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

	void AddRecentProject(const QString& path);
	_NODISCARD QStringList GetRecentProjects() const;

private:
	void EnsureRecentVideosExist();
	void EnsureRecentProjectsExist();

	static const inline QString MINIMZED_WIDTH = "MINIMZED_WIDTH";
	static const inline QString MINIMZED_HEIGHT = "MINIMZED_HEIGHT";
	static const inline QString IS_MAXIMIZED = "IS_MAXIMIZED";
	static const inline QString RECENT_VIDEOS = "RECENT_VIDEOS";
	static const inline QString RECENT_PROJECTS = "RECENT_PROJECTS";
	QSettings m_settings;
};