// Includes
#include <AudioInput.h>
#include <cinder/app/AppBasic.h>

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class BasicSampleApp : public AppBasic 
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
	AudioInput mInput;
	float * mData;

};

// Draw
void BasicSampleApp::draw()
{

	// Clear screen
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	// Get data
	if (mData != NULL)
	{

		// Get size of data
		int32_t mDataSize = mInput.getDataSize();

		// Get dimensions
		float mScale = ((float)getWindowWidth() - 20.0f) / (float)mDataSize;
		float mWindowHeight = (float)getWindowHeight();

		// Use polyline to depict audio
		PolyLine<Vec2f> mLine;

		// Iterate through data and populate line
		for (int32_t i = 0; i < mDataSize; i++) 
			mLine.push_back(Vec2f(i * mScale + 10.0f, mData[i] * (mWindowHeight - 20.0f) * 0.25f + (mWindowHeight * 0.5f + 10.0f)));

		// Draw signal
		gl::draw(mLine);

	}

}

// Called when buffer is full
void BasicSampleApp::onData(float * data, int32_t size)
{

	// Get data
	mData = data;

}

// Called on exit
void BasicSampleApp::quit()
{

	// Stop input
	mInput.stop();

	// Free resources
	if (mData != NULL)
		delete [] mData;

}

// Set up
void BasicSampleApp::setup()
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

	// Initialize array
	mData = NULL;

	// Start receiving audio
	mInput.addCallback<BasicSampleApp>(&BasicSampleApp::onData, this);
	mInput.start();

}

// Start application
CINDER_APP_BASIC(BasicSampleApp, RendererGl)
