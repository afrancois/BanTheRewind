// Includes
#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"
#include "cinder/CinderMath.h"
#include "Kiss.h"
#include "Resources.h"

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class KissFileSampleApp : public AppBasic 
{

public:

	// Cinder callbacks
	void draw();
	void quit();
	void setup();
	void update();

private:

	// Audio file
	audio::SourceRef mAudioSource;
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mBuffer;

	// Analyzer
	bool mFftInit;
	Kiss mFft;

};

// Draw
void KissFileSampleApp::draw()
{

	// Clear screen
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	// Check init flag
	if (mFftInit)
	{

		// Get data
		float * mFreqData = mFft.getAmplitude();
		float * mTimeData = mFft.getData();
		int32_t mDataSize = mFft.getBinSize();

		// Get dimensions
		float mScale = ((float)getWindowWidth() - 20.0f) / (float)mDataSize;
		float mWindowHeight = (float)getWindowHeight();

		// Use polylines to depict time and frequency domains
		PolyLine<Vec2f> mFreqLine;
		PolyLine<Vec2f> mTimeLine;

		// Iterate through data
		for (int32_t i = 0; i < mDataSize; i++) 
		{

			// Do logarithmic plotting for frequency domain
			double mLogSize = log((double)mDataSize);
			float x = (float)(log((double)i) / mLogSize) * (double)mDataSize;
			float y = math<float>::clamp(mFreqData[i] * (x / mDataSize) * log((double)(mDataSize - i)), 0.0f, 2.0f);

			// Plot points on lines
			mFreqLine.push_back(Vec2f(x * mScale + 10.0f,           -y * (mWindowHeight - 20.0f) * 1.25f + (mWindowHeight - 10.0f)));
			mTimeLine.push_back(Vec2f(i * mScale + 10.0f, mTimeData[i] * (mWindowHeight - 20.0f) * 0.3f  + (mWindowHeight * 0.15f + 10.0f)));

		}

		// Draw signals
		gl::draw(mFreqLine);
		gl::draw(mTimeLine);

	}

}

// Called on exit
void KissFileSampleApp::quit() 
{

	// Stop track
	mTrack->enablePcmBuffering(false);
	mTrack->stop();

}

// Set up
void KissFileSampleApp::setup()
{

	// Set up window
	setWindowSize(600, 600);

	// Set up OpenGL
	gl::enableAlphaBlending();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Set line color
	gl::color(Color(1, 1, 1));
	
	// Load and play audio
	mAudioSource = audio::load(loadResource(RES_SAMPLE));
	mTrack = audio::Output::addTrack(mAudioSource, false);
	mTrack->enablePcmBuffering(true);
	mTrack->play();

	// Set init flag
	mFftInit = false;

}

// Runs update logic
void KissFileSampleApp::update() 
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
					mFft.setData(mBuffer->getInterleavedData()->mData);

			}

		}

	}

}

// Start application
CINDER_APP_BASIC(KissFileSampleApp, RendererGl)
