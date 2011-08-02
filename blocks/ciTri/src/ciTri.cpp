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
#include <ciTri.h>

// Imports
using namespace ci;
using namespace tpp;
using namespace std;

// Convert point list into delaunay triangles
vector<Triangle> ciTri::triangulate(const vector<Vec2f> & points, float resolution)
{

	// Initialize output list
    vector<Triangle> mTriangles;

	// Convert list of points
	vector<Vec2f> mPoints = points;
    float mSize = (float)mPoints.size();
    float mCount = math<float>::min(resolution, mSize);
    Delaunay::Point mPoint;
    vector<Delaunay::Point> mVertices;
    for (int32_t i = 0; i< mCount; i++)
    {
        int32_t mId = (int32_t)((float)i / mCount * mSize);
		mPoint[0] = mPoints[mId].x;
        mPoint[1] = mPoints[mId].y;
        mVertices.push_back(mPoint);
    }

	// Triangulate points
	Delaunay mDelaunay(mVertices);
	mDelaunay.Triangulate();

	// Iterate through triangles
    for (Delaunay::fIterator mTriIt = mDelaunay.fbegin(); mTriIt != mDelaunay.fend(); ++mTriIt)
    {

		// Get point indexes
        int32_t mA = mDelaunay.Org(mTriIt);
        int32_t mB = mDelaunay.Dest(mTriIt);
        int32_t mC = mDelaunay.Apex(mTriIt);
        int32_t mAId = (int32_t)(((float)mA / resolution) * mSize);
        int32_t mBId = (int32_t)(((float)mB / resolution) * mSize);
        int32_t mCId = (int32_t)(((float)mC / resolution) * mSize);

		// Set positions in triangles
        Vec2f mTriangle[3];
        mTriangle[0] = Vec2f(mPoints[mAId].x, mPoints[mAId].y);
        mTriangle[1] = Vec2f(mPoints[mBId].x, mPoints[mBId].y);
        mTriangle[2] = Vec2f(mPoints[mCId].x, mPoints[mCId].y);

		// Find center of triangle
		Vec2f mCentroid = Vec2f(
			(mTriangle[0].x + mTriangle[1].x + mTriangle[2].x) / 3.0f, 
			(mTriangle[0].y + mTriangle[1].y + mTriangle[2].y) / 3.0f
		);

		// Initialize properties to test triangle position
		int32_t mCounter = 0;
		Vec2f mPointA = mPoints[0];
		Vec2f mPointB;

		// Iterate through points
		for (int32_t i = 1; i < (int32_t)mSize; i++)
		{

			// Get test point
			mPointB = mPoints[i];

			// Compare centroid of this triangle to the previous one
			if (mCentroid.y > math<float>::min(mPointA.y, mPointB.y) && 
				mCentroid.y <= math<float>::max(mPointA.y, mPointB.y) && 
				mCentroid.x <= math<float>::max(mPointA.x, mPointB.x) && 
				mPointA.y != mPointB.y && 
				(mPointA.x == mPointB.x || mCentroid.x <= (mCentroid.y - mPointA.y) * (mPointB.x - mPointA.x) / (mPointB.y - mPointA.y) + mPointA.x))
				mCounter++;

			// Assign this point to last
			mPointA = mPointB;

		}

		// Only include triangles which are inside shape
        if (mCounter % 2 != 0)
        {

			// Set triangle data
            Triangle mTriData;
            mTriData.a = Vec2f(mTriangle[0].x, mTriangle[0].y);
            mTriData.b = Vec2f(mTriangle[1].x, mTriangle[1].y);
            mTriData.c = Vec2f(mTriangle[2].x, mTriangle[2].y);
            mTriData.area = (float)mDelaunay.area(mTriIt);
			mTriData.centroid = mCentroid;

			// Add triangle to list
            mTriangles.push_back(mTriData);

        }

    }

	// Return triangles
	return mTriangles;

}
