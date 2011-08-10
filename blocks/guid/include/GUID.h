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
#include <boost/algorithm/string.hpp>
#include "cinder/Cinder.h"
#include "time.h"

// Imports
using namespace ci;
using namespace std;

namespace cinder
{

	class guid
	{

	public:

		// Get a new GUID
		static string create() 
		{

			// The GUID
			int_fast8_t mGuid[39];

			// Generate a GUID based on time
			int_fast8_t * nGuid = mGuid;
			int32_t i;
			srand(static_cast<uint32_t> (time(NULL)));
			* nGuid++ = '{';
			for (i = 0; i < 8; i++, nGuid++)
				((* nGuid = (rand() % 16)) < 10) ? * nGuid += 48 : * nGuid += 55;
			* nGuid++ = '-';
			for (i = 0; i < 4; i++, nGuid++)
				((* nGuid = (rand() % 16)) < 10) ? * nGuid += 48 : * nGuid += 55;
			* nGuid++ = '-';
			for (i = 0; i < 4; i++, nGuid++)
				((* nGuid = (rand() % 16)) < 10) ? * nGuid += 48 : * nGuid += 55;
			* nGuid++ = '-';
			for (i = 0; i < 4; i++, nGuid++)
				((* nGuid = (rand() % 16)) < 10) ? * nGuid += 48 : * nGuid += 55;
			* nGuid++ = '-';
			for (i = 0; i < 12; i++, nGuid++)
				((* nGuid = (rand() % 16)) < 10) ? * nGuid += 48 : * nGuid += 55;
			* nGuid++ = '}';
			* nGuid = '\0';

			// Build return string
			string mGuidStr = boost::to_lower_copy((string)mGuid);

			// Clean up
			nGuid = 0;

			// Return string
			return mGuidStr;

		}

	};

}
