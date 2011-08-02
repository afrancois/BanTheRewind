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
#include "ciFlyCap.h"

/****** CON/DE-STRUCTORS ******/

ciFlyCap::Obj::Obj()
{

	// Set flags
	mCapturing = false;
	mNewFrame = false;

	// Set default properties
	mCamera = 0;
	mChannelCount = 3;
	mDeviceId = 0;
	mSurfaceChannelOrder = SurfaceChannelOrder::RGB;

	// Get camera count
	mErr = mBusManager.GetNumOfCameras(&mCameraCount);
	if (mErr != PGRERROR_OK)
		showError();

}

// Destructor
ciFlyCap::Obj::~Obj()
{
	stop();
}



/****** PUBLIC METHODS ******/

// Get new frame flag
bool ciFlyCap::Obj::checkNewFrame()
{
	return mNewFrame;
}

// Connect to camera
bool ciFlyCap::Obj::connect()
{

	// Get camera GUID using device index
	mErr = mBusManager.GetCameraFromIndex(mDeviceId, &mGuid);
    if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

    // Connect to the camera
	mCamera = new FlyCapture2::Camera();
    mErr = mCamera->Connect(&mGuid);
    if (mErr != PGRERROR_OK)
	{
		showError();
		mCamera = 0;
		return false;
	}

	// Get camera info
	mErr = mCamera->GetCameraInfo(&mCameraInfo);
	if (mErr != PGRERROR_OK)
	{
		showError();
		mCamera = 0;
		return false;
	}

	// Return true if no error
	return true;

}

// Get capturing flag
bool ciFlyCap::Obj::isCapturing()
{
	return mCapturing;
}

// Get connection status
bool ciFlyCap::Obj::isConnected()
{

	// False is no camera
	if (mCamera == 0)
		return false;

	// Connection status
	return mCamera->IsConnected();

}

// Get init flag
bool ciFlyCap::Obj::isInitialized()
{
	return mCamera != 0;
}

// Save image to file
bool ciFlyCap::Obj::save(const string & filename)
{

	// Bail if no camera
	if (mCamera == 0)
		return false;

	// Save image
	mErr = mRawImage.Save(filename.c_str());
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}
	return true;

}

// Start capturing
bool ciFlyCap::Obj::start()
{

	// Free resources first if already capturing
	if (mCapturing)
		stop();

	// Connect, if needed
	if (!isConnected())
		connect();

	// Bail if no camera
	if (mCamera == 0)
		return false;

	// Initialize surface
	mSurface.reset();
	mSurface = Surface8u(mRawImage.GetCols(), mRawImage.GetRows(), false, mSurfaceChannelOrder);

	// Start capture
	mErr = mCamera->StartCapture();
    if (mErr != PGRERROR_OK)
	{
		showError();
		stop();
		return false;
	}

	// Default to application frame rate
	mProperty.type = PropertyType::FRAME_RATE;
	mErr = mCamera->GetProperty(&mProperty);
	if (mErr != PGRERROR_OK)
	{
		mProperty.absControl = true;
		mProperty.absValue = app::getFrameRate();
		mErr = mCamera->SetProperty(&mProperty);
	}

	// Run update in separate thread
	mCapturing = true;
	mNewFrame = false;
	thread(&ciFlyCap::Obj::update, this);

	// Return true if no error
	return true;

}

// Stop capturing
void ciFlyCap::Obj::stop()
{

	// Check if we need to stop
	if (mCamera != 0 && mCamera->IsConnected())
	{

		// Turn off flags
		mCapturing = false;
		mNewFrame = false;

		// Stop capturing images
		mErr = mCamera->StopCapture();
		if (mErr != PGRERROR_OK)
			return showError();

		// Disconnect the camera
		mErr = mCamera->Disconnect();
		if (mErr != PGRERROR_OK)
			return showError();

		// Free resources
		mSurface.reset();
		mRawImage.ReleaseBuffer();
		delete mCamera;
		mCamera = 0;

	}

}



/****** GETTERS ******/

// Get bandwidth allocation
BandwidthAllocation ciFlyCap::Obj::getBandwidthAllocation() 
{

	// Bail if no camera available
	if (mCamera == 0)
		return BandwidthAllocation::BANDWIDTH_ALLOCATION_UNSPECIFIED;
	
	// Update configuration
	mErr = mCamera->GetConfiguration(&mFC2Config);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return BandwidthAllocation::BANDWIDTH_ALLOCATION_UNSPECIFIED;
	}
	
	// Return value
	return mFC2Config.bandwidthAllocation; 

}

// Get Bayer tile format
BayerTileFormat ciFlyCap::Obj::getBayerTileFormat() 
{

	// Return format from image
	return mRawImage.GetBayerTileFormat();

}

// Get number of cameras attached on this interface
uint32_t ciFlyCap::Obj::getCameraCount() 
{ 
	return mCameraCount; 
}

// Get camera info object
CameraInfo ciFlyCap::Obj::getCameraInfo() 
{

	// Return empty info if no camera
	if (mCamera == 0)
		return CameraInfo();

	// Get info
	mErr = mCamera->GetCameraInfo(&mCameraInfo);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return CameraInfo();
	}
	return mCameraInfo; 

}

// Get color processing algorithm
ColorProcessingAlgorithm ciFlyCap::Obj::getColorProcessingAlgorithm() 
{ 

	// Return value from image
	return mRawImage.GetColorProcessing();

}

// Get device ID (index)
int32_t ciFlyCap::Obj::getDeviceId() 
{ 
	return mDeviceId; 
}

// Get grab mode value
GrabMode ciFlyCap::Obj::getGrabMode() 
{

	// Bail if no camera available
	if (mCamera == 0)
		return GrabMode::UNSPECIFIED_GRAB_MODE;
	
	// Update configuration
	mErr = mCamera->GetConfiguration(&mFC2Config);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return GrabMode::UNSPECIFIED_GRAB_MODE;
	}
	
	// Return value
	return mFC2Config.grabMode; 

}

// Get grab timrout value
int32_t ciFlyCap::Obj::getGrabTimeout() 
{

	// Bail if no camera available
	if (mCamera == 0)
		return GrabTimeout::TIMEOUT_UNSPECIFIED;
	
	// Update configuration
	mErr = mCamera->GetConfiguration(&mFC2Config);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return GrabTimeout::TIMEOUT_UNSPECIFIED;
	}
	
	// Return value
	return (int32_t)mFC2Config.grabTimeout;

}

// Get image height
int32_t	ciFlyCap::Obj::getHeight() 
{ 

	// Return row count
	return mRawImage.GetRows();

}

// Get pixel format
PixelFormat ciFlyCap::Obj::getPixelFormat() 
{

	// Return format from image
	return mRawImage.GetPixelFormat();

}

// Retrive property value
float ciFlyCap::Obj::getProperty(const PropertyType & propertyType) 
{

	// Get property
	mProperty.type = propertyType;
	mErr = mCamera->GetProperty(&mProperty);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return -1;
	}

	// Get property info
	mPropertyInfo.type = propertyType;
	mErr = mCamera->GetPropertyInfo(&mPropertyInfo);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return -1;
	}

	// Return value
	return mPropertyInfo.absValSupported ? mProperty.absValue : static_cast<float>(mProperty.valueA);

}

// Retrive property range
Vec2f ciFlyCap::Obj::getPropertyRange(const PropertyType & propertyType, bool absMode) 
{

	// Create return vector
	Vec2f mRange = Vec2f::zero();

	// Get property info
	mPropertyInfo.type = propertyType;
	mErr = mCamera->GetPropertyInfo(&mPropertyInfo);
	if (mErr != PGRERROR_OK)
	{
		showError();
	}
	else
	{
		mRange.x = absMode ? mPropertyInfo.absMin : mPropertyInfo.min;
		mRange.y = absMode ? mPropertyInfo.absMax : mPropertyInfo.max;
	}

	// Return range
	return mRange;

}

// Get camera image as Surface
Surface8u ciFlyCap::Obj::getSurface() 
{ 
	return mSurface; 
}

// Returns video mode
VideoMode ciFlyCap::Obj::getVideoMode() 
{

	// Bail if no camera
	if (mCamera == 0)
		return VideoMode::NUM_VIDEOMODES;

	// Get current values
	mErr = mCamera->GetVideoModeAndFrameRate(&mVideoMode, &mFrameRate);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return VideoMode::NUM_VIDEOMODES;
	}

	// Return video mode
	return mVideoMode;

}

// Get image width
int32_t ciFlyCap::Obj::getWidth() 
{ 

	// Return raw image width
	return mRawImage.GetCols(); 

}



/****** SETTERS ******/

// Set bandwidth allocation
bool ciFlyCap::Obj::setBandwidthAllocation(const BandwidthAllocation & value) 
{

	// Return false if not changing value or no camera
	if (mCamera == 0 || mFC2Config.bandwidthAllocation == value)
		return false;

	// Update value
	mFC2Config.bandwidthAllocation = value;
	mErr = mCamera->SetConfiguration(&mFC2Config);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Return true if no error
	return true;

}

// Set Bayer tile format
bool ciFlyCap::Obj::setBayerTileFormat(const BayerTileFormat & value) 
{

	// Return false if not changing value or no camera
	if (mRawImage.GetBayerTileFormat() == value)
		return false;

	// Update value
	mErr = mRawImage.SetDimensions(mRawImage.GetRows(), mRawImage.GetCols(), mRawImage.GetStride(), mRawImage.GetPixelFormat(), value);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Return true if no error
	return true;

}

// Set color processing algorithm
bool ciFlyCap::Obj::setColorProcessingAlgorithm(const ColorProcessingAlgorithm & value) 
{

	// Return false if not changing value
	if (mRawImage.GetColorProcessing() == value)
		return false;

	// Set value
	mErr = mRawImage.SetDefaultColorProcessing(value);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Return true if no error
	return true;

}

// Set device index
bool ciFlyCap::Obj::setDeviceId(int32_t deviceId)
{

	// Return if ID is not changing
	if (deviceId == mDeviceId)
		return false;

	// ID is out of range
	if (deviceId < 0 || deviceId >= (int32_t)mCameraCount)
		return false;

	// Update device ID
	mDeviceId = deviceId;

	// Stop camera if it is currently running, just connect otherwise
	if (mCapturing || mCamera == 0 || mCamera->IsConnected())
	{
		stop();
		return start();
	}

	// Connect at new device ID
	return connect();

}

// Set grab mode
bool ciFlyCap::Obj::setGrabMode(const GrabMode & value) 
{
	
	// Return false if not changing value
	if (mCamera == 0 || mFC2Config.grabMode == value)
		return false;

	// Update value
	mFC2Config.grabMode = value;
	mErr = mCamera->SetConfiguration(&mFC2Config);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Return true if no error
	return true;

}

// Set grab timeout (delayed capture)
bool ciFlyCap::Obj::setGrabTimeout(const GrabTimeout & value) 
{

	// Return false if not changing value
	if (mCamera == 0 || mFC2Config.grabTimeout == value)
		return false;

	// Update value
	mFC2Config.grabTimeout = value;
	mErr = mCamera->SetConfiguration(&mFC2Config);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Return true if no error
	return true;

}

// Set pixel format
bool ciFlyCap::Obj::setPixelFormat(const PixelFormat & value) 
{

	// Return false if not changing value or no camera
	if (mRawImage.GetPixelFormat() == value)
		return false;

	// Store values in case we need to restore them
	SurfaceChannelOrder mPrevSurfaceChannelOrder = mSurfaceChannelOrder;
	int32_t mPrevChannelCount = mChannelCount;
	mChannelCount = -1;

	// Key on pixel format
	switch (value)
	{
	case PixelFormat::PIXEL_FORMAT_411YUV8:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_422YUV8:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_444YUV8:
		mSurfaceChannelOrder = SurfaceChannelOrder::BGR;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_BGR:
		mSurfaceChannelOrder = SurfaceChannelOrder::BGR;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_BGRU:
		mSurfaceChannelOrder = SurfaceChannelOrder::BGRA;
		mChannelCount = 4;
		break;
	case PixelFormat::PIXEL_FORMAT_MONO12:
		mSurfaceChannelOrder = SurfaceChannelOrder::CHAN_RED;
		mChannelCount = 1;
		break;
	case PixelFormat::PIXEL_FORMAT_MONO16:
		mSurfaceChannelOrder = SurfaceChannelOrder::CHAN_RED;
		mChannelCount = 1;
		break;
	case PixelFormat::PIXEL_FORMAT_MONO8:
		mSurfaceChannelOrder = SurfaceChannelOrder::CHAN_RED;
		mChannelCount = 1;
		break;
	case PixelFormat::PIXEL_FORMAT_RAW12:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_RAW16:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_RAW8:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_RGB16:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_RGB8:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_RGBU:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGBA;
		mChannelCount = 3;
		break;
	case PixelFormat::PIXEL_FORMAT_S_MONO16:
		mSurfaceChannelOrder = SurfaceChannelOrder::CHAN_RED;
		mChannelCount = 1;
		break;
	case PixelFormat::PIXEL_FORMAT_S_RGB16:
		mSurfaceChannelOrder = SurfaceChannelOrder::RGB;
		mChannelCount = 3;
		break;
	case PixelFormat::UNSPECIFIED_PIXEL_FORMAT:
		mSurfaceChannelOrder = SurfaceChannelOrder::UNSPECIFIED;
		mChannelCount = -1;
		break;
	}

	// If input value format is unspecifed, restore values and bail
	if (mChannelCount < 0)
	{
		mChannelCount = mPrevChannelCount;
		mSurfaceChannelOrder = mPrevSurfaceChannelOrder;
		return false;
	}

	// Update dimensions
	mErr = mRawImage.SetDimensions(mRawImage.GetRows(), mRawImage.GetCols(), mRawImage.GetStride(), value, mRawImage.GetBayerTileFormat());
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Re-start input if we're previously capturing
	if (mCapturing)
	{
		stop();
		return start();
	}

	// Just return if we weren't already capturing
	return true;

}

// Set image property
bool ciFlyCap::Obj::setProperty(const PropertyType & propertyType, float value, bool absMode) 
{

	// Check camera
	if (mCamera == 0)
		return false;

	// Get property value from camera
	mProperty.type = propertyType;
	mErr = mCamera->GetProperty(&mProperty);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Get property info from camera
	mPropertyInfo.type = propertyType;
	mErr = mCamera->GetPropertyInfo(&mPropertyInfo);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}
	
	// Turn off auto for this property so we can change it
	mProperty.autoManualMode = false;

	// White balance is not supported in this method 
	// because it requires two values
	if (mProperty.type != WHITE_BALANCE)
    {
		
		// Abs mode
		if (absMode && mPropertyInfo.absValSupported)
		{

			// Set value
			mProperty.absValue = value;
			mProperty.absControl = true;

		}
		else
		{

			// Set value
			mProperty.valueA = (uint32_t)value;
			mProperty.absControl = false;

		}

    }
	else
	{

		// Not supported
		return false;

	}

	// Apply property to camera
	mErr = mCamera->SetProperty(&mProperty, false);
    if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Success has three S's
	return true;

}

// Set size
bool ciFlyCap::Obj::setSize(const Vec2i & size)
{

	// Bail if size is not changing
	if (size.x == mRawImage.GetCols() && size.y == mRawImage.GetRows())
		return false;

	// Update size
	mErr = mRawImage.SetDimensions(size.y, size.x, mRawImage.GetStride(), mRawImage.GetPixelFormat(), mRawImage.GetBayerTileFormat());
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}
	return true;

}

// Update video mode
bool ciFlyCap::Obj::setVideoMode(const VideoMode & value) 
{

	// Check camera
	if (mCamera == 0)
		return false;

	// Get current values
	mErr = mCamera->GetVideoModeAndFrameRate(&mVideoMode, &mFrameRate);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Update video mode
	mErr = mCamera->SetVideoModeAndFrameRate(value, mFrameRate);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}
	return true;

}

bool ciFlyCap::Obj::setWhiteBalance(uint32_t red, uint32_t blue)
{

	// Check camera
	if (mCamera == 0)
		return false;

	// Get property value from camera
	mProperty.type = PropertyType::WHITE_BALANCE;
	mErr = mCamera->GetProperty(&mProperty);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Get property info from camera
	mPropertyInfo.type = PropertyType::WHITE_BALANCE;
	mErr = mCamera->GetPropertyInfo(&mPropertyInfo);
	if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}
	
	// Turn off auto for this property so we can change it
	mProperty.autoManualMode = false;

	// Set values
	mProperty.valueA = red;
	mProperty.valueB = blue;
	mProperty.absControl = false;

	// Apply property to camera
	mErr = mCamera->SetProperty(&mProperty, false);
    if (mErr != PGRERROR_OK)
	{
		showError();
		return false;
	}

	// Success has three S's
	return true;

}



/****** PRIVATE METHODS ******/

// Show error
void ciFlyCap::Obj::showError()
{
	console() << mErr.GetDescription() << "\n";
}

// Runs update logic
void ciFlyCap::Obj::update()
{

	// Check capturing flag
	while (mCapturing)
	{

		// Turn off flag
		mNewFrame = false;

		// Retrieve an image
		mErr = mCamera->RetrieveBuffer(&mRawImage);
		if (mErr != PGRERROR_OK)
			return showError();

		// Copy buffer to surface (faster way to do this?)
		memcpy(mSurface.getData(), mRawImage.GetData(), mRawImage.GetCols() * mRawImage.GetRows() * mChannelCount);

		// Turn on new frame flag
		mNewFrame = true;

		// Sleep for application frame interval or this shit gets bananas
		Sleep((DWORD)(1000.0f / app::getFrameRate()));

	}

}
