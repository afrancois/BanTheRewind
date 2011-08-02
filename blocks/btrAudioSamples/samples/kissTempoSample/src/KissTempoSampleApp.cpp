// Includes
#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"
#include "cinder/CinderMath.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "TextField.h"
#include "Kiss.h"
#include "Resources.h"

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class KissTempoSampleApp : public AppBasic 
{

public:

	// Cinder callbacks
	void draw();
	void quit();
	void setup();
	void update();

private:

	// Set default neighbor count
	static const int32_t DEFAULT_NEIGHBOR_COUNT = 6;

	// Audio file
	audio::SourceRef mAudioSource;
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mBuffer;

	// Analyzer
	bool mFftInit;
	Kiss mFft;

	// Data
	int32_t mDataSize;
	int32_t mInputSize;
	float * mInputData;
	float * mTimeData;

	// Peak information
	double mAdjustTime;
	int32_t mFirstPeak;
	int32_t mNeighbors;
	vector<int32_t> mPeakDistances;
	int32_t mSampleDistance;
	stringstream mStringStream;
	float mTempo;
	TextField mText;
	float mThreshold;

	// Waveform drawing data
	vector<float> mWaveform;

};

// Draw
void KissTempoSampleApp::draw()
{

	// Clear screen
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	// Check sizes
	if (mDataSize > 0 && mWaveform.size() > 0)
	{

		// Get dimensions
		float mWindowWidth = (float)getWindowWidth();
		float mCenter = mWindowWidth * 0.5f;

		// Draw waveform
		float y = 0.0f;
		PolyLine<Vec2f> mLine;
		for (vector<float>::const_iterator i = mWaveform.begin(); i != mWaveform.end(); ++i, y += 6.5f)
		{
			float x = (* i) * mWindowWidth * 0.25f;
			mLine.push_back(Vec2f(mCenter + x, y));
			mLine.push_back(Vec2f(mCenter - x, y + 3.25f));
		}
		gl::draw(mLine);

	}

	// Draw tempo
	mStringStream.str("");
	mStringStream << mTempo << " bpm";
	mText.str(mStringStream.str());
	gl::draw(mText.getTexture(), mText.getBounds());

	// Same frame
	//writeImage(getAppPath() + "frames/frame_" + to_string((long double)getElapsedFrames()) + ".png", copyWindowSurface());

}

// Called on exit
void KissTempoSampleApp::quit() 
{

	// Stop track
	mTrack->enablePcmBuffering(false);
	mTrack->stop();

	// Clean up
	delete [] mInputData;
	delete [] mTimeData;

}

// Set up
void KissTempoSampleApp::setup()
{

	// Set up window
	setWindowSize(720, 405);

	// Set up OpenGL
	gl::enableAlphaBlending();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Set line color
	gl::color(Color(1, 1, 1));

	// Set low pass filter on FFT to calculate tempo based on beats
	mFft.setFilter(0.1f);

	// Set font
	mText = TextField(Vec2i(265, 300), Font(loadResource(RES_FONT), 32));

	// Initialize values
	mAdjustTime = getElapsedSeconds();
	mDataSize = 0;
	mFirstPeak = -1;
	mInputSize = 0;
	mNeighbors = DEFAULT_NEIGHBOR_COUNT;
	mSampleDistance = 0;
	mTempo = 0.0f;
	mThreshold = 0.1f;

	// Load and play audio
	mAudioSource = audio::load(loadResource(RES_SAMPLE));
	mTrack = audio::Output::addTrack(mAudioSource, false);
	mTrack->enablePcmBuffering(true);
	mTrack->play();

}

// Runs update logic
void KissTempoSampleApp::update() 
{

	// Check if track is playing and has a PCM buffer available
	if (mTrack->isPlaying() && mTrack->isPcmBuffering())
	{

		// Get buffer
		mBuffer = mTrack->getPcmBuffer();
		if (mBuffer && mBuffer->getInterleavedData())
		{

			// Get sample count
			uint32_t mSampleCount = mBuffer->getInterleavedData()->mSampleCount;
			if (mSampleCount > 0)
			{

				// Initialize analyzer, if needed
				if (!mFftInit)
				{
					mFftInit = true;
					mFft.setDataSize(mSampleCount);
				}

				// Analyze data
				if (mBuffer->getInterleavedData()->mData != 0) 
				{
					mInputData = mBuffer->getInterleavedData()->mData;
					mInputSize = mBuffer->getInterleavedData()->mSampleCount;
					mFft.setData(mInputData);
				}

				// Get data
				mTimeData = mFft.getData();
				mDataSize = mFft.getBinSize();

				// Iterate through amplitude data
				for (int32_t i = 0; i < mDataSize; i++, mSampleDistance++) 
				{

					// Check value against threshold
					if (mTimeData[i] >= mThreshold)
					{

						// Determine neighbor tange
						int32_t mStart = max<int32_t>(i - mNeighbors, 0);
						int32_t mEnd = min<int32_t>(i + mNeighbors, mDataSize - 1);

						// Compare this value with neighbors to find peak
						bool mPeak = true;
						for (int32_t j = mStart; j < mEnd; j++)
							if (j != i && mTimeData[i] < mTimeData[j]) mPeak = false;

						// This is a peak
						if (mPeak)
						{

							// Add distance between this peak and last into the 
							// list. Just note position if this is the first peak.
							if (mFirstPeak >= 0)
								mPeakDistances.push_back(mSampleDistance);
							else
								mFirstPeak = mSampleDistance;
							
							// Reset distance counter
							mSampleDistance = 0;

						}

					}

				}

			}

		}

		// We have multiple peaks to compare
		if (mPeakDistances.size() > 1)
		{

			// Add distances
			int32_t mTotal = 0;
			for (vector<int32_t>::const_iterator i = mPeakDistances.begin(); i != mPeakDistances.end(); ++i)
				mTotal += (* i);

			// Determine tempo based on average peak distance
			mTempo = (float)(44100 / (mTotal / mPeakDistances.size())) * 60.0f / 1000.0f;

		}

		// Get current window height
		int32_t mWindowHeight = getWindowHeight() / 8;

		// Add up values, combine input and filtered values
		// to emphasize bass
		float mTotal = 0.0f;
		for (int32_t i = 0; i < mDataSize; i++) 
			if (i * 8 < mInputSize)
				mTotal += mTimeData[i] * 2.0f * mInputData[i * 8];

		// Add average to drawing line
		mWaveform.push_back(mTotal / (float)mDataSize);

		// Remove points when vector is wider than screen
		while (mWaveform.size() >= mWindowHeight)
			mWaveform.erase(mWaveform.begin());

	}

	// Adjust neighbor count to stay in range
	if (getElapsedSeconds() - mAdjustTime > 1.0)
	{
		if (mTempo >= 150.0f) 
			mNeighbors++;
		else if (mTempo <= 110.0f) 
			mNeighbors--;
		else
			mNeighbors = DEFAULT_NEIGHBOR_COUNT;
		mNeighbors = max<int32_t>(min<int32_t>(mNeighbors, 15), 1);
		mAdjustTime = getElapsedSeconds();
	}

}

// Start application
CINDER_APP_BASIC(KissTempoSampleApp, RendererGl)
