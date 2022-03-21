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
		
		_NODISCARD const cv::Mat& GetCurrentImage() const;
		_NODISCARD int GetCurrentFrameIndex() const;
		_NODISCARD int GetWidth() const;
		_NODISCARD int GetHeigth() const;
		_NODISCARD int GetFrameRate() const;
		_NODISCARD int GetFrameCount() const;
		_NODISCARD bool IsLoaded() const;

		bool LoadFromFile(const QString& path);
		void ReadFrameAtIndex(const int& index);

	public slots:
		void ReadNextFrame();

	signals:
		/**
		 * \brief Called when a new video frame gets read.
		 * \param frameIndex Index of the currently displayed video frame.
		 * \param isJump Whether the frame is the frame just after the previous one, or if there was a jump in the timeline.
		 */
		void FrameChanged(int frameIndex, bool isJump);
		void VideoLoaded();

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