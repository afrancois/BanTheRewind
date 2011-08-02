/*
 * 
 * Copyright (c) 2011, Ban the Rewind
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or 
 * without modification, are permitted provided that the following 
 * conditions are met:
 * 
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in 
 * the documentation and/or other materials provided with the 
 * distribution.
 * 
 * Neither the name of the Ban the Rewind nor the names of its 
 * contributors may be used to endorse or promote products 
 * derived from this software without specific prior written 
 * permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

// Include header
#include "Lame.h"

// Constructor
Lame::Obj::Obj()
{

	// Set encoding flag
	mEncoding = false;
	mProgress = 0.0f;

	// Load LAME DLL
	mBlade = LoadLibraryA("lame_enc.dll");

	// Check DLL
	if (mBlade == NULL)
	{
		console() << "Unable to load lame_enc.dll";
		return;
	}

	// Define interfaces into LAME API
	mBeInitStream = (BEINITSTREAM)GetProcAddress(mBlade, TEXT_BEINITSTREAM);
    mBeEncodeChunk = (BEENCODECHUNK)GetProcAddress(mBlade, TEXT_BEENCODECHUNK);
    mBeDeinitStream = (BEDEINITSTREAM)GetProcAddress(mBlade, TEXT_BEDEINITSTREAM);
    mBeCloseStream = (BECLOSESTREAM)GetProcAddress(mBlade, TEXT_BECLOSESTREAM);
    mBeVersion = (BEVERSION)GetProcAddress(mBlade, TEXT_BEVERSION);
    mBeWriteVBRHeader = (BEWRITEVBRHEADER)GetProcAddress(mBlade, TEXT_BEWRITEVBRHEADER);
    mBeWriteInfoTag = (BEWRITEINFOTAG)GetProcAddress(mBlade,TEXT_BEWRITEINFOTAG);

	// Check interfaces
	if (!mBeInitStream || !mBeEncodeChunk || !mBeDeinitStream || !mBeCloseStream || !mBeVersion || !mBeWriteVBRHeader)
    {
        console() << "Unable to get interfaces from lame_enc.dll.";
        return;
    }

	// Get version
	mBeVersion(&mVersion);

	// Initialize config
	memset(&mConfig, 0, sizeof(mConfig));
	mConfig.dwConfig = BE_CONFIG_LAME;

	// Set up LAME configuration
	mConfig.format.LHV1.dwStructVersion = 1;
    mConfig.format.LHV1.dwStructSize = sizeof(mConfig);     
    mConfig.format.LHV1.dwSampleRate = SAMPLE_RATE;
    mConfig.format.LHV1.dwReSampleRate = 0;
    mConfig.format.LHV1.nMode = BE_MP3_MODE_JSTEREO;
    mConfig.format.LHV1.dwBitrate = 192;
    mConfig.format.LHV1.nPreset = LQP_VERYHIGH_QUALITY;
    mConfig.format.LHV1.dwMpegVersion = MPEG1;
    mConfig.format.LHV1.dwPsyModel = 0;
    mConfig.format.LHV1.dwEmphasis = 0;
    mConfig.format.LHV1.bOriginal = TRUE;
    mConfig.format.LHV1.bWriteVBRHeader = TRUE;
    mConfig.format.LHV1.bNoRes = TRUE;
    mConfig.format.LHV1.bCRC = TRUE;

}

// Destructor
Lame::Obj::~Obj()
{
}

// Thread callback
void Lame::Obj::operator()()
{

	// Reset some properties
	mError = 0;
	mHbeStream = 0;
	mResult = S_OK;

	// Initialize encoding stream
	DWORD mMp3BufferSize = 0;
	DWORD mWavBufferSize = 0;
	mError = mBeInitStream(&mConfig, &mWavBufferSize, &mMp3BufferSize, &mHbeStream);
	if (mError != BE_ERR_SUCCESSFUL)
    {
        console() << "Error opening encoding stream\n";
        return;
    }

	// Allocate MP3 and WAV buffers
	PBYTE mMp3Buffer = NULL;
	PSHORT mWavBuffer = NULL;
    mMp3Buffer = new BYTE[mMp3BufferSize];
	mWavBuffer = new SHORT[mWavBufferSize];
	if (!mMp3Buffer || !mWavBuffer)
	{
        console() << "Out of memory:\n";
        return;
	}

	// Declare properties to keep track of encoding chunks
	DWORD mComplete = 0;
	DWORD mRead = 0;
	DWORD mSize = 0;
	DWORD mWrite = 0;

	// Open files
	FILE * mFileIn = NULL;
	FILE * mFileOut = NULL;
	mFileIn = fopen(mSource.c_str(), "rb");
	mFileOut= fopen(mDestination.c_str(), "wb+");
	if (mFileIn == NULL || mFileOut == NULL)
	{
        console() << "Unable to open file";
        return;
	}

	// Get file size
    fseek(mFileIn, 0, SEEK_END);
	mSize = ftell(mFileIn);

	// Seek past header to PCM data
	fseek(mFileIn, sizeof(WAVFILEHEADER), SEEK_SET);

	// Iterate through PCM data
	while ((mRead = fread(mWavBuffer, sizeof(SHORT), mWavBufferSize, mFileIn)) > 0)
	{

		// Encode samples
		mError = mBeEncodeChunk(mHbeStream, mRead, mWavBuffer, mMp3Buffer, &mWrite);
		if (mError != BE_ERR_SUCCESSFUL)
		{
			console() << "Unable to encode file";
			return;
		}

		// Write mWrite bytes that are returned in the pMP3Buffer to disk
		if (fwrite(mMp3Buffer, 1, mWrite, mFileOut) != mWrite)
		{
			console() << "Unable to save MP3";
			return;
		}

        // Update progress
        mComplete += mRead * sizeof(SHORT);
        mProgress = (float)mComplete / (float)(mSize);

	}

	// Uninitialize the stream
	mError = mBeDeinitStream(mHbeStream, mMp3Buffer, &mWrite);
	if (mError != BE_ERR_SUCCESSFUL)
	{
		mBeCloseStream(mHbeStream);
		console() << "Exit stream failed";
		return;
	}

	// Write any remaining bytes to disk
	if (mWrite && fwrite(mMp3Buffer, 1, mWrite, mFileOut) != mWrite)
	{
		console() << "Unable to save MP3";
		return;
	}

	// Clean up
	mBeCloseStream(mHbeStream);
	delete [] mWavBuffer;
	delete [] mMp3Buffer;
	fclose(mFileIn);
	fclose(mFileOut);

	// Write the VBR tag
	mBeWriteVBRHeader(mDestination.c_str());

	// Encoding complete
	mEncoding = false;

	// Return to stop thread
	return;

}

// Encode MP3
void Lame::Obj::encode(const string & source, const string & destination, int32_t bitRate)
{

	// Record path for thread
	mDestination = destination;
	mSource = source;

	// Set bit rate
	mConfig.format.LHV1.dwBitrate = bitRate;

	// Check if file exists
	if (!boost::filesystem::exists(boost::filesystem::path(source)))
	{
		console() << "Input file does not exist";
		return;
	}

	// Check DLL
	if (mBlade == NULL)
	{
		console() << "LAME not initialized";
		return;
	}

	// Check encoding flag
	if (!mEncoding)
	{

		// Set encoding flag
		mEncoding = true;
		mProgress = 0.0f;

		// Run encoding in separate thread
		thread(&Lame::Obj::operator(), this);

	}

}

// Return encode progress
float Lame::Obj::getProgress()
{

	// DO IT!
	return mProgress;

}

// Return encoding flag
bool Lame::Obj::isEncoding()
{

	// DO IT!
	return mEncoding;

}
