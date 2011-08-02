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
#include "cinder/Cinder.h"
#include "Sprite.h"
#include <map>

// Imports
using namespace ci;
using namespace std;

// Sprite manager class
class SpriteBatch
{

public:

	// Aliases
	typedef map<int32_t, Sprite> SpriteList;

	// Con/de-structor
	SpriteBatch() {}
	~SpriteBatch() {}

	// Adds sprite to list and set its ID
	void insert(const Sprite & sprite)
	{
		int32_t mSpriteId = mSpriteList.empty() ? 0 : mSpriteList.rbegin()->first + 1;
		sprite.setId(mSpriteId);
		mSpriteList.insert(make_pair(mSpriteId, sprite));
	}

	// Adds vector of sprites to list and set their IDs
	void insert(const vector<Sprite> & sprites)
	{
		for (vector<Sprite>::const_iterator mSprite = sprites.begin(); mSprite != sprites.end(); ++mSprite)
		{
			int32_t mSpriteId = mSpriteList.empty() ? 0 : mSpriteList.rbegin()->first + 1;
			mSprite->setId(mSpriteId);
			mSpriteList.insert(make_pair(mSpriteId, (* mSprite)));
		}
	}

	// Remove sprite from list and reset its ID
	void erase(const Sprite & sprite)
	{
		mSpriteList.erase(mSpriteList.find(sprite.getId()));
		sprite.setId(-numeric_limits<int32_t>::max());
	}

	// Clear list
	void clear() 
	{
		for (SpriteList::iterator mSprite = mSpriteList.begin(); mSprite != mSpriteList.end(); ++mSprite)
			mSprite->second.setId(-numeric_limits<int32_t>::max());
		mSpriteList.clear(); 
	}

	// Draw sprites
	void draw()
	{
		for (SpriteList::iterator mSprite = mSpriteList.begin(); mSprite != mSpriteList.end(); ++mSprite)
			mSprite->second.draw();
	}

private:

	// The list
	SpriteList mSpriteList;

};

#endif
