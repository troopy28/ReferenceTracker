#include "Video.h"
#include <QFile>
#include <QDebug>
#include <QDataStream>

namespace Data
{
	Video::Video() :
		m_filePath(),
		m_frameMat(720, 1280, CV_8UC3, cv::Scalar(0.0, 0.0, 0.0)),
		m_frameCount(0),
		m_frameRate(-1),
		m_currentFrameIndex(0),
		m_width(1280),
		m_height(720),
		m_capture()
	{
		m_frameMat.setTo(cv::Scalar(0.0, 0.0, 0.0));
	}

	Video::Video(Video&& other) noexcept :
		m_filePath(std::move(other.m_filePath)),
		m_frameMat(std::move(other.m_frameMat)),
		m_frameCount(other.m_frameCount),
		m_frameRate(other.m_frameRate),
		m_currentFrameIndex(other.m_currentFrameIndex),
		m_width(other.m_width),
		m_height(other.m_height),
		m_capture(other.m_capture)
	{
	}

	Video& Video::operator=(Video&& other) noexcept
	{
		m_filePath = std::move(other.m_filePath);
		m_frameMat = std::move(other.m_frameMat);
		m_frameCount = other.m_frameCount;
		m_frameRate = other.m_frameRate;
		m_currentFrameIndex = other.m_currentFrameIndex;
		m_width = other.m_width;
		m_height = other.m_height;
		m_capture = other.m_capture;
		return *this;
	}

	bool Video::LoadFromFile(const QString& path)
	{
		// 1. Before changing anything, ensure the file at the specified path exists.
		if (!QFile::exists(path))
		{
			qWarning() << "Video at" << path << "does not exist.";
			return false;
		}

		// 2. Try loading the video on a dummy capture to see if it works.
		cv::VideoCapture testCapture;
		testCapture.open(path.toStdString());
		if (!testCapture.isOpened())
		{
			qWarning() << "Could not load the video at" << path << "(but the file does exist -- make sure it is a video).";
			return false;
		}

		// 3. Reset all the members of the class: if loading the video fails despite the previous test,
		// this class must hold no data related to the previous video.
		m_filePath = QString();
		m_currentFrameIndex = 0;
		m_frameRate = 0;
		m_frameCount = 0;
		if (m_capture.isOpened())
			m_capture.release();
		m_frameMat.setTo(cv::Scalar(0.0, 0.0, 0.0));

		// 4. Load the video.
		m_capture.open(path.toStdString());
		if (m_capture.isOpened())
		{
			m_frameRate = static_cast<int>(m_capture.get(cv::CAP_PROP_FPS));
			m_frameCount = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_COUNT));
			m_width = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_WIDTH));
			m_height = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
		}
		else
		{
			qWarning() << "Could not load the video at" << path << "(but the file does exist -- make sure it is a video).";
			return false;
		}
		m_filePath = path;
		emit VideoLoaded();

		// 5. Load the first frame of the video.
		ReadFrameAtIndex(0);
		return true;
	}

	void Video::ReadNextFrame()
	{
		const int nextIndex = m_currentFrameIndex + 1;
		// If we are already at the last frame: do nothing.
		if (nextIndex >= m_frameCount)
		{
			return;
		}

		// Read the next frame and increment the counter.
		m_capture >> m_frameMat;
		m_currentFrameIndex++;

		emit FrameChanged(m_currentFrameIndex, false);
	}

	void Video::ReadFrameAtIndex(const int& index)
	{
		// 1. Clamp the index.
		const int clampedIndex = std::clamp(index, 0, m_frameCount);

		// 2. Special case: clampedIndex is current +1.
		if (clampedIndex == m_currentFrameIndex + 1)
		{
			ReadNextFrame();
		}
		// 3. General case: read an arbitrary frame.
		// This includes the case where we want to re-read the current frame
		// (this is needed for instance for the first frame read.)
		else
		{
			m_capture.set(cv::CAP_PROP_POS_FRAMES, clampedIndex);
			m_capture >> m_frameMat;
			m_currentFrameIndex = clampedIndex;
			emit FrameChanged(m_currentFrameIndex, true);
		}
	}

	const cv::Mat& Video::GetCurrentImage() const
	{
		return m_frameMat;
	}

	int Video::GetCurrentFrameIndex() const
	{
		return m_currentFrameIndex;
	}

	int Video::GetWidth() const
	{
		return m_width;
	}

	int Video::GetHeigth() const
	{
		return m_height;
	}

	int Video::GetFrameRate() const
	{
		return m_frameRate;
	}

	int Video::GetFrameCount() const
	{
		return m_frameCount;
	}

	bool Video::IsLoaded() const
	{
		return m_frameCount != 0;
	}

	QString Video::GetFilePath() const
	{
		return m_filePath;
	}
}
