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
#include <cinder/Area.h>
#include <cinder/app/App.h>
#include <cinder/Text.h>
#include <cinder/gl/Texture.h>

// Text input class
class TextField
{

private:

	// The object
	class Obj : public ci::Area
	{

	public:

		// Con/de-structor
		Obj(const ci::Area & area, const ci::Font & font, bool wrap);
		~Obj();

		// Get rendered texture
		ci::gl::Texture getTexture();

		// Get render area
		ci::Area getBounds();

		// String operators
		void operator +(const std::string & value);
		void operator +=(const std::string & value);
		void operator --();
		bool operator ==(const std::string & value);
		bool operator !=(const std::string & value);

		// Get/set value
		void str(const int_fast8_t * value);
		void str(const std::string &value);
		std::string str();

		// Change text display settings
		void setFont(const ci::Font & font);
		void setLeading(float leading);

		// Updates texture
		void update();

		// Move 
		void setX(int32_t value);
		void setY(int32_t value);

		// These are needed to prevent simultaneous
		// contacts from crashing
		double mLastContactTime;
		double mContactInterval;

	private:

		// Text
		ci::Area mBounds;
		float mLeading;
		ci::Font mFont;
		ci::gl::Texture mTexture;
		ci::gl::Texture mTexWord;
		ci::TextLayout mTextLayout;
		ci::TextLayout mTextLayoutLine;
		ci::TextLayout mTextLayoutWord;
		std::vector<std::string> mTokens;
		std::string mValue;
		bool mWrap;

	};

	//	The object
	std::shared_ptr<Obj> mObj;

public: 
	
	// Contructors
	TextField() {};
	TextField(const ci::Area & area, const ci::Font & font) 
	{ 
		mObj = std::shared_ptr<Obj>(new Obj(area, font, true));
	};
	TextField(const ci::Vec2f & position, const ci::Font & font) 
	{ 
		mObj = std::shared_ptr<Obj>(new Obj(ci::Area((int32_t)position.x, (int32_t)position.y, (int32_t)position.x + 1, (int32_t)position.y + 1), font, false)); 
	};
	TextField(const ci::Vec2i & position, const ci::Font & font) 
	{ 
		mObj = std::shared_ptr<Obj>(new Obj(ci::Area(position.x, position.y, position.x + 1, position.y + 1), font, false)); 
	};
	~TextField() { mObj.reset(); }

	// Texture methods
	ci::gl::Texture getTexture() { return mObj->getTexture(); };
	ci::Area getBounds() { return mObj->getBounds(); };

	// Srting operators
	void operator +(const std::string & value) { mObj->operator+(value); }
	void operator +=(const std::string & value) { mObj->operator+=(value); }
	void operator --() { mObj->operator--(); }
	bool operator ==(const std::string & value) { return mObj->operator==(value); }
	bool operator !=(const std::string & value) { return mObj->operator!=(value); }

	// String methods
	void str(const int_fast8_t * value) { mObj->str(value); }
	void str(const std::string &value) { mObj->str(value); }
	std::string str() { return mObj->str(); }

	// Font methods
	void setFont(const ci::Font & font) { mObj->setFont(font); }
	void setLeading(float leading) { mObj->setLeading(leading); }

	// Area methods
	bool contains(const ci::Vec2f & offset) 
	{
		// Bail if simulateous touches
		if (ci::app::getElapsedSeconds() - mObj->mLastContactTime < mObj->mContactInterval)
			return false;
		mObj->mLastContactTime = ci::app::getElapsedSeconds();
		return mObj->contains(offset); 
	}
	void setX(int32_t value) { mObj->setX(value); }
	void setY(int32_t value) { mObj->setY(value); }
	void setX1(int32_t value) 
	{ 
		mObj->setX1(value); 
		mObj->update();
	}
	void setX2(int32_t value) 
	{ 
		mObj->setX2(value);
		mObj->update();
	}
	void setY1(int32_t value) 
	{ 
		mObj->setY1(value);
		mObj->update();
	}
	void setY2(int32_t value) 
	{ 
		mObj->setY2(value);
		mObj->update();
	}

};
