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
#include "ciEmotiv.h"

// Imports
using namespace ci;
using namespace ci::app;
using namespace std;

// Constructor
ciEmotiv::Obj::Obj()
{

	// Set up target channel list
	mTargetChannelList[0] = ED_COUNTER;
	mTargetChannelList[1] = ED_AF3;
	mTargetChannelList[2] = ED_F7;
	mTargetChannelList[3] = ED_F3;
	mTargetChannelList[4] = ED_FC5;
	mTargetChannelList[5] = ED_T7;
	mTargetChannelList[6] = ED_P7;
	mTargetChannelList[7] = ED_O1;
	mTargetChannelList[8] = ED_O2;
	mTargetChannelList[9] = ED_P8;
	mTargetChannelList[10] = ED_T8;
	mTargetChannelList[11] = ED_FC6;
	mTargetChannelList[12] = ED_F4;
	mTargetChannelList[13] = ED_F8;
	mTargetChannelList[14] = ED_AF4;
	mTargetChannelList[15] = ED_GYROX;
	mTargetChannelList[16] = ED_GYROY;
	mTargetChannelList[17] = ED_TIMESTAMP;
	mTargetChannelList[18] = ED_FUNC_ID;
	mTargetChannelList[19] = ED_FUNC_VALUE;
	mTargetChannelList[20] = ED_MARKER;
	mTargetChannelList[21] = ED_SYNC_SIGNAL;

	// Initialize frequency data
	mFreqData = 0;
	mFftEnabled = true;
	mLastSampleTime = 0.0;
	mSampleTime = 1.0;

	// Initialize brainwave frequencies
	mAlpha = 0.0f;
	mBeta = 0.0f;
	mDelta = 0.0f;
	mGamma = 0.0f;
	mTheta = 0.0f;

	// Start update thread
	mRunning = true;
	mThread = boost::thread(&ciEmotiv::Obj::update, this);

}

// Destructor
ciEmotiv::Obj::~Obj()
{

	// Disconnect / clean up
	if (mConnected)
		disconnect();
	if (mFreqData != 0)
		delete [] mFreqData;

	// Stop thread
	mRunning = false;

}

// Connect to Emotiv Engine
bool ciEmotiv::Obj::connect(const string & deviceId, const string & remoteAddress, uint16_t port)
{

	try
	{

		// Connect to remote engine or composer or the local Emotiv Engine
		if (remoteAddress.length() > 0 && port > 0)
			mConnected = EE_EngineRemoteConnect(remoteAddress.c_str(), port, deviceId.c_str()) == EDK_OK;
		else
			mConnected = EE_EngineConnect(deviceId.c_str()) == EDK_OK;

		// Connected
		if (mConnected)
		{

			// Add event listening
			mEvent = EE_EmoEngineEventCreate();
			mState = EE_EmoStateCreate();

			// Set up EEG data sampler
			mData = EE_DataCreate();
			EE_DataSetBufferSizeInSec((float)mSampleTime);

		}

	}
	catch (...)
	{

		// Disconnect if routine fails
		mConnected = false;

	}

	// Return connected flag
	return mConnected;

}

// Disconnect from Emotiv Engine 
bool ciEmotiv::Obj::disconnect()
{

	// Disconnect and free resources
	try
	{
		if (EE_EngineDisconnect() != EDK_OK)
			return false;
		EE_EmoStateFree(mState);
		EE_EmoEngineEventFree(mEvent);
		EE_DataFree(mData);
		mConnected = false;
		return true;
	}
	catch (...)
	{
		return false;
	}

}

// Get number of connected devices
int32_t ciEmotiv::Obj::getNumUsers()
{

	// Retrieve and return number of connected devices
	uint32_t users = 0;
	EE_EngineGetNumUser(&users);
	return users;

}

// Load profile onto device
bool ciEmotiv::Obj::loadProfile(const string & profilePath, uint32_t userId)
{

	// Bail if not connected
	if (!mConnected)
		return false;

	// DO IT!
	try
	{
		if (EE_LoadUserProfile(userId, profilePath.c_str()) != EDK_OK)
			return false;
		return true;
	}
	catch (...)
	{
		return false;
	}

}

// List profiles
map<string, string> ciEmotiv::Obj::listProfiles(const string & dataPath)
{

	// Create return object
	map<string, string> profiles;
	
	// Iterate through all files in directory, adding all "EMU" files
	string dataDirectory = dataPath.length() > 0 ? dataPath : getAppPath() + "data";
	if (boost::filesystem::exists(dataDirectory))
		for (boost::filesystem::directory_iterator fileIt(dataDirectory), mEnd; fileIt != mEnd; ++fileIt)
			if (boost::to_lower_copy(fileIt->filename().substr(fileIt->filename().find_last_of("."))) == ".emu")
				profiles.insert(make_pair(fileIt->filename().substr(0, fileIt->filename().find_last_of(".")), fileIt->path().string()));

	// Return list
	return profiles;

}

// Main loop
void ciEmotiv::Obj::update()
{

	// Lock scope
	boost::mutex::scoped_lock lock(mMutex);

	// Check running flag
	while (mRunning)
	{

		// Check connection
		if (mConnected)
		{

			// We have a new event to process
			if (EE_EngineGetNextEvent(mEvent) == EDK_OK)
			{

				// Get user ID
				uint32_t userId;
				if (EE_EmoEngineEventGetUserId(mEvent, &userId) == EDK_OK)
				{

					// Get event type
					EE_Event_t eventType = EE_EmoEngineEventGetType(mEvent);

					// Enable data acquisition for new users
					if (eventType == EE_UserAdded)
						EE_DataAcquisitionEnable(userId, true);

					// Status update
					if (eventType == EE_EmoStateUpdated) 
					{

						// Get Emotiv state
						EE_EmoEngineEventGetEmoState(mEvent, mState);
						if (mState)
						{

							// Create map of Expresiv Suite results
							std::map<EE_ExpressivAlgo_t, float> expressivStates;
							EE_ExpressivAlgo_t upperFaceAction = ES_ExpressivGetUpperFaceAction(mState);
							float upperFacePower = ES_ExpressivGetUpperFaceActionPower(mState);
							EE_ExpressivAlgo_t lowerFaceAction = ES_ExpressivGetLowerFaceAction(mState);
							float lowerFacePower = ES_ExpressivGetLowerFaceActionPower(mState);
							expressivStates[upperFaceAction] = upperFacePower;
							expressivStates[lowerFaceAction] = lowerFacePower;

							// FFT analysis enabled and data has been sampled
							if (mFftEnabled && getElapsedSeconds() - mLastSampleTime >= mSampleTime)
							{

								// Reset brainwave frequencies
								mAlpha = 0.0f;
								mBeta = 0.0f;
								mDelta = 0.0f;
								mGamma = 0.0f;
								mTheta = 0.0f;

								// Update sample time
								mLastSampleTime = getElapsedSeconds();

								// Get raw EEG data
								EE_DataUpdateHandle(userId, mData);
								uint32_t samplesTaken = 0x00;
								EE_DataGetNumberOfSample(mData, &samplesTaken);
								if (samplesTaken != 0) 
								{

									// Populate data and add it to the mean
									float * meanData = new float[samplesTaken];
									double * data = new double[samplesTaken];
									for (int32_t i = 0; i < (int32_t)samplesTaken; i++)
										for (int32_t j = 0; j < sizeof(mTargetChannelList) / sizeof(EE_DataChannel_t); j++)
										{
											EE_DataGet(mData, mTargetChannelList[j], data, samplesTaken);
											if (i >= EEG_CHANNEL_BEGIN && i <= EEG_CHANNEL_END)
												meanData[i] += static_cast<float>(data[i]);
										}

									// Clean up data
									delete [] data;

									// Average and normalize values
									for (int32_t i = 0; i < (int32_t)samplesTaken; i++)
										meanData[i] = (float)((meanData[i] / (float)EEG_CHANNEL_COUNT) / (8.0 * 0x10000000));

									// Get FFT data
									mFft.setDataSize(samplesTaken);
									mFft.setData(meanData);
									mFreqData = mFft.getAmplitude();

									// Clean up
									delete [] meanData;

									// Need at least thirty windows
									int32_t dataSize = mFft.getBinSize();
									if (dataSize > 30)
									{

										// Delta
										for (int32_t i = 0; i < 4; i++)
											mDelta += mFreqData[i];
										mDelta /= 4.0f;

										// Theta
										for (int32_t i = 4; i < 8; i++)
											mTheta += mFreqData[i];
										mTheta /= 4.0f;

										// Alpha
										for (int32_t i = 8; i <= 13; i++)
											mAlpha += mFreqData[i];
										mAlpha /= 6.0f;

										// Beta
										for (int32_t i = 14; i < 30; i++)
											mBeta += mFreqData[i];
										mBeta /= 6.0f;

										// Gamma
										for (int32_t i = 30; i < (int32_t)samplesTaken; i++)
											mGamma += mFreqData[i];
										mGamma /= (float)(samplesTaken - 30);

									}

								}

							}

							// Dispatch event
							mSignal(EmotivEvent(
								ES_GetTimeFromStart(mState), 
								userId, 
								ES_GetWirelessSignalStatus(mState), 
								ES_ExpressivIsBlink(mState), 
								ES_ExpressivIsLeftWink(mState), 
								ES_ExpressivIsRightWink(mState), 
								ES_ExpressivIsLookingLeft(mState), 
								ES_ExpressivIsLookingRight(mState), 
								expressivStates[EXP_EYEBROW], 
								expressivStates[EXP_FURROW], 
								expressivStates[EXP_SMILE], 
								expressivStates[EXP_CLENCH], 
								expressivStates[EXP_SMIRK_LEFT], 
								expressivStates[EXP_SMIRK_RIGHT], 
								expressivStates[EXP_LAUGH], 
								ES_AffectivGetExcitementShortTermScore(mState), 
								ES_AffectivGetExcitementLongTermScore(mState), 
								ES_AffectivGetEngagementBoredomScore(mState), 
								ES_CognitivGetCurrentAction(mState), 
								ES_CognitivGetCurrentActionPower(mState), 
								mAlpha, 
								mBeta, 
								mDelta, 
								mGamma, 
								mTheta
								));

						}

					}

				}

			}

		}

	}

	// Unlock scope
	mMutex.unlock();

}

/****** EVENT ******/

// Constructor
EmotivEvent::Obj::Obj()
{

	// Set default values
	float mTime = 0.0f;
	uint32_t mUserId = 0x00;
	EE_SignalStrength_t mWirelessSignalStatus = EE_SignalStrength_t::NO_SIGNAL;
	int32_t mBlink = 0;
	int32_t mWinkLeft = 0;
	int32_t mWinkRight = 0;
	int32_t mLookLeft = 0;
	int32_t mLookRight = 0;
	float mEyebrow = 0.0f;
	float mFurrow = 0.0f;
	float mSmile = 0.0f;
	float mClench = 0.0f;
	float mSmirkLeft = 0.0f;
	float mSmirkRight = 0.0f;
	float mLaugh = 0.0f;
	float mShortTermExcitement = 0.0f;
	float mLongTermExcitement = 0.0f;
	float mEngagementBoredom = 0.0f;
	EE_CognitivAction_t mCognitivAction = EE_CognitivAction_enum::COG_NEUTRAL;
	float mCognitivPower = 0.0f;

}

// Destructor
EmotivEvent::Obj::~Obj()
{
}
