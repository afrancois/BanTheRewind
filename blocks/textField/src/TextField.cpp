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
TextField::Obj::Obj(const Area & area, const Font & font, bool wrap) : mFont(font), mLeading(font.getLeading()), mWrap(wrap)
{

	// Set size
	set(area.getX1(), area.getY1(), area.getX2(), area.getY2());

}

// Destructor
TextField::Obj::~Obj()
{

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

	// Check length
	if (value.length() > 0)
	{

		// Add character
		mValue += value;

		// Update texture
		update();

	}

}

// Remove a character
void TextField::Obj::operator--()
{

	// DO IT!
	if (mValue.length() > 0) 
		mValue = mValue.substr(0, mValue.length() - 1);

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

	// DO IT!
	return mBounds;

}

// Draw
gl::Texture TextField::Obj::getTexture()
{

	// Return texture
	return mTexture;

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
	int32_t mDiff = value - x1; 
	x1 += mDiff;
	x2 += mDiff;
	update();

}

// Move Y
void TextField::Obj::setY(int32_t value) 
{ 

	// Update corners
	int32_t mDiff = value - y1; 
	y1 += mDiff;
	y2 += mDiff;
	update();

}

// Get value
string TextField::Obj::str()
{

	// DO IT!
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

			// Split string to words
			mTokens.clear();
			boost::split(mTokens, mValue, boost::is_any_of(" "));

			// Iterate through words
			mValue = "";
			string mLine = "";
			for (vector<string>::const_iterator mToken = mTokens.begin(); mToken != mTokens.end(); ++mToken)
			{

				// Skip empty tokens
				if (mToken->length() > 0)
				{

					// Render word
					mTextLayoutWord = TextLayout();
					mTextLayoutWord.setColor(Color::white());
					mTextLayoutWord.setFont(mFont);
					mTextLayoutWord.setLeadingOffset(mLeading);
					mTextLayoutWord.addLine((* mToken));
					mTexWord = gl::Texture(mTextLayoutWord.render(true, false));

					// Render line
					if (mLine.length() > 0)
					{
						mTextLayoutLine = TextLayout();
						mTextLayoutLine.setColor(Color::white());
						mTextLayoutLine.setFont(mFont);
						mTextLayoutLine.setLeadingOffset(mLeading);
						mTextLayoutLine.addLine(mLine);
						mTexture = gl::Texture(mTextLayoutLine.render(true, false));
					}

					// We're just starting or line width has been reached
					if (mLine.length() <= 0 || x1 + mTexture.getWidth() + mTexWord.getWidth() > x2)
					{

						// Start new line
						if (mLine.length() > 0) mValue += "\n";
						mValue += (* mToken);
						mLine = (* mToken);

					}
					else
					{

						// Add word
						mValue += (* mToken);
						mLine += (* mToken);

					}

					// Add space
					mLine += " ";
					mValue += " ";

				}

			}

		}

		// Create final layout
		mTextLayout = TextLayout();
		mTextLayout.setColor(Color::white());
		mTextLayout.setFont(mFont);
		mTextLayout.setLeadingOffset(mLeading);

		// Add lines to layout
		mTokens.clear();
		boost::split(mTokens, mValue, boost::is_any_of("\n"));
		bool mRender = false;
		for (vector<string>::const_iterator mToken = mTokens.begin(); mToken != mTokens.end(); ++mToken)
		{
			if (mToken->length() > 0)
			{
				mTextLayout.addLine((* mToken));
				mRender = true;
			}
		}

		// Only render if lines were added to the layout
		if (mRender)
		{

			// Render text
			mTexture = gl::Texture(mTextLayout.render(true, false));

			// Get bounds
			mBounds = mTexture.getBounds();
			mBounds.set(mBounds.getX1() + x1, mBounds.getY1() + y1, mBounds.getX2() + x1, mBounds.getY2() + y1);

		}

	}

}
