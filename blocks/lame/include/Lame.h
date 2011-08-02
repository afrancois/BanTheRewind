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

#pragma once

// Includes
#include "BladeMP3EncDLL.h"
#include "cinder/app/App.h"
#include "cinder/Thread.h"
#include "boost/filesystem.hpp"
#include <iostream>
#include <fstream>

// Imports
using namespace ci;
using namespace ci::app;
using namespace std;

// Encodes WAV to MP3
class Lame
{

private:

	// The object
	class Obj
	{

	private:

		// Header for WAV file
		typedef struct
		{
			char RIFF[4];
			DWORD bytes;
			char WAVE[4];
			char fmt[4];
			int siz_wf;
			WORD wFormatTag;
			WORD nChannels;
			DWORD nSamplesPerSec;
			DWORD nAvgBytesPerSec;
			WORD nBlockAlign;
			WORD wBitsPerSample;
			char data[4];
			DWORD pcmbytes;
		} WAVFILEHEADER;

	public:

		// Con/de-structor
		Obj();
		~Obj();

		// Thread callback
		void operator()();

		// Encode methods
		void encode(const string & source, const string & destination, int32_t bitRate = 192);
		float getProgress();
		bool isEncoding();

	private:

		// Just the one sample rate for now
		static const int32_t SAMPLE_RATE = 44100;

		// File paths
		string mSource;
		string mDestination;

		// Encoding flag
		volatile bool mEncoding;
		float mProgress;

		// LAME objects
		HINSTANCE mBlade;
		BE_CONFIG mConfig;
		BE_ERR mError;
		BE_VERSION mVersion;
		HBE_STREAM mHbeStream;

		// Interface to LAME API
		BECLOSESTREAM mBeCloseStream;
		BEDEINITSTREAM mBeDeinitStream;
		BEENCODECHUNK mBeEncodeChunk;
		BEINITSTREAM mBeInitStream;
		BEVERSION mBeVersion;
		BEWRITEINFOTAG mBeWriteInfoTag;
		BEWRITEVBRHEADER mBeWriteVBRHeader;

		// For checking function calls
		HRESULT mResult;

	};

	// Pointer to object
	std::shared_ptr<Obj> mObj;

public:

	// Con/de-structor
	Lame() { mObj = std::shared_ptr<Obj>(new Obj()); }
	~Lame() { mObj.reset(); }

	// Encode
	void encode(const string & source, const string & destination, int32_t bitRate = 192) { mObj->encode(source, destination, bitRate); }

	// Getters
	float getProgress() { return mObj->getProgress(); };
	bool isEncoding() { return mObj->isEncoding(); };

};
