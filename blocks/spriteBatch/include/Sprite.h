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
#include <cinder/Area.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>

// Imports
using namespace ci;
using namespace std;

// Sprite class
class Sprite
{

private:

	class Obj : public Area
	{

	public: 

		// Con/de-structors
		Obj(const gl::Texture & texture, const Area & area, const ColorAf & color) : mAlpha(1.0f), 
			mBounds(texture.getCleanBounds()), 
			mColor(color), 
			mId(-numeric_limits<int32_t>::max()), 
			mTexture(texture), 
			mVisible(true)
		{
			set(area.getX1(), area.getY1(), area.getX2(), area.getY2());
			setCenter();
			
			// Set contact interval
			mContactInterval = 0.1;
			mLastContactTime = 0.0;

		}
		~Obj() { mTexture.reset(); };

		// Draw
		void draw() {
			if (mVisible && mBounds.getWidth() > 0 && mBounds.getHeight() > 0)
			{
				gl::color(mAlpha == 1.0f ? mColor : ColorAf(mColor.r, mColor.g, mColor.b, mAlpha));
				gl::draw(mTexture, (Area)(* this)); 
			}
		}

		// Get/set
		float getAlpha() { return mAlpha; }
		Vec2f getCenter() { return mCenter; }
		int32_t getId() { return mId; }
		bool getVisible() { return mVisible; }
		void setAlpha(float alpha) { mAlpha = alpha; }
		void setCenter() { mCenter = Vec2f((float)(x1 + (x2 - x1) / 2), (float)(y1 + (y2 - y1) / 2)); }
		void setColor(const ColorAf & color) { mColor = color; }
		void setId(int32_t id) { mId = id; }
		void setVisible(bool visible) { mVisible = visible; }
		void setX(int32_t value) { 
			int32_t mDiff = value - x1; 
			x1 += mDiff;
			x2 += mDiff;
			setCenter();
		}
		void setY(int32_t value) { 
			int32_t mDiff = value - y1; 
			y1 += mDiff;
			y2 += mDiff;
			setCenter();
		}

		// These are needed to prevent simultaneous
		// contacts from crashing
		double mLastContactTime;
		double mContactInterval;

	private:

		// Color
		ColorAf mColor;

		// Texture
		Area mBounds;
		gl::Texture mTexture;

		// ID
		int32_t mId;

		// Visibility
		bool mVisible;
		float mAlpha;

		// Center
		Vec2f mCenter;

	};

	//	The object
	std::shared_ptr<Obj> mObj;

public:

	// Constructor
	Sprite() {};
	Sprite(const gl::Texture & texture, const Area & area, const ColorAf & color) : mObj(shared_ptr<Obj>(new Obj(texture, area, color))) {}
	~Sprite() { mObj.reset(); }

	// Methods
	bool contains(const Vec2f & offset) 
	{
		// Bail if simulateous touches
		if (!mObj)
			return false;
		if (getElapsedSeconds() - mObj->mLastContactTime < mObj->mContactInterval)
			return false;
		mObj->mLastContactTime = getElapsedSeconds();
		return mObj->contains(offset); 
	}
	void draw() { mObj->draw(); }
	float getAlpha() const { return mObj->getAlpha(); }
	Vec2f getCenter() { return mObj->getCenter(); }
	int32_t getHeight() { return mObj->getY2() - mObj->getY1(); }
	int32_t getId() const { return mObj->getId(); }
	bool getVisible() const { return mObj->getVisible(); }
	int32_t getWidth() { return mObj->getX2() - mObj->getX1(); }
	void setAlpha(float alpha) const { mObj->setAlpha(alpha); }
	void setColor(const ColorAf & color) const { mObj->setColor(color); }
	void setId(int32_t id) const { mObj->setId(id); }
	void setVisible(bool visible) const { mObj->setVisible(visible); }
	int32_t getX1() { return mObj->getX1(); }
	int32_t getX2() { return mObj->getX2(); }
	int32_t getY1() { return mObj->getY1(); };
	int32_t getY2() { return mObj->getY2(); }
	void setX(int32_t value) { mObj->setX(value); };
	void setX1(int32_t value) { 
		mObj->setX1(value); 
		mObj->setCenter();
	}
	void setX2(int32_t value) { 
		mObj->setX2(value);
		mObj->setCenter();
	}
	void setY(int32_t value) { mObj->setY(value); }
	void setY1(int32_t value) { 
		mObj->setY1(value); 
		mObj->setCenter();
	}
	void setY2(int32_t value) { 
		mObj->setY2(value); 
		mObj->setCenter();
	}

};
