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
#include "boost/algorithm/string.hpp"
#include "cinder/Cinder.h"
#include "cinder/DataSource.h"
#include "cinder/Url.h"
#include <fstream>
#include <stdint.h>
#include "json/json.h"
#include <string>
#if defined(CINDER_MSW)
	#include <stdio.h>
	#include <Windows.h>
	#include <WinInet.h>
#endif

// Imports
using namespace std;

namespace cinder 
{ 

	namespace json
	{

		// Alias for JsonCpp types
		typedef Json::ValueType Type;
		typedef Json::Value Value;

		// Load JSON from file, URL
		Value load(const string & path);
		Value loadUrl(const string & url, bool isEscaped = false);

		// De/serialize
		Value deserialize(const string & value);
		string serialize(const Value & value);

		// Append value
		void append(Value & object, const string & key, bool value);
		void append(Value & object, const string & key, char * value);
		void append(Value & object, const string & key, double value);
		void append(Value & object, const string & key, float value);
		void append(Value & object, const string & key, int32_t value);
		void append(Value & object, const string & key, string value);
		void append(Value & object, const string & key, uint32_t value);

		// Append list of values
		void append(Value & object, const string & key, vector<bool> values);
		void append(Value & object, const string & key, vector<double> values);
		void append(Value & object, const string & key, vector<float> values);
		void append(Value & object, const string & key, vector<int32_t> values);
		void append(Value & object, const string & key, vector<string> values);
		void append(Value & object, const string & key, vector<uint32_t> values);
		void append(Value & object, const string & key, vector<Value> values);

		// Read values (index)
		bool readBool(Value & object, int32_t index);
		double readDouble(Value & object, int32_t index);
		float readFloat(Value & object, int32_t index);
		int32_t readInt(Value & object, int32_t index);
		string readString(Value & object, int32_t index);
		uint32_t readUint(Value & object, int32_t index);
		Value readValue(Value & object, int32_t index);

		// Read values (index, plural)
		vector<bool> readBools(Value & object, int32_t index);
		vector<double> readDoubles(Value & object, int32_t index);
		vector<float> readFloats(Value & object, int32_t index);
		vector<int32_t> readInts(Value & object, int32_t index);
		vector<string> readStrings(Value & object, int32_t index);
		vector<uint32_t> readUints(Value & object, int32_t index);
		vector<Value> readValues(Value & object, int32_t index);

		// Read values (key)
		bool readBool(Value & object, const string & key);
		double readDouble(Value & object, const string & key);
		float readFloat(Value & object, const string & key);
		int32_t readInt(Value & object, const string & key);
		string readString(Value & object, const string & key);
		Value readValue(Value & object, const string & key);
		uint32_t readUint(Value & object, const string & key);

		// Read values (key, plural)
		vector<bool> readBools(Value & object, const string & key);
		vector<double> readDoubles(Value & object, const string & key);
		vector<float> readFloats(Value & object, const string & key);
		vector<int32_t> readInts(Value & object, const string & key);
		vector<string> readStrings(Value & object, const string & key);
		vector<Value> readValues(Value & object, const string & key);
		vector<uint32_t> readUints(Value & object, const string & key);

		// Save JSON to file, URL
		void save(const Value & value, const string & path);
#if defined(CINDER_MSW)
		string saveUrl(const Value & value, const string & url);
#endif

	}

}
