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
#include <cinder/app/AppBasic.h>
#include <cinder/gl/Material.h>
#include <cinder/gl/Light.h>
#include <cinder/ip/Resize.h>
#include <cinder/ip/Flip.h>
#include <cinder/Camera.h>
#include <cinder/Capture.h>
#include <cinder/Channel.h>
#include <cinder/ImageIo.h>
#include <cinder/ObjLoader.h>
#include <cinder/Rand.h>
#include <cinder/Surface.h>
#include <cinder/TriMesh.h>
#include <cinder/Utilities.h>
#include <CinderBullet.h>
#include "Resources.h"

// Imports
using namespace ci;
using namespace ci::bullet;
using namespace ci::app;
using namespace std;

// Bullet physics sample application
class SampleApp : public AppBasic 
{

public:

	// Cinder callbacks
	void draw();
	void keyDown(KeyEvent event);
	void mouseDown(MouseEvent event);
	void mouseWheel(MouseEvent event);
	void resize(int32_t width, int32_t height);
	void setup();
	void shutdown();
	void update();
	
private:

	// Size of terrain image
	static const int32_t IMAGE_SIZE = 256;

	// Bullet
	bullet::DynamicsWorldRef mDynamicsWorld;
	
	// Gets heightmap from camera
	Capture mCapture;

	// Camera
	CameraPersp mCamera;

	// Shading
	gl::Material mMaterial;
	gl::Light * mLight;

	// Models
	TriMesh mConcave;
	TriMesh mConvex;

	// Wireframe flag
	bool mWireframe;

};
