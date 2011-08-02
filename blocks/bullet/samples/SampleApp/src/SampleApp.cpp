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
#include "SampleApp.h"

// Render
void SampleApp::draw()
{

	// Set up screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl::setMatrices(mCamera);
	gl::rotate(Vec3f(-45.0f, math<float>::sin((float)getElapsedSeconds()) * 20.0f, 0.0f));

	// Draw world
	mMaterial.apply();
	mDynamicsWorld->draw(mWireframe);

}

// Handles key press
void SampleApp::keyDown(KeyEvent event)
{

	// Key on key...
	switch (event.getCode())
	{
	case KeyEvent::KEY_ESCAPE:
		shutdown();
		break;
	case KeyEvent::KEY_F11:
		setFullScreen(!isFullScreen());
		break;
	case KeyEvent::KEY_SPACE:
		writeImage(getAppPath() + "\\frame_" + toString(getElapsedFrames()) + ".png", copyWindowSurface());
		break;
	case KeyEvent::KEY_w:
		mWireframe = !mWireframe;
		break;
	}

}

// Handles mouse button press
void SampleApp::mouseDown(MouseEvent event)
{

	// Set random size and position
	float size = Rand::randFloat(5.0f, 20.0f);
	Vec3f position = Vec3f(
		(float)((rand() % 400) - 200), 
		(float)((rand() % 100) + 150), 
		(float)((rand() % 400) - 200)
		);

	// Drop primitive into terrain
	switch (Rand::randInt(0, 5))
	{
	case 0:
		mDynamicsWorld->insert(createRigidSphere(mDynamicsWorld, size, 32, position, Quatf()))->setLifespan(5.0);
		break;
	case 1:
		mDynamicsWorld->insert(createRigidBox(mDynamicsWorld, Vec3f(size, size, size), position, Quatf()))->setLifespan(5.0);
		break;
	case 2:
		mDynamicsWorld->insert(createRigidHull(mDynamicsWorld, mConvex, Vec3f(size, size, size), position, Quatf()))->setLifespan(5.0);
		break;
	case 3:
		size *= 0.75f;
		mDynamicsWorld->insert(createRigidMesh(mDynamicsWorld, mConcave, Vec3f(size, size, size) * 0.15f, 0.05f, position, Quatf()))->setLifespan(5.0);
		break;
	case 4:
		mDynamicsWorld->insert(createRigidCylinder(mDynamicsWorld, size * 0.75f, size, size * 2.0f, 16, position, Quatf()))->setLifespan(5.0);
		break;
	}

}

// Handles mouse wheel
void SampleApp::mouseWheel(MouseEvent event)
{

	// Zoom
	mCamera.setEyePoint(mCamera.getEyePoint() - Vec3f(0.0f, 0.0f, event.getWheelIncrement() * 20.0f));

}

// Handles window resize
void SampleApp::resize(int32_t width, int32_t height)
{

	// Set up viewport
	glViewport(0, 0, width, height);
	mCamera.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 2000.0f);
	gl::setMatrices(mCamera);

}

// Setup
void SampleApp::setup()
{

	/****** OPEN GL ******/

	// Set up window
	setFrameRate(60.0f);
	setWindowSize(1600, 900);
	setFullScreen(true);

	// Set up OpenGL
	gl::enableAlphaBlending();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	// Set up camera
	mCamera.lookAt(Vec3f(0.0f, 0.0f, -1000.0f), Vec3f::zero());

	// Set up lighting
	mLight = new gl::Light(gl::Light::DIRECTIONAL, 0);
	mLight->setDirection(Vec3f(0.0f, 0.1f, 0.3f).normalized());
	mLight->setAmbient(ColorAf(0.2f, 0.2f, 0.2f, 1.0f));
	mLight->setDiffuse(ColorAf(1.0f, 1.0f, 1.0f, 1.0f));
	mLight->update(mCamera);
	mLight->enable();

	// Set up color materials
	gl::color(ColorAf::white());
	mMaterial.setSpecular(ColorAf(0.8f, 0.68f, 0.62f, 1.0f));
	mMaterial.setDiffuse(ColorAf(0.8f, 0.8f, 1.0f, 1.0f));
	mMaterial.setAmbient(ColorAf(0.7f, 0.7f, 0.7f, 1.0f));
	mMaterial.setShininess(50.0f);

	// Run first resize to initialize camera
	resize(getWindowWidth(), getWindowHeight());

	// Wireframes off
	mWireframe = false;

	/****** MODELS ******/

	// Load OBJ files as meshes
	ObjLoader loader(loadResource(RES_SPHERE)->createStream());
	loader.load(& mConvex);
	loader = ObjLoader(loadResource(RES_TORUS)->createStream());
	loader.load(& mConcave);

	/****** TERRAIN ******/

	// Create Bullet world
	mDynamicsWorld = bullet::createWorld();

	// Capture image
	mCapture = Capture(640, 480);
	mCapture.start();
	if (!mCapture.isCapturing())
	{
		shutdown();
		return;
	}
	while (!mCapture.checkNewFrame());

	// Resize captured image to use for height map
	Surface32f mTerrainImage = Surface32f(ip::resizeCopy(mCapture.getSurface(), Area(0, 0, 640, 480), Vec2i(IMAGE_SIZE, IMAGE_SIZE)));
	ip::flipVertical(&mTerrainImage);
	mCapture.stop();

	// Add terrain
	RigidObjectRef terrain = createRigidTerrain(mDynamicsWorld, mTerrainImage, 256, 256, -200, 200, 1, Vec3f(6.0f, 140.0f, 6.0f));
	terrain->getBody()->setMassProps(0, btVector3(0, 0, 0));
	mDynamicsWorld->insert(terrain);

}

// Called on exit
void SampleApp::shutdown()
{

	// Delete light pointer
	delete mLight;

	// Force exit
	exit(1);

}

// Runs update logic
void SampleApp::update()
{

	// Update dynamics world
	mDynamicsWorld->update();

}

// Run application
CINDER_APP_BASIC(SampleApp, RendererGl)
