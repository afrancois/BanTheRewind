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
#include <Mail.h>

namespace cinder 
{ 

	namespace mail
	{

		// Constructor
		Message::Message(const std::string & to, const std::string & from, const std::string & subject, const std::string & message, const std::string & server, int32_t port, bool mxLookUp)
		{

			// Initialize message
			if (to.length() > 0 && from.length() > 0)
				mObj = std::shared_ptr<jwsmtp::mailer>(new jwsmtp::mailer(to.c_str(), from.c_str(), subject.c_str(), message.c_str(), server.c_str(), (uint32_t)port, mxLookUp));

		}

		// Destructor
		Message::~Message()
		{

			// Reset internal object
			reset();
			if (mObj)
				mObj.reset();

		}

		// Send the message
		void Message::send() 
		{
			if (mObj)
				mObj->send();
		}
		void Message::send() const
		{
			if (mObj)
				mObj->send();
		}

		// Clear message
		void Message::reset()
		{
			if (mObj)
				mObj->reset();
		}

		// Get server response
		const std::string Message::getResponse() { return mObj ? mObj->response() : ""; }
		const std::string Message::getResponse() const { return mObj ? mObj->response() : ""; }

		// Recipient
		bool Message::addTo(const std::string & to) { return mObj ? mObj->addrecipient(to.c_str()) : false; }
		void Message::clearTo() 
		{ 
			if (mObj)
				mObj->clearrecipients(); 
		}
		bool Message::removeTo(const std::string & to) { return mObj ? mObj->removerecipient(to.c_str()) : false; }

		// Attachments
		bool Message::addAttachment(const std::string & filename) { return mObj ? mObj->attach(filename) : false; }
		bool Message::addAttachment(const ci::fs::path & path) { return addAttachment(path.string()); }
		void Message::clearAttachments() 
		{
			if (mObj)
				mObj->clearattachments();
		}
		bool Message::removeAttachment(const std::string & filename) { return mObj ? mObj->removeattachment(filename) : false; }
		bool Message::removeAttachment(const ci::fs::path & path) { return removeAttachment(path.string()); }

		// Setters
		void Message::setAuthentication(const AuthType & authType, const std::string & username, const std::string & password)
		{
			if (mObj)
			{
				mObj->authtype(authType);
				mObj->username(username);
				mObj->password(password);
			}
		}
		bool Message::setFrom(const std::string & from) { return mObj ? mObj->setsender(from) : false; }
		bool Message::setMessage(const std::string & message, bool html)
		{
			if (mObj)
			{
				if (html)
					return mObj->setmessageHTML(message);
				else
					return mObj->setmessage(message);
			}
			return false;
		}
		bool Message::setServer(const std::string & server) { return mObj ? mObj->setserver(server) : false; }
		bool Message::setSubject(const std::string & subject) { return mObj ? mObj->setsubject(subject) : false; }
		bool Message::setTo(const std::string & to)
		{
			if (mObj)
			{
				clearTo();
				return addTo(to);
			}
			return false;
		}

	}

}
