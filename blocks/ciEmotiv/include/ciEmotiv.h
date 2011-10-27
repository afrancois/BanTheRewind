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
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <cinder/app/App.h>
#include <cinder/Cinder.h>
#include <cinder/Utilities.h>
#include <emotiv/EmoStateDLL.h>
#include <emotiv/edk.h>
#include <emotiv/edkErrorCode.h>
#include <Kiss.h>

// Emotive event
class EmotivEvent
{

private:

	// The object
	struct Obj
	{

		// Con/de-structor
		Obj();
		~Obj();

		// Event properties
		float mTime;
		uint32_t mUserId;
		EE_SignalStrength_t mWirelessSignalStatus;
		int32_t mBlink;
		int32_t mWinkLeft;
		int32_t mWinkRight;
		int32_t mLookLeft;
		int32_t mLookRight;
		float mEyebrow;
		float mFurrow;
		float mSmile;
		float mClench;
		float mSmirkLeft;
		float mSmirkRight;
		float mLaugh;
		float mShortTermExcitement;
		float mLongTermExcitement;
		float mEngagementBoredom;
		EE_CognitivAction_t mCognitivAction;
		float mCognitivPower;
		float mAlpha;
		float mBeta;
		float mDelta;
		float mGamma;
		float mTheta;

	};

	// Object reference
	typedef std::shared_ptr<Obj> EmotivEventRef;
	EmotivEventRef mObj;

public:

	// Wireless signal constants
	static const int32_t BAD_SIGNAL = static_cast<int32_t>(EE_SignalStrength_t::BAD_SIGNAL);
	static const int32_t GOOD_SIGNAL = static_cast<int32_t>(EE_SignalStrength_t::GOOD_SIGNAL);
	static const int32_t NO_SIGNAL = static_cast<int32_t>(EE_SignalStrength_t::NO_SIGNAL);

	// Cognitive action constants
	static const int32_t COG_DISAPPEAR = static_cast<int32_t>(EE_CognitivAction_t::COG_DISAPPEAR);
	static const int32_t COG_DROP = static_cast<int32_t>(EE_CognitivAction_t::COG_DROP);
	static const int32_t COG_LEFT = static_cast<int32_t>(EE_CognitivAction_t::COG_LEFT);
	static const int32_t COG_LIFT = static_cast<int32_t>(EE_CognitivAction_t::COG_LIFT);
	static const int32_t COG_NEUTRAL = static_cast<int32_t>(EE_CognitivAction_t::COG_NEUTRAL);
	static const int32_t COG_PULL = static_cast<int32_t>(EE_CognitivAction_t::COG_PULL);
	static const int32_t COG_PUSH = static_cast<int32_t>(EE_CognitivAction_t::COG_PUSH);
	static const int32_t COG_RIGHT = static_cast<int32_t>(EE_CognitivAction_t::COG_RIGHT);
	static const int32_t COG_ROTATE_CLOCKWISE = static_cast<int32_t>(EE_CognitivAction_t::COG_ROTATE_CLOCKWISE);
	static const int32_t COG_ROTATE_COUNTER_CLOCKWISE = static_cast<int32_t>(EE_CognitivAction_t::COG_ROTATE_COUNTER_CLOCKWISE);
	static const int32_t COG_ROTATE_FORWARDS = static_cast<int32_t>(EE_CognitivAction_t::COG_ROTATE_FORWARDS);
	static const int32_t COG_ROTATE_LEFT = static_cast<int32_t>(EE_CognitivAction_t::COG_ROTATE_LEFT);
	static const int32_t COG_ROTATE_REVERSE = static_cast<int32_t>(EE_CognitivAction_t::COG_ROTATE_REVERSE);
	static const int32_t COG_ROTATE_RIGHT = static_cast<int32_t>(EE_CognitivAction_t::COG_ROTATE_RIGHT);

	// Con/de-structor
	EmotivEvent(
		float time = 0.0f, 
		uint32_t userId = 0x00, 
		EE_SignalStrength_t wirelessSignalStatus = EE_SignalStrength_t::NO_SIGNAL, 
		int32_t blink = 0, 
		int32_t winkLeft = 0, 
		int32_t winkRight = 0, 
		int32_t lookLeft = 0, 
		int32_t lookRight = 0, 
		float eyebrow = 0.0f, 
		float furrow = 0.0f, 
		float smile = 0.0f, 
		float clench = 0.0f, 
		float smirkLeft = 0.0f, 
		float smirkRight = 0.0f, 
		float laugh = 0.0f, 
		float shortTermExcitement = 0.0f, 
		float longTermExcitement = 0.0f, 
		float engagementBoredom = 0.0f, 
		EE_CognitivAction_t cognitivAction = EE_CognitivAction_enum::COG_NEUTRAL, 
		float cognitivPower = 0.0f, 
		float alpha = 0.0f, 
		float beta = 0.0f, 
		float delta = 0.0f, 
		float gamma = 0.0f, 
		float theta = 0.0f
		) 
	{
		mObj = EmotivEventRef(new Obj());
		mObj->mAlpha = alpha;
		mObj->mBeta = beta;
		mObj->mBlink = blink;
		mObj->mClench = clench;
		mObj->mCognitivAction = cognitivAction;
		mObj->mCognitivPower = cognitivPower;
		mObj->mDelta = delta;
		mObj->mEngagementBoredom = engagementBoredom;
		mObj->mEyebrow = eyebrow;
		mObj->mFurrow = furrow;
		mObj->mGamma = gamma;
		mObj->mLaugh = laugh;
		mObj->mLongTermExcitement = longTermExcitement;
		mObj->mLookLeft = lookLeft;
		mObj->mLookRight = lookRight;
		mObj->mShortTermExcitement = shortTermExcitement;
		mObj->mSmile = smile;
		mObj->mSmirkLeft = smirkLeft;
		mObj->mSmirkLeft = smirkRight;
		mObj->mTheta = theta;
		mObj->mTime = time;
		mObj->mUserId = userId;
		mObj->mWinkLeft = winkLeft;
		mObj->mWinkRight = winkRight;
		mObj->mWirelessSignalStatus = wirelessSignalStatus;
	}
	~EmotivEvent() {}

	// Getters
	float getTime() { return mObj->mTime; }
	uint32_t getUserId() { return mObj->mUserId; }
	int32_t getWirelessSignalStatus() { return static_cast<int32_t>(mObj->mWirelessSignalStatus); }
	int32_t getBlink() { return mObj->mBlink; }
	int32_t getWinkLeft() { return mObj->mWinkLeft; }
	int32_t getWinkRight() { return mObj->mWinkRight; }
	int32_t getLookLeft() { return mObj->mLookLeft; }
	int32_t getLookRight() { return mObj->mLookRight; }
	float getEyebrow() { return mObj->mEyebrow; }
	float getFurrow() { return mObj->mFurrow; }
	float getSmile() { return mObj->mSmile; }
	float getClench() { return mObj->mClench; }
	float getSmirkLeft() { return mObj->mSmirkLeft; }
	float getSmirkRight() { return mObj->mSmirkRight; }
	float getLaugh() { return mObj->mLaugh; }
	float getShortTermExcitement() { return mObj->mShortTermExcitement; }
	float getLongTermExcitement() { return mObj->mLongTermExcitement; }
	float getEngagementBoredom() { return mObj->mEngagementBoredom; }
	int32_t getCognitivAction() { return static_cast<int32_t>(mObj->mCognitivAction); }
	float getCognitivPower() { return mObj->mCognitivPower; }
	float getAlpha() { return mObj->mAlpha; }
	float getBeta() { return mObj->mBeta; }
	float getDelta() { return mObj->mDelta; }
	float getGamma() { return mObj->mGamma; }
	float getTheta() { return mObj->mTheta; }

};

// Emotiv wrapper
class ciEmotiv
{

private:

	// Callback alias
	typedef boost::signals2::connection Callback;
	typedef std::shared_ptr<Callback> CallbackRef;
	typedef std::map<int32_t, CallbackRef> CallbackList;

	// EEG channels to use in FFT calculation
	static const int32_t EEG_CHANNEL_BEGIN = 1;
	static const int32_t EEG_CHANNEL_COUNT = 14;
	static const int32_t EEG_CHANNEL_END = 14;

	// The object
	struct Obj
	{

		// Con/de-structor
		Obj();
		~Obj();

		// Dis/connect
		bool connect(const std::string & deviceId, const std::string & remoteAddress, uint16_t port);
		bool disconnect();
		int32_t getNumUsers();

		// Callback list
		boost::signals2::signal<void (EmotivEvent)> mSignal;
		CallbackList mCallbacks;

		// Profiles
		std::map<std::string, std::string> listProfiles(const std::string & dataPath);
		bool loadProfile(const std::string & profilePath, uint32_t userId);

		// Event handlers
		EmoEngineEventHandle mEvent;
		EmoStateHandle mState;

		// Connected status
		bool mConnected;

		// Raw EEG data, FFT
		DataHandle mData;
		Kiss mFft;
		bool mFftEnabled;
		float * mFreqData;
		EE_DataChannel_t mTargetChannelList[22];
		double mSampleTime;
		double mLastSampleTime;

		// Brainwave frequencies
		float mAlpha;
		float mBeta;
		float mDelta;
		float mGamma;
		float mTheta;

		// Threading
		boost::mutex mMutex;
		bool mRunning;
		boost::thread mThread;
		void update();

	};

	// Pointer to object
	typedef std::shared_ptr<Obj> EmotivRef;
	EmotivRef mObj;

public:

	// Emotiv Composer port number
	static const uint16_t COMPOSER_PORT = 1726;
	static const uint16_t REMOTE_PORT = 3008;

	// Con/de-structor
	ciEmotiv() : mObj(EmotivRef(new Obj())) {}
	~ciEmotiv() {}

	// Dis/connect
	bool connect(const std::string & deviceId = "Emotiv Systems-5", const std::string & remoteAddress = "", uint16_t port = 0) { return mObj->connect(deviceId, remoteAddress, port); }
	bool connected() { return mObj->mConnected; }
	bool disconnect() { return mObj->disconnect(); }
	int32_t getNumUsers() { return mObj->getNumUsers(); }

	// Dis/en-able FFT
	void enableFft(bool enabled) { mObj->mFftEnabled = enabled; }
	bool fftEnabled() { return mObj->mFftEnabled; }

	// Profiles
	std::map<std::string, std::string> listProfiles(const std::string & dataPath = "") { return mObj->listProfiles(dataPath); }
	bool loadProfile(const std::string & profilePath, uint32_t userId = 0x00) { return mObj->loadProfile(profilePath, userId); }

	// Add callback
	template<typename T>
	int32_t addCallback(void (T::* callbackFunction)(EmotivEvent event), T * callbackObject) 
	{

		// Determine return ID
		int32_t mCallbackID = mObj->mCallbacks.empty() ? 0 : mObj->mCallbacks.rbegin()->first + 1;

		// Create callback and add it to the list
		mObj->mCallbacks.insert(make_pair(mCallbackID, CallbackRef(new Callback(mObj->mSignal.connect(boost::function<void (EmotivEvent)>(boost::bind(callbackFunction, callbackObject, ::_1)))))));

		// Return callback ID
		return mCallbackID;

	}

	// Removes callback
	void removeCallback(int32_t callbackID) 
	{

		// Disconnect the callback connection
		mObj->mCallbacks.find(callbackID)->second->disconnect();

		// Remove the callback from the list
		mObj->mCallbacks.erase(callbackID); 

	}

};
