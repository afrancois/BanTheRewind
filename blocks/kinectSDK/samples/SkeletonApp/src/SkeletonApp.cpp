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

// Includes
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cinder/app/AppBasic.h>
#include <cinder/Camera.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>
#include <cinder/params/Params.h>
#include <cinder/Vector.h>
#include <cinder/Utilities.h>
#include <Kinect.h>

// Imports
using namespace ci;
using namespace ci::app;
using namespace std;

// Kinect/Cinder skeleton app
class SkeletonApp : public AppBasic 
{

public:

	// Cinder callbacks
	void draw();
	void keyDown(KeyEvent event);
	void prepareSettings(Settings * settings);
	void setup();
	void update();

private:

	// Kinect
	void drawSegment(const Skeleton & skeleton, const vector<JointName> & joints);
	Kinect mKinect;
	vector<Skeleton> mSkeletons;

	// Skeleton segments
	void defineBody();
	vector<JointName> mBody;
	vector<JointName> mLeftArm;
	vector<JointName> mLeftLeg;
	vector<JointName> mRightArm;
	vector<JointName> mRightLeg;
	vector<vector<JointName> > mSegments;

	// Camera
	CameraPersp mCamera;

};

// Define body drawing
void SkeletonApp::defineBody()
{

	// Bail if defined
	if (mSegments.size() > 0)
		return;
	
	// Body
	mBody.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	mBody.push_back(NUI_SKELETON_POSITION_SPINE);
	mBody.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	mBody.push_back(NUI_SKELETON_POSITION_HEAD);

	// Left arm
	mLeftArm.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	mLeftArm.push_back(NUI_SKELETON_POSITION_SHOULDER_LEFT);
	mLeftArm.push_back(NUI_SKELETON_POSITION_ELBOW_LEFT);
	mLeftArm.push_back(NUI_SKELETON_POSITION_WRIST_LEFT);
	mLeftArm.push_back(NUI_SKELETON_POSITION_HAND_LEFT);

	// Left leg
	mLeftLeg.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	mLeftLeg.push_back(NUI_SKELETON_POSITION_HIP_LEFT);
	mLeftLeg.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
	mLeftLeg.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
	mLeftLeg.push_back(NUI_SKELETON_POSITION_FOOT_LEFT);

	// Right arm
	mRightArm.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	mRightArm.push_back(NUI_SKELETON_POSITION_SHOULDER_RIGHT);
	mRightArm.push_back(NUI_SKELETON_POSITION_ELBOW_RIGHT);
	mRightArm.push_back(NUI_SKELETON_POSITION_WRIST_RIGHT);
	mRightArm.push_back(NUI_SKELETON_POSITION_HAND_RIGHT);

	// Right leg
	mRightLeg.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	mRightLeg.push_back(NUI_SKELETON_POSITION_HIP_RIGHT);
	mRightLeg.push_back(NUI_SKELETON_POSITION_KNEE_RIGHT);
	mRightLeg.push_back(NUI_SKELETON_POSITION_ANKLE_RIGHT);
	mRightLeg.push_back(NUI_SKELETON_POSITION_FOOT_RIGHT);

	// Build skeleton drawing list
	mSegments.push_back(mBody);
	mSegments.push_back(mLeftArm);
	mSegments.push_back(mLeftLeg);
	mSegments.push_back(mRightArm);
	mSegments.push_back(mRightLeg);

}

// Render
void SkeletonApp::draw()
{

	// Clear window
	gl::setViewport(getWindowBounds());
	gl::clear(Colorf(0.1f, 0.1f, 0.1f));

	// We're capturing
	if (mKinect.isCapturing())
	{

		// Set up camera for 3D
		gl::setMatrices(mCamera);
		glScalef(2.0f, 2.0f, 2.0f);

		// Iterate through skeletons
		uint32_t i = 0;
		for (vector<Skeleton>::const_iterator skeletonIt = mSkeletons.cbegin(); skeletonIt != mSkeletons.cend(); ++skeletonIt, i++)
			if (skeletonIt->size() == JointName::NUI_SKELETON_POSITION_COUNT)
			{

				// Set color
				gl::color(mKinect.getUserColor(i));

				// Draw joints
				for (Skeleton::const_iterator jointIt = skeletonIt->cbegin(); jointIt != skeletonIt->cend(); ++jointIt)
					gl::drawSphere(jointIt->second * Vec3f(-1.0f, 1.0f, 1.0f), 0.025f, 16);

				// Draw body
				for (vector<vector<JointName> >::const_iterator segmentIt = mSegments.cbegin(); segmentIt != mSegments.cend(); ++segmentIt)
					drawSegment(* skeletonIt, * segmentIt);

			}

	}

}

// Draw segment
void SkeletonApp::drawSegment(const Skeleton & skeleton, const vector<JointName> & joints)
{

	// DO IT!
	glBegin(GL_LINES);
	for (uint32_t i = 0; i < joints.size() - 1; i++)
	{
		glVertex3f(skeleton.at(joints[i]) * Vec3f(-1.0f, 1.0f, 1.0f));
		glVertex3f(skeleton.at(joints[i + 1]) * Vec3f(-1.0f, 1.0f, 1.0f));
	}
	glEnd();

}

// Handles key press
void SkeletonApp::keyDown(KeyEvent event)
{

	// Quit, toggle fullscreen
	switch (event.getCode())
	{
	case KeyEvent::KEY_ESCAPE:
		shutdown();
		exit(1);
		break;
	case KeyEvent::KEY_f:
		setFullScreen(!isFullScreen());
		break;
	}

}

// Prepare window
void SkeletonApp::prepareSettings(Settings * settings)
{

	// DO IT!
	settings->setWindowSize(800, 600);
	settings->setFrameRate(60.0f);

}

// Set up
void SkeletonApp::setup()
{

	// Set up OpenGL
	glLineWidth(2.0f);
	gl::color(ColorAf::white());

	// Start Kinect
	mKinect.removeBackground();
	mKinect.enableDepth(false);
	mKinect.enableVideo(false);
	mKinect.start();

	// Set up camera
	mCamera.lookAt(Vec3f(0.0f, 0.0f, -3.0f), Vec3f::zero());
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 1.0f, 1000.0f);

	// Define drawing body
	defineBody();

}

// Runs update logic
void SkeletonApp::update()
{

	// Acquire skeletons
	if (mKinect.isCapturing() && mKinect.checkNewSkeletons())
		mSkeletons = mKinect.getSkeletons();

}

// Run application
CINDER_APP_BASIC(SkeletonApp, RendererGl)
