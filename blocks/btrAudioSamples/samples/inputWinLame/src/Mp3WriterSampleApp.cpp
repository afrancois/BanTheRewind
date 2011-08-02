// Includes
#include <AudioInput.h>
#include <cinder/app/AppBasic.h>
#include <cinder/audio/Callback.h>
#include <cinder/audio/Io.h>
#include <cinder/audio/Output.h>
#include <cinder/audio/PcmBuffer.h>
#include <fstream>
#include <iostream>
#include <Kiss.h>
#include <Lame.h>
#include <TextField.h>

// Imports
using namespace ci;
using namespace ci::app;

// Main application
class Mp3WriterSampleApp : public AppBasic 
{

private:

	// Test states
	struct TestState
	{

		static const uint32_t NONE = 0x00;
		static const uint32_t INPUT = 0x01;
		static const uint32_t ENCODE = 0x02;
		static const uint32_t OUTPUT = 0x03;

	};

public:

	// Cinder callbacks
	void draw();
	void keyUp(KeyEvent event);
	void quit();
	void setup();
	void update();

	// Audio input handler
	void onData(float * data, int32_t size);

private:

	// Window dimensions
	static const int32_t WINDOW_WIDTH = 600; 
	static const int32_t WINDOW_HEIGHT = 600;

	// Current test state
	uint32_t mState;

	// Files
	string mFileWav;
	string mFileMp3;

	// Audio input
	AudioInput mAudioInput;
	float * mInputData;
	double mTestPause;
	double mTestStartTime;
	double mTestDuration;

	// Audio output
	audio::PcmBuffer32fRef mBuffer;
	int32_t mCallbackId;
	Kiss mFft;
	bool mFftInit;
	audio::SourceRef mSoundLoader;
	float * mOutputData;
	audio::TrackRef mTrack;

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

	// Writes PCM buffer to file
	void writeData();

	// MP3 encoding
	Lame mEncoder;
	void encode();

	// Drawing
	Area mAreaEncoder;
	Area mAreaInput;
	Area mAreaOutput;
	ColorAf mColor;
	Font mFont;
	TextField mTextFieldEncoder;
	TextField mTextFieldInput;
	TextField mTextFieldInstructions;
	TextField mTextFieldOutput;
	void setColor(uint32_t state);

};

// Draw
void Mp3WriterSampleApp::draw()
{

	// Clear screen
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	// Draw background boxes
	mColor.a = 0.3f;
	setColor(TestState::INPUT);
	gl::color(mColor);
	gl::drawSolidRect(Rectf(mAreaInput));
	setColor(TestState::ENCODE);
	gl::color(mColor);
	gl::drawSolidRect(Rectf(mAreaEncoder));
	setColor(TestState::OUTPUT);
	gl::color(mColor);
	gl::drawSolidRect(Rectf(mAreaOutput));

	// Draw text
	mColor.a = 1.0f;
	gl::color(ColorAf::white());
	gl::draw(mTextFieldInstructions.getTexture(), mTextFieldInstructions.getBounds());
	setColor(TestState::INPUT);
	gl::color(mColor);
	gl::draw(mTextFieldInput.getTexture(), mTextFieldInput.getBounds());
	setColor(TestState::ENCODE);
	gl::color(mColor);
	gl::draw(mTextFieldEncoder.getTexture(), mTextFieldEncoder.getBounds());
	setColor(TestState::OUTPUT);
	gl::color(mColor);
	gl::draw(mTextFieldOutput.getTexture(), mTextFieldOutput.getBounds());

	// Input
	if (mState == TestState::INPUT && mInputData != 0)
	{

		// Update color
		setColor(TestState::INPUT);
		gl::color(mColor);

		// Get size of data
		int32_t mDataSize = mAudioInput.getDataSize();

		// Get dimensions
		float mScale = (float)mAreaInput.getWidth() / (float)mDataSize;

		// Use polyline to depict audio
		PolyLine<Vec2f> mLine;

		// Iterate through data and populate line
		for (int32_t i = 0; i < mDataSize; i++) 
		{
			float mValue = mInputData[i];
			if (math<float>::abs(mValue) > 1.0f)
				mValue = 0.0f; // Controls line size a bit
			mLine.push_back(Vec2f(i * mScale + mAreaInput.getX1(), (mAreaInput.getY1() + mAreaInput.getHeight() * 0.5f) + mValue * mAreaInput.getHeight() * 0.5f));
		}

		// Draw signal
		gl::draw(mLine);
		
	}
	
	// Encoding
	if (mState == TestState::ENCODE)
	{

		// Update color
		setColor(TestState::ENCODE);
		gl::color(mColor);

		// Draw progress bar
		gl::drawLine(Vec2f((float)mAreaEncoder.getX1(), (float)(mAreaEncoder.getY1() + mAreaEncoder.getHeight() * 0.5f)), 
			Vec2f((float)mAreaEncoder.getX1() + ((float)mAreaEncoder.getWidth() * mEncoder.getProgress()), (float)(mAreaEncoder.getY1() + mAreaEncoder.getHeight() * 0.5f)));

	}

	// Playing
	if (mState == TestState::OUTPUT && mOutputData != 0)
	{

		// Update color
		setColor(TestState::OUTPUT);
		gl::color(mColor);

		// Get size of data
		int32_t mDataSize = mFft.getDataSize();

		// Get dimensions
		float mScale = (float)mAreaOutput.getWidth() / (float)mDataSize;

		// Use polyline to depict audio
		PolyLine<Vec2f> mLine;

		// Iterate through data and populate line
		for (int32_t i = 0; i < mDataSize; i++) 
			mLine.push_back(Vec2f(i * mScale + mAreaOutput.getX1(), (mAreaOutput.getY1() + mAreaOutput.getHeight() * 0.5f) + (mOutputData[i] * 2.0f - 1.0f) * mAreaOutput.getHeight() * 0.5f));

		// Draw signal
		gl::draw(mLine);

	}

}

// Handles key press
void Mp3WriterSampleApp::keyUp(KeyEvent event)
{

	// Quit
	if (event.getCode() == KeyEvent::KEY_ESCAPE)
		quit();

	// Start test with space bar
	if (mState == TestState::NONE && event.getCode() == KeyEvent::KEY_SPACE)
		mState = TestState::INPUT;

}

// Called when buffer is full
void Mp3WriterSampleApp::onData(float * data, int32_t size)
{

	// Get data
	mInputData = data;

	// Check recording flag and buffer size
	if (mRecording)
	{

		// Save the PCM data to file and reset the array if we 
		// don't have room for this buffer
		if (mPcmBufferPosition + size >= mPcmBufferSize) 
			writeData();

		// Copy PCM data to file buffer
		copy(mAudioInput.getData(), mAudioInput.getData() + size, mPcmBuffer + mPcmBufferPosition);

		// Update PCM position
		mPcmBufferPosition += size;

	}

}

// Called on exit
void Mp3WriterSampleApp::quit()
{

	// Stop input
	if (mAudioInput.isReceiving())
	{
		mAudioInput.removeCallback(mCallbackId);
		mAudioInput.stop();
	}

	// Free resources
	if (mInputData != 0)
		delete [] mInputData;
	if (mOutputData != 0)
		delete [] mOutputData;
	if (mPcmBuffer != 0)
		delete [] mPcmBuffer;

}

// Set current color
void Mp3WriterSampleApp::setColor(uint32_t state)
{

	// Set default color
	mColor.r = 1.0f;
	mColor.g = 1.0f;
	mColor.b = 1.0f;

	// Bail if no test state is active
	if (mState == TestState::NONE)
		return;

	// Set color to red if drawing active state
	if (state == mState)
	{
		mColor.r = 1.0f;
		mColor.g = 0.0f;
		mColor.b = 0.0f;
	}

}

// Set up
void Mp3WriterSampleApp::setup()
{

	// Set up window
	setFrameRate(60);
	setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// Set up OpenGL
	gl::enableAlphaBlending();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Set initial test state
	mState = TestState::NONE;
	mTestDuration = 5.0;
	mTestPause = 1.0;
	mTestStartTime = 0.0;

	// Set file names
	mFileMp3 = getAppPath() + "test.mp3";
	mFileWav = getAppPath() + "test.wav";

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

	// Initialize buffers
	mFftInit = false;
	mInputData = 0;
	mOutputData = 0;

	// Set color and font
	mColor = ColorAf(1.0f, 1.0f, 1.0f, 1.0f);
	mFont = Font("Courier", 10.0f);

	// Set up instructions text
	mTextFieldInstructions = TextField(Vec2i(10, 10), mFont);
	mTextFieldInstructions.str("Press SPACE to start test\nRecords WAV for five seconds, then encodes and plays back MP3");

	// Input
	mTextFieldInput = TextField(Vec2i(mTextFieldInstructions.getBounds().getX1(), mTextFieldInstructions.getBounds().getY2() + mTextFieldInstructions.getBounds().getY1()), mFont);
	mTextFieldInput.str("INPUT");
	mAreaInput.set(mTextFieldInput.getBounds().getX1(), mTextFieldInput.getBounds().getY2(), WINDOW_WIDTH - mTextFieldInstructions.getBounds().getY1(), mTextFieldInput.getBounds().getY2() + WINDOW_HEIGHT / 3);

	// Encoder
	mTextFieldEncoder = TextField(Vec2i(mAreaInput.getX1(), mAreaInput.getY2() + mTextFieldInstructions.getBounds().getY1() * 2), mFont);
	mTextFieldEncoder.str("ENCODER");
	mAreaEncoder.set(mTextFieldEncoder.getBounds().getX1(), mTextFieldEncoder.getBounds().getY2(), WINDOW_WIDTH - mTextFieldInstructions.getBounds().getY1(), mTextFieldEncoder.getBounds().getY2() + WINDOW_HEIGHT / 20);

	// Output
	mTextFieldOutput = TextField(Vec2i(mAreaEncoder.getX1(), mAreaEncoder.getY2() + mTextFieldInstructions.getBounds().getY1() * 2), mFont);
	mTextFieldOutput.str("OUTPUT");
	mAreaOutput.set(mTextFieldOutput.getBounds().getX1(), mTextFieldOutput.getBounds().getY2(), WINDOW_WIDTH - mTextFieldInstructions.getBounds().getY1(), mTextFieldOutput.getBounds().getY2() + WINDOW_HEIGHT / 3);

}

// Runs update logic
void Mp3WriterSampleApp::update()
{

	// Recording
	if (mState == TestState::INPUT)
	{

		// Recording
		if (mRecording)
		{

			// Time is up
			if (getElapsedSeconds() - mTestStartTime >= mTestDuration)
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

				// Stop input
				mAudioInput.removeCallback(mCallbackId);
				mAudioInput.stop();
				mRecording = false;
				
				// Pause
				mTestStartTime = getElapsedSeconds();
				while (getElapsedSeconds() - mTestStartTime >= mTestPause);

				// Encode
				mEncoder.encode(mFileWav, mFileMp3);
				mState = TestState::ENCODE;

			}

		}
		else
		{

			// Start receiving audio
			mAudioInput.start();
			mCallbackId = mAudioInput.addCallback<Mp3WriterSampleApp>(&Mp3WriterSampleApp::onData, this);

			// Set size values
			mPcmBufferPosition = 0;
			mPcmTotalSize = 0;

			// Open file for streaming
			mFile.open(mFileWav, ios::binary|ios::trunc);

			// Start recording
			mTestStartTime = getElapsedSeconds();
			mRecording = true;
			
		}

	}

	// Encoding
	if (mState == TestState::ENCODE)
	{

		// Encoding complete
		if (!mEncoder.isEncoding())
		{

			// Pause
			mTestStartTime = getElapsedSeconds();
			while (getElapsedSeconds() - mTestStartTime >= mTestPause);

			// Play MP3
			mSoundLoader = audio::load(mFileMp3);
			mTrack = audio::Output::addTrack(mSoundLoader, true);
			mTrack->enablePcmBuffering(true);

			// Change state
			mState = TestState::OUTPUT;

		}

	}

	// Output state
	if (mState == TestState::OUTPUT)
	{

		// Check track
		if (mTrack)
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

						// Set output data
						mOutputData = mFft.getData();

					}

				}

			}
			else
			{

				// Remove track
				audio::Output::removeTrack(0);

				// Reset objects
				mOutputData = 0;
				mTrack.reset();
				mSoundLoader.reset();

				// Reset state
				mState = TestState::NONE;

			}

		}

	}

}

// Append file buffer to output WAV
void Mp3WriterSampleApp::writeData()
{

	// Update header with new PCM length
	mPcmBufferPosition *= sizeof(int16_t);
	mPcmTotalSize += mPcmBufferPosition;
	mFileHeader.bytes = mPcmTotalSize + sizeof(AudioInput::WAVFILEHEADER);
	mFileHeader.pcmbytes = mPcmTotalSize;
	mFile.seekp(0);
	mFile.write(reinterpret_cast<char *>(&mFileHeader), sizeof(mFileHeader));

	// Append PCM data
	mFile.seekp(mPcmTotalSize - mPcmBufferPosition + sizeof(AudioInput::WAVFILEHEADER));
	mFile.write(reinterpret_cast<char *>(mPcmBuffer), mPcmBufferPosition);

	// Reset file buffer position
	mPcmBufferPosition = 0;

}

// Start application
CINDER_APP_BASIC(Mp3WriterSampleApp, RendererGl)
