#pragma once

#include <opencv2/opencv.hpp>

#include <QObject>

namespace Data
{
	class Video final : public QObject
	{
		Q_OBJECT

	public:
		Video();
		~Video() override = default;
		Q_DISABLE_COPY(Video);
		Video(Video&& other) noexcept;
		Video& operator=(Video&& other) noexcept;

		void LoadVideo(const QString& path);
		void ReadNextFrame();
		void ReadFrameAtIndex(const int& index);

		_NODISCARD const cv::Mat& GetCurrentImage() const;
		_NODISCARD int GetCurrentFrameIndex() const;
		_NODISCARD int GetWidth() const;
		_NODISCARD int GetHeigth() const;
		_NODISCARD int GetFrameRate() const;

	private:
		QString m_filePath;
		cv::Mat m_frameMat;
		int m_frameCount;
		int m_frameRate;
		int m_currentFrameIndex;
		int m_width;
		int m_height;
		cv::VideoCapture m_capture;
	};
}