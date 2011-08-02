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
#include "boost/filesystem.hpp"
#include "cinder/Cinder.h"
#include "cinder/CinderMath.h"
#include "cinder/app/App.h"
#include "cinder/Surface.h"
#include "cinder/Thread.h"
#include "cinder/Vector.h"
#include "FlyCapture2/FlyCapture2.h"

// Imports
using namespace ci;
using namespace ci::app;
using namespace FlyCapture2;
using namespace std;

// FlyCapyure wrapper
class ciFlyCap
{

private:

	// The object
	class Obj
	{

	public:

		// Con/de-structor
		Obj();
		~Obj();

		// Methods
		bool checkNewFrame();
		bool connect();
		bool isCapturing();
		bool isConnected();
		bool isInitialized();
		bool save(const string & filename);
		bool start();
		void stop();

		// Getters
		BandwidthAllocation getBandwidthAllocation();
		BayerTileFormat getBayerTileFormat();
		uint32_t getCameraCount();
		CameraInfo getCameraInfo();
		ColorProcessingAlgorithm getColorProcessingAlgorithm();
		int32_t getDeviceId();
		GrabMode getGrabMode();
		int32_t getGrabTimeout();
		int32_t	getHeight();
		InterfaceType getInterfaceType();
		PixelFormat getPixelFormat();
		float getProperty(const PropertyType & value);
		Vec2f getPropertyRange(const PropertyType & value, bool absMode);
		Surface8u getSurface();
		VideoMode getVideoMode();
		int32_t getWidth();

		// Setters
		bool setBandwidthAllocation(const BandwidthAllocation & value);
		bool setBayerTileFormat(const BayerTileFormat & value);
		bool setColorProcessingAlgorithm(const ColorProcessingAlgorithm & value);
		bool setDeviceId(int32_t deviceId);
		bool setGrabMode(const GrabMode & value);
		bool setGrabTimeout(const GrabTimeout & value);
		bool setPixelFormat(const PixelFormat & value);
		bool setProperty(const PropertyType & propertyType, float value, bool absMode);
		bool setSize(const Vec2i & size);
		bool setVideoMode(const VideoMode & value);
		bool setWhiteBalance(uint32_t red, uint32_t blue);

	private:

		// Writes PGRERROR messages to console
		void showError();

		// Device ID
		int32_t mDeviceId;

		// Frame
		int32_t mChannelCount;
		bool mCapturing;
		bool mNewFrame;
		SurfaceChannelOrder mSurfaceChannelOrder;
		Surface8u mSurface;
		void update();

		// FlyCapture objects
		BusManager mBusManager;
		FlyCapture2::Camera * mCamera;
		uint32_t mCameraCount;
		CameraInfo mCameraInfo;
		Error mErr;
		FC2Config mFC2Config;
		FrameRate mFrameRate;
		PGRGuid mGuid;
		Property mProperty;
		PropertyInfo mPropertyInfo;
		Image mRawImage;
		VideoMode mVideoMode;

	};

	// Pointer to object
	std::shared_ptr<Obj> mObj;

public: 

	// Con/de-structor
	ciFlyCap() {}
	ciFlyCap(int32_t width, int32_t height, int32_t deviceId = 0) 
	{ 
		mObj = std::shared_ptr<Obj>(new Obj()); 
		mObj->setSize(Vec2i(width, height));
		mObj->setDeviceId(deviceId);
	}
	~ciFlyCap() { mObj.reset(); }

	// Methods
	bool checkNewFrame() { return mObj ? mObj->checkNewFrame() : false; }
	bool connect() { return mObj ? mObj->connect() : false; }
	bool isCapturing() { return mObj ? mObj->isCapturing() : false; }
	bool isInitialized() { return mObj ? mObj->isInitialized() : false; }
	bool isConnected() { return mObj ? mObj->isConnected() : false; }
	bool save(string filename) { return mObj ? mObj->save(filename) : false; }
	bool start() { return mObj ? mObj->start() : false; }
	void stop() 
	{ 
		if (mObj) 
			mObj->stop(); 
	}

	// Getters
	float getAspectRatio() { return (float)mObj->getWidth() / (float)mObj->getHeight(); }
	BandwidthAllocation getBandwidthAllocation() { return mObj->getBandwidthAllocation(); }
	BayerTileFormat getBayerTileFormat() { return mObj->getBayerTileFormat(); }
	Area getBounds() { return Area(0, 0, mObj->getWidth(), mObj->getHeight()); }
	uint32_t getCameraCount() { return mObj->getCameraCount(); }
	CameraInfo getCameraInfo() { return mObj->getCameraInfo(); }
	ColorProcessingAlgorithm getColorProcessingAlgorithm() { return mObj->getColorProcessingAlgorithm(); }
	int32_t getDeviceId() const { return mObj->getDeviceId(); }
	GrabMode getGrabMode() { return mObj->getGrabMode(); }
	int32_t getGrabTimeout() { return mObj->getGrabTimeout(); }
	int32_t getHeight() const { return mObj->getHeight(); }
	InterfaceType getInterfaceType() { return mObj->getInterfaceType(); }
	PixelFormat getPixelFormat() { return mObj->getPixelFormat(); }
	float getProperty(const PropertyType & value) { return mObj->getProperty(value); }
	Vec2f getPropertyRange(const PropertyType & value, bool absMode = true) { return mObj->getPropertyRange(value, absMode); }
	Surface8u getSurface() const { return mObj->getSurface(); }
	VideoMode getVideoMode() { return mObj->getVideoMode(); }
	Vec2i getSize() { return Vec2i(mObj->getWidth(), mObj->getHeight()); }
	int32_t getWidth() const { return mObj->getWidth(); }

	// Setters
	bool setBandwidthAllocation(const BandwidthAllocation & value = BandwidthAllocation::BANDWIDTH_ALLOCATION_ON) { return mObj ? mObj->setBandwidthAllocation(value) : false; }
	bool setBayerTileFormat(const BayerTileFormat & value = BayerTileFormat::NONE) { return mObj ? mObj->setBayerTileFormat(value) : false; }
	bool setColorProcessingAlgorithm(const ColorProcessingAlgorithm & value = ColorProcessingAlgorithm::DEFAULT) { return mObj ? mObj->setColorProcessingAlgorithm(value) : false; }
	bool setDeviceId(int32_t deviceId) 
	{
		if (mObj)
		{
			if (mObj->isCapturing())
			{
				mObj->stop();
				mObj->setDeviceId(deviceId);
				return mObj->start();
			}
			return mObj->setDeviceId(deviceId);
		}
		else
		{
			return false;
		}
	}
	bool setGrabMode(const GrabMode & value = GrabMode::DROP_FRAMES) { return mObj ? mObj->setGrabMode(value) : false; }
	bool setGrabTimeout(const GrabTimeout & value = GrabTimeout::TIMEOUT_UNSPECIFIED) { return mObj ? mObj->setGrabTimeout(value) : false; }
	bool setPixelFormat(const PixelFormat & value = PixelFormat::PIXEL_FORMAT_RGB) { return mObj ? mObj->setPixelFormat(value) : false; }
	bool setProperty(const PropertyType & propertyType, float value = 0.0f, bool absMode = true) { return mObj ? mObj->setProperty(propertyType, value, absMode) : false; }
	bool setSize(const Vec2i & size) 
	{
		if (mObj)
		{
			if (mObj->isCapturing())
			{
				mObj->stop();
				mObj->setSize(size);
				return mObj->start();
			}
			return mObj->setSize(size);
		}
		else
		{
			return false;
		}
	}
	bool setVideoMode(const VideoMode & value = VideoMode::VIDEOMODE_640x480RGB) { return mObj ? mObj->setVideoMode(value) : false; }
	bool setWhiteBalance(uint32_t red, uint32_t blue) { return mObj ? mObj->setWhiteBalance(red, blue) : false; }

};
