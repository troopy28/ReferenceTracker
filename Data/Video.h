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
		_NODISCARD int GetHeight() const;
		_NODISCARD int GetFrameRate() const;
		_NODISCARD int GetFrameCount() const;
		_NODISCARD bool IsLoaded() const;
		_NODISCARD QString GetFilePath() const;

		/**
		 * \brief Tries loading the video at the specified path.
		 * \param path Path to the video file on the disk.
		 * \return Whether loading the video was a success or not.
		 */
		bool LoadFromFile(const QString& path);

		/**
		 * \brief Reads the video at a specified frame.
		 * \param index Index of the video frame to read.
		 */
		void ReadFrameAtIndex(const int& index);

	public slots:

		/**
		 * \brief Reads the video frame right after the last frame that
		 * was read.
		 */
		void ReadNextFrame(bool forceJump = false);

	signals:
		/**
		 * \brief Called when a new video frame gets read.
		 * \param frameIndex Index of the currently displayed video frame.
		 * \param isJump Whether the frame is the frame just after the previous one, or if there was a jump in the timeline.
		 */
		void FrameChanged(int frameIndex, bool isJump);
		/**
		 * \brief Called when a new video gets loaded from the disk.
		 */
		void VideoLoaded();

	private:
		/**
		 * \brief Path of the video file on the disk.
		 */
		QString m_filePath;
		/**
		 * \brief Content of the current video frame.
		 */
		cv::Mat m_frameMat;
		/**
		 * \brief Number of frames in the video.
		 */
		int m_frameCount;
		/**
		 * \brief Framerate of the video.
		 */
		int m_frameRate;
		/**
		 * \brief Index of the frame currently loaded.
		 */
		int m_currentFrameIndex;
		/**
		 * \brief Width of a video frame.
		 */
		int m_width;
		/**
		 * \brief Height of a video frame.
		 */
		int m_height;
		/**
		 * \brief OpenCV object used to load the frames from the video file into memory.
		 */
		cv::VideoCapture m_capture;
	};
}