// Includes
#include <AudioInput.h>
#include <cinder/app/AppBasic.h>
#include <fstream>
#include <iostream>

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class WavWriterSampleApp : public AppBasic 
{

public:

	// Cinder callbacks
	void draw();
	void keyUp(KeyEvent event);
	void quit();
	void setup();
	
	// Receives input
	void onData(float * data, int32_t size);

	// Writes PCM buffer to file
	void writeData();

private:

	// Audio input
	AudioInput mInput;
	float * mData;

	// File writing
	ofstream mFile;
	AudioInput::WAVFILEHEADER mFileHeader;
	int16_t * mPcmBuffer;
	int32_t mPcmBufferPosition;
	int32_t mPcmBufferSize;
	uint32_t mPcmTotalSize;
	bool mRecording;

	// WAV format
	int32_t mBitsPerSample;
	int32_t mChannelCount;
	int32_t mSampleRate;

};

// Draw
void WavWriterSampleApp::draw()
{

	// Clear screen
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	// Get data
	if (mData != 0)
	{

		// Get size of data
		int32_t mDataSize = mInput.getDataSize();

		// Get dimensions
		float mScale = ((float)getWindowWidth() - 20.0f) / (float)mDataSize;
		float mWindowHeight = (float)getWindowHeight();

		// Use polyline to depict audio
		PolyLine<Vec2f> mLine;

		// Set line color
		if (mRecording)
			gl::color(Color(1, 0, 0));
		else
			gl::color(Color(1, 1, 1));

		// Iterate through data and populate line
		for (int32_t i = 0; i < mDataSize; i++) 
			mLine.push_back(Vec2f(i * mScale + 10.0f, mData[i] * (mWindowHeight - 20.0f) * 0.25f + (mWindowHeight * 0.5f + 10.0f)));

		// Draw signal
		gl::draw(mLine);

	}

}

// Handles key press
void WavWriterSampleApp::keyUp(KeyEvent event)
{

	// Toggle recording with space bar
	if (event.getCode() == KeyEvent::KEY_SPACE)
	{

		// Currently recording
		if (mRecording)
		{

			// Save remaining data
			if (mPcmBufferSize > 0) 
				writeData();

			// Close file
			if (mFile.is_open())
			{
				mFile.flush();
				mFile.close();
			}

		}
		else
		{

			// Set size values
			mPcmBufferPosition = 0;
			mPcmTotalSize = 0;

			// Open file for streaming
			mFile.open(getAppPath() + "output.wav", ios::binary|ios::trunc);

		}

		// Toggle recording flag
		mRecording = !mRecording;

	}

}

// Called when buffer is full
void WavWriterSampleApp::onData(float * data, int32_t size)
{

	// Get float data
	mData = data;

	// Check recording flag and buffer size
	if (mRecording && size <= BUFFER_LENGTH)
	{

		// Save the PCM data to file and reset the array if we 
		// don't have room for this buffer
		if (mPcmBufferPosition + size >= mPcmBufferSize) 
			writeData();

		// Copy PCM data to file buffer
		copy(mInput.getData(), mInput.getData() + size, mPcmBuffer + mPcmBufferPosition);

		// Update PCM position
		mPcmBufferPosition += size;

	}

}

// Called on exit
void WavWriterSampleApp::quit()
{

	// Stop input
	mInput.stop();

	// Free resources
	if (mData != 0)
		delete [] mData;
	if (mPcmBuffer != 0)
		delete [] mPcmBuffer;

}

// Append file buffer to output WAV
void WavWriterSampleApp::writeData()
{

	// Update header with new PCM length
	mPcmBufferPosition *= sizeof(int16_t);
	mPcmTotalSize += mPcmBufferPosition;
	mFileHeader.bytes = mPcmTotalSize + sizeof(AudioInput::WAVFILEHEADER);
	mFileHeader.pcmbytes = mPcmTotalSize;
	mFile.seekp(0);
	mFile.write(reinterpret_cast<char *>(&mFileHeader), sizeof(mFileHeader));

	// Append PCM data
	if (mPcmBufferPosition > 0)
	{
		mFile.seekp(mPcmTotalSize - mPcmBufferPosition + sizeof(AudioInput::WAVFILEHEADER));
		mFile.write(reinterpret_cast<char *>(mPcmBuffer), mPcmBufferPosition);
	}

	// Reset file buffer position
	mPcmBufferPosition = 0;

}

// Set up
void WavWriterSampleApp::setup()
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

	// Define audio settings
	mBitsPerSample = sizeof(int16_t) * 8;
	mChannelCount = 2;
	mSampleRate = 44100;

	// Set up file stream
	mRecording = false;
	mPcmBufferSize = (BUFFER_COUNT * BUFFER_LENGTH) / sizeof(int16_t); // Defined in AudioInput.h
	mPcmBuffer = new int16_t[mPcmBufferSize];
	memset(mPcmBuffer, (int16_t)0, mPcmBufferSize);

	// Set up output file header
	mFileHeader.siz_wf = mBitsPerSample;
	mFileHeader.wFormatTag = WAVE_FORMAT_PCM;
	mFileHeader.nChannels = mChannelCount;
	mFileHeader.nSamplesPerSec = mSampleRate;
	mFileHeader.nAvgBytesPerSec = mSampleRate * mChannelCount * sizeof(int16_t);
	mFileHeader.nBlockAlign = mChannelCount * sizeof(int16_t);
	mFileHeader.wBitsPerSample = mBitsPerSample;
	memcpy(mFileHeader.RIFF, "RIFF", 4);
	memcpy(mFileHeader.WAVE, "WAVE", 4);
	memcpy(mFileHeader.fmt, "fmt ", 4);
	memcpy(mFileHeader.data, "data", 4);

	// Initialize buffer
	mData = 0;

	// Start receiving audio
	mInput.addCallback<WavWriterSampleApp>(&WavWriterSampleApp::onData, this);
	mInput.start();

	// List devices -- setting device other than 0 not yet implemented
	DeviceList mDeviceList = mInput.getDeviceList();
	for (DeviceList::const_iterator i = mDeviceList.begin(); i != mDeviceList.end(); ++i)
		console() << "\n" << i->first << ": " << i->second << "\n";

}

// Start application
CINDER_APP_BASIC(WavWriterSampleApp, RendererGl)
