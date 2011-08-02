// Includes
#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/CinderMath.h"
#include "Kiss.h"

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class KissBasicSampleApp : public AppBasic 
{

public:

	// Cinder callbacks
	void setup();
	void mouseMove(MouseEvent event);
	void sineWave(uint64_t inSampleOffset, uint32_t ioSampleCount, audio::Buffer32f * ioBuffer);
	void draw();

private:

	// Audio generation settings
	float mAmplitude;
	float mFreqTarget;
	float mPhase;
	float mPhaseAdjust;
	float mMaxFreq;
	float mMinFreq;

	// Analyzer
	bool mFftInit;
	Kiss mFft;

};

// Draw
void KissBasicSampleApp::draw()
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
			mFreqLine.push_back(Vec2f(x * mScale + 10.0f,           -y * (mWindowHeight - 20.0f) * 0.25f + (mWindowHeight - 10.0f)));
			mTimeLine.push_back(Vec2f(i * mScale + 10.0f, mTimeData[i] * (mWindowHeight - 20.0f) * 0.25f + (mWindowHeight * 0.25 + 10.0f)));

		}

		// Draw signals
		gl::draw(mFreqLine);
		gl::draw(mTimeLine);

	}

}

// Handles mouse movement
void KissBasicSampleApp::mouseMove(MouseEvent event)
{

	// Change frequency and amplitude based on mouse position
	// Scale everything logarithmically to get a better feel and sound
	mAmplitude = 1.0f - event.getY() / (float)getWindowHeight();
	double width = (double)getWindowWidth();
	double x = width - (double)event.getX();
	float mPosition = (float)((log(width) - log(x)) / log(width));
	mFreqTarget = math<float>::clamp(mMaxFreq * mPosition, mMinFreq, mMaxFreq);
	mAmplitude = math<float>::clamp(mAmplitude * (1.0f - mPosition), 0.05f, 1.0f);

}

// Set up
void KissBasicSampleApp::setup()
{

	// Set up window
	setFrameRate(60);
	setWindowSize(600, 600);

	// Set up OpenGL
	gl::enableAlphaBlending();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Set line color
	gl::color(Color(1, 1, 1));

	// Set synth properties
	mAmplitude = 0.5f;
	mMaxFreq = 20000.0f;
	mMinFreq = 1.0f;
	mFreqTarget = 0.0f;
	mPhase = 0.0f;
	mPhaseAdjust = 0.0f;
	
	// Play sine
	audio::Output::play(audio::createCallback(this, &KissBasicSampleApp::sineWave));

	// Set init flag
	mFftInit = false;

}

// Creates sine wave
void KissBasicSampleApp::sineWave(uint64_t inSampleOffset, uint32_t ioSampleCount, audio::Buffer32f * ioBuffer) 
{

	// Fill buffer with sine wave
	mPhaseAdjust = mPhaseAdjust * 0.95f + (mFreqTarget / 44100.0f) * 0.05f;
	for (int32_t i = 0; i < ioSampleCount; i++ ) 
	{
		mPhase += mPhaseAdjust;
		mPhase = mPhase - math<float>::floor(mPhase);
		float val = math<float>::sin(mPhase * 2.0f * M_PI) * mAmplitude;	
		ioBuffer->mData[i * ioBuffer->mNumberChannels] = val;
		ioBuffer->mData[i * ioBuffer->mNumberChannels + 1] = val;
	}

	// Initialize analyzer
	if (!mFftInit)
	{
		mFftInit = true;
		mFft.setDataSize(ioSampleCount);
	}

	// Analyze data
	mFft.setData(ioBuffer->mData);

}

// Start application
CINDER_APP_BASIC(KissBasicSampleApp, RendererGl)
