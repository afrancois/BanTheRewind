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
#include "FtpClient.h"

// Constructor
FtpClient::FtpClient()
{

	// Set flags
	mConnected = false;

}

// Destructor
FtpClient::~FtpClient()
{

	// Clean up
	delete [] mBuffer;

}

// Close connection
void FtpClient::close()
{

	try
	{

		// Close connection
		InternetCloseHandle(mInternetHnd);
		InternetCloseHandle(mFtpHnd);
		mConnected = false;
		OutputDebugStringA("FTP connection closed:\n");

	}
	catch (...)
	{

		// Report error
		showError();

	}

}

// Connect to server
bool FtpClient::connect(const string & url, const string & username, const string & password)
{

	// Close current connection
	if (mConnected) 
		close();

	// Get a handle on the Internet
	mInternetHnd = InternetOpenA("FtpClient", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (mInternetHnd == NULL)
	{

		// Report error and break
		OutputDebugStringA("Unable to connect to Internet:\n");
		showError();
		return false;

	}

	// Establish FTP connection
	mFtpHnd = InternetConnectA(mInternetHnd, url.c_str(), INTERNET_DEFAULT_FTP_PORT, username.c_str(), password.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_PASSIVE, 0x00000000);
	if (mFtpHnd == NULL)
	{

		// Report error, close, and break
		OutputDebugStringA("Unable to connect to FTP server:\n");
		showError();
		close();
		return false;

	}

	// Set connection flag
	mConnected = true;
	OutputDebugStringA("FTP connection established\n");

	// Return to stop thread
	return true;

}

// Reports error
void FtpClient::showError()
{

	// Get error
	DWORD errCode = GetLastError();

	// Internet error
	if (errCode == ERROR_INTERNET_EXTENDED_ERROR)
	{

		// Write error to buffer
		DWORD errLength = 1024;
		char errBuffer[1024];
		InternetGetLastResponseInfoA(&errCode, errBuffer, &errLength);

		// Get error as string
		OutputDebugStringA((LPCSTR)(LPCTSTR)errBuffer);
		OutputDebugStringA("\n");

		// Clear buffer
		LocalFree(errBuffer);

	}
	else
	{

		// Format message
		LPVOID errBuffer;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &errBuffer, 0, NULL);
		LPVOID dispBuffer = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)errBuffer) + 40) * sizeof(TCHAR)); 
		swprintf((LPTSTR)dispBuffer, LocalSize(dispBuffer) / sizeof(TCHAR), TEXT("Error %d: %s"), errCode, errBuffer); 

		// Output error
		OutputDebugStringA((LPCSTR)(LPCTSTR)dispBuffer);
		OutputDebugStringA("\n");

		// Clear buffers
		LocalFree(errBuffer);
		LocalFree(dispBuffer);

	}

}

// Upload file
bool FtpClient::upload(const string & source, const string & destination)
{

	// Check connection
	if (mFtpHnd == NULL)
	{

		// Report error and break
		OutputDebugStringA("Not connected to FTP server:\n");
		showError();
		return false;

	}

	// Check if file exists
	if (!boost::filesystem::exists(boost::filesystem::path(source)))
	{

		// Report error and break
		OutputDebugStringA((string() + "\"" + source + "\" does not exist").c_str());
		return false;

	}

	// Split destination to vector
	vector<string> mPath;
	boost::split(mPath, destination, boost::is_any_of("/"));

	// Iterate through directories
	LPCSTR mFilename;
	for (vector<string>::const_iterator mDirectory = mPath.cbegin(); mDirectory != mPath.cend(); ++mDirectory)
	{

		// End of the line
		if (mDirectory == mPath.end() - 1)
		{

			// Convert to character array
			mFilename = (LPCSTR)mDirectory->c_str();

		}
		else
		{

			// Convert to character array
			LPCSTR nDirectory = (LPCSTR)mDirectory->c_str();

			// Open directory
			if (!FtpSetCurrentDirectoryA(mFtpHnd, nDirectory))
			{

				// Create directory
				if (!FtpCreateDirectoryA(mFtpHnd, nDirectory))
				{

					// Report error and break
					OutputDebugStringA("Unable to create directory:\n");
					showError();
					return false;

				}
				else
				{

					// Open new directory
					if (!FtpSetCurrentDirectoryA(mFtpHnd, nDirectory))
					{

						// Report error and break
						OutputDebugStringA("Unable to open new directory:\n");
						showError();
						return false;

					}

				}

			}

		}

	}

	// Check for existing file
	LPWIN32_FIND_DATAA mInfo = LPWIN32_FIND_DATAA();
	HINTERNET mRemoteFileHnd = FtpFindFirstFileA(mFtpHnd, mFilename, mInfo, INTERNET_FLAG_RELOAD, 0);

	// Clean up memory
	LocalFree(mInfo);
	delete mInfo;

	// File exists
	if (mRemoteFileHnd != NULL)
	{
		
		// Warn
		OutputDebugStringA("File already exists. Overwriting.\n");

		// Close remote file
		InternetCloseHandle(mRemoteFileHnd);

		// Delete file
		if (!FtpDeleteFileA(mFtpHnd, mFilename))
		{

			// Report error and break
			OutputDebugStringA("Unable to delete file from server:\n");
			showError();
			mPath.clear();
			return false;

		}

	}

	// Open remote file for upload
	HINTERNET mFileHnd = FtpOpenFileA(mFtpHnd, mFilename, GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, 0);
	if (mFileHnd == NULL)
	{

		// Report error and break
		OutputDebugStringA("Unable to open remote file:\n");
		showError();
		return false;

	}

	// Open local file
	mFileIO.open(source.c_str(), ios::in|ios::binary|ios::ate);
	mFileSize = (DWORD)mFileIO.tellg();
	mFileIO.seekg(0, ios::beg);

	// Return flag
	bool mUploadSuccess = false;

	// Check file size
	if (mFileSize > 0)
	{

		// Allocate memory for buffer and read file
		mBuffer = new int_fast8_t[mFileSize];
		mFileIO.read(mBuffer, mFileSize);
		
		// Write buffer to remote file
		mBytesWritten = 0;
		mUploadSuccess = InternetWriteFile(mFileHnd, mBuffer, mFileSize, &mBytesWritten) == TRUE;
		if (!mUploadSuccess)
			showError();

		// Clean up
		delete [] mBuffer;

	}

	// Close file, clean up
	mFileIO.close();
	InternetCloseHandle(mFileHnd);
	mFileHnd = NULL;
	mPath.clear();

	// Write result
	OutputDebugStringA(mUploadSuccess ? "File uploaded.\n" : "Upload did not complete.\n");

	// Return flag
	return mUploadSuccess;

}
