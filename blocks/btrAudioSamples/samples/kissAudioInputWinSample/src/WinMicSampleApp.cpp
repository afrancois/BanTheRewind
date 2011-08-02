// Includes
#include "AudioInput.h"
#include "cinder/app/AppBasic.h"
#include "Kiss.h"

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class WinMicSampleApp : public AppBasic 
{

public:

	// Cinder callbacks
	void draw();
	void quit();
	void setup();

	// Audio callback
	void onData(float * data, int32_t size);

private:

	// Audio input
	int32_t mCallbackID;
	AudioInput mInput;

	// Analyzer
	Kiss mFft;
	float * mFreqData;
	float * mTimeData;

};

// Draw
void WinMicSampleApp::draw()
{

	// Clear screen
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	// Get data
	if (mFreqData != NULL && mTimeData != NULL)
	{

		// Get data
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
			float x = (float)(log((double)i) / mLogSize) * (float)mDataSize;
			float y = math<float>::clamp(mFreqData[i] * (x / mDataSize) * (float)log((double)(mDataSize - i)), 0.0f, 2.0f);

			// Plot points on lines
			mFreqLine.push_back(Vec2f(x * mScale + 10.0f,           -y * (mWindowHeight - 20.0f) * 0.25f + (mWindowHeight - 10.0f)));
			mTimeLine.push_back(Vec2f(i * mScale + 10.0f, mTimeData[i] * (mWindowHeight - 20.0f) * 0.25f + (mWindowHeight * 0.25f + 10.0f)));

		}

		// Draw signals
		gl::draw(mFreqLine);
		gl::draw(mTimeLine);

	}

}

// Called when buffer is full
void WinMicSampleApp::onData(float * data, int32_t size)
{

	// Analyze data
	mFft.setData(data);
	mFreqData = mFft.getAmplitude();
	mTimeData = mFft.getData();

}

// Called on exit
void WinMicSampleApp::quit()
{

	// Stop input
	mInput.removeCallback(mCallbackID);
	mInput.stop();

	// Free resources
	if (mFreqData != NULL)
		delete [] mFreqData;
	if (mTimeData != NULL)
		delete [] mTimeData;

}

// Set up
void WinMicSampleApp::setup()
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

	// Initialize arrays
	mFreqData = NULL;
	mTimeData = NULL;

	DeviceList devices = mInput.getDeviceList();
	for (DeviceList::const_iterator deviceIt = devices.cbegin(); deviceIt != devices.cend(); ++deviceIt)
	{
		OutputDebugStringA(deviceIt->second.c_str());
		OutputDebugStringA("\n");
	}

	// Start receiving audio
	mCallbackID = mInput.addCallback<WinMicSampleApp>(&WinMicSampleApp::onData, this);
	mInput.start();

}

// Start application
CINDER_APP_BASIC(WinMicSampleApp, RendererGl)
