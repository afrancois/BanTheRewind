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
#include <cinder/Filesystem.h>
#include <jwsmtp/jwsmtp.h>

namespace cinder 
{ 

	namespace mail
	{

		// Authorization type
		typedef enum jwsmtp::mailer::authtype AuthType;

		// Message class
		class Message
		{

		private:

			// Internal object
			std::shared_ptr<jwsmtp::mailer> mObj;

		public:

			// Con/de-structor
			Message(
				const std::string & to = "", 
				const std::string & from = "", 
				const std::string & subject = "", 
				const std::string & message = "", 
				const std::string & server = "127.0.0.1", 
				int32_t port = 25, 
				bool mxLookUp = true);
			~Message();

			// Send the message
			void send();
			void send() const;

			// Reset
			void reset();

			// Get server response
			const std::string getResponse();
			const std::string getResponse() const;

			// Recipient
			bool addTo(const std::string & to);
			bool addTo(const std::vector<std::string> & to);
			void clearTo();
			bool removeTo(const std::string & to);
			bool removeTo(const std::vector<std::string> & to);

			// Attachments
			bool addAttachment(const std::string & path);
			bool addAttachments(const std::vector<std::string> & paths);
			bool addAttachment(const ci::fs::path & path);
			bool addAttachments(const std::vector<ci::fs::path> & paths);
			void clearAttachments();
			bool removeAttachment(const std::string & path);
			bool removeAttachments(const std::vector<std::string> & paths);
			bool removeAttachment(const ci::fs::path & path);
			bool removeAttachments(const std::vector<ci::fs::path> & paths);

			// Setters
			void setAuthentication(const AuthType & authType = AuthType::PLAIN, const std::string & username = "", const std::string & password = "");
			bool setFrom(const std::string & from);
			bool setMessage(const std::string & message, bool html = false);
			bool setServer(const std::string & server);
			bool setSubject(const std::string & subject);
			bool setTo(const std::string & to);

		};

	}

}
