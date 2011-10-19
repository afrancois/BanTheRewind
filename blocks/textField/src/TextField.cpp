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
#include <TextField.h>

// Imports
using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

// Constructor
TextField::Obj::Obj(const Area & area, const Font & font, bool wrap, bool centered) 
	: mCentered(centered), mFlipped(false), mFont(font), mLeading(font.getLeading()), mWrap(wrap)
{

	// Set timing properties
	mContactInterval = 0.1;
	mLastContactTime = 0.0;

	// Set size
	set(area.getX1(), area.getY1(), area.getX2(), area.getY2());

}

// Destructor
TextField::Obj::~Obj()
{

	// Clean up
	if (mFont)
		mFont.reset();
	if (mTexture)
		mTexture.reset();
	mValue.clear();

}

// Add a character
void TextField::Obj::operator +(const string & value)
{

	// Call combo operator
	operator +=(value);

}

// Add a character
void TextField::Obj::operator +=(const string & value)
{

	// Add character
	if (value.length() > 0)
	{
		mValue += value;
		update();
	}

}

// Remove a character
void TextField::Obj::operator --()
{

	// DO IT!
	if (mValue.length() > 0) 
	{
		mValue = mValue.substr(0, mValue.length() - 1);
		update();
	}

}

// Comparison
bool TextField::Obj::operator ==(const string & value)
{

	// DO IT!
	return mValue == value;

}

// Not comparison
bool TextField::Obj::operator !=(const string & value)
{

	// DO IT!
	return mValue != value;

}

// Get actual render area (width/height excludes transparent pixels)
Area TextField::Obj::getBounds()
{
	return mBounds;
}
Area TextField::Obj::getBounds() const
{
	return mBounds;
}

// Get texture
gl::Texture TextField::Obj::getTexture()
{
	return mTexture;
}
gl::Texture TextField::Obj::getTexture() const
{
	return mTexture;
}

// Flip texture
void TextField::Obj::setFlipped(bool flip)
{

	// Change font and update
	mFlipped = flip;
	update();

}

// Update font
void TextField::Obj::setFont(const Font & font)
{

	// Change font and update
	mFont = font;
	mLeading = mFont.getLeading();
	update();

}

// Set leading
void TextField::Obj::setLeading(float leading)
{

	// DO IT!
	mLeading = leading;
	update();

}

// Move X
void TextField::Obj::setX(int32_t value) 
{ 

	// Update corners
	int32_t diff = value - x1; 
	x1 += diff;
	x2 += diff;
	update();

}

// Move Y
void TextField::Obj::setY(int32_t value) 
{ 

	// Update corners
	int32_t diff = value - y1; 
	y1 += diff;
	y2 += diff;
	update();

}

// Get value
string TextField::Obj::str()
{
	return mValue;
}
string TextField::Obj::str() const
{
	return mValue;
}

// Set value
void TextField::Obj::str(const int_fast8_t * value)
{

	// Update text
	mValue = string(value);
	update();

}

// Set value
void TextField::Obj::str(const string & value)
{

	// Update text
	mValue = value;
	update();

}

// Update texture
void TextField::Obj::update()
{

	// Reset texture
	mTexture.reset();

	// Check length
	if (mValue.length() > 0) 
	{

		// Word wrapping is on
		if (mWrap)
		{

			// Create multiline text
			mTextBox = TextBox().alignment(mCentered ? TextBox::CENTER : TextBox::LEFT).font(mFont).size(Vec2i(this->getWidth(), TextBox::GROW)).text(mValue);
			mTextBox.setColor(ColorAf::white());
			mTextBox.setBackgroundColor(ColorAf(0.5F, 0.0f, 0.0f, 0.0f));
			mTexture = gl::Texture(mTextBox.render());

		}
		else
		{

			// Create single line text
			mTextLayout = TextLayout();
			mTextLayout.setColor(Color::white());
			mTextLayout.setFont(mFont);
			mTextLayout.setLeadingOffset(mLeading);
			if (mCentered)
				mTextLayout.addCenteredLine(mValue);
			else
				mTextLayout.addLine(mValue);
			mTexture = gl::Texture(mTextLayout.render(true, false));

		}

		// Flip, if needed
		mTexture.setFlipped(mFlipped);

	}
	else
	{

		// Create minimal texture if string is empty
		mTexture.reset();
		mTexture = gl::Texture(Surface8u(8, 8, true, SurfaceChannelOrder::RGBA));

	}

	// Update bounds
	mBounds = mTexture.getBounds();
	mBounds.set(mBounds.getX1() + x1, mBounds.getY1() + y1, mBounds.getX2() + x1, mBounds.getY2() + y1);

}
