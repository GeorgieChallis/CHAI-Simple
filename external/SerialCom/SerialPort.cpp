#include "SerialPort.h"
#include <iostream>
#include <stdio.h>

//#include "Common.h"


// ###################################################################################################
// Serial Port

// Constructors & Destructor

SerialPort::SerialPort()
	: connected(false), COMport(COM_PORT) {}

SerialPort::SerialPort(const char* _COMport)
	: connected(false), COMport(_COMport) {}

SerialPort::~SerialPort()
{
	this->disconnect();

	if (this->listenerThread != nullptr)
	{
		this->listenerThread->join();
		delete this->listenerThread;
	}	
}

// Create HANDLE and connect to specified COMport
bool SerialPort::connect()
{
	bool success = false;
	if (this->connected)
		this->disconnect();

	this->connected = false;

	// Create Handler
	this->handler = CreateFileA(static_cast<LPCSTR>(COMport),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// Initialize Handler
	if (this->handler == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			std::cout << "Handle could not be attatched; the specified COM port is unavailable." << std::endl;
		}
		else
		{	
			std::cout <<"Connection failed." << std::endl;
			//std::cout << GetLastErrorAsString() << std::endl;
		}
	}
	else
	{
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters))
		{
			std::cout <<"Connection failed; failed to retrieve current serial parameters.";
		}
		else
		{
			dcbSerialParameters.BaudRate = CBR_115200;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				std::cout <<"Connection failed; Could not set Serial port parameters.";
			}
			else
			{
				this->connected = true;

				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(500);
				
				success = true;
			}
		}
	}
	return success;
}

void SerialPort::disconnect()
{
	if (this->connected)
	{
		std::cout <<"Serial: Closing connection..." << std::endl;

		this->connected = false;
		this->listening = false;

		CloseHandle(this->handler);
	}
}

bool SerialPort::isConnected()
{
	return this->connected;
}

void SerialPort::writeByte(uint8_t byte)
{
	
	DWORD transmittedBytes;
	char bytes[] = { byte };

	if (!this->connected)
	{
		//std::cout <<"Serial: No active connection.";
		return;
	}

	if (!WriteFile(this->handler, bytes, 1, &transmittedBytes, NULL))
	{
		std::cout <<"Data could not be written.";
		ClearCommError(this->handler, &this->errors, &this->status);
	}

}

char SerialPort::readByte()
{

	// Check connection
	if (!this->connected)
	{
		//std::cout <<"Serial: No active connection.";
		return 0;
	}

	// Check queue
	ClearCommError(this->handler, &this->errors, &this->status);

	//if (this->status.cbInQue == 0) {
		//return 0;
	//}

	// Read buffer
	DWORD bytesread;
	uint8_t buffer[1];

	if (ReadFile(this->handler, buffer, 1, &bytesread, NULL))
	{
		return buffer[0];
	}
	else
	{
		std::cout <<"Error occured reading data.";
		return 0;
	}
	
}

void SerialPort::readBuffer(uint8_t& buffer, int length)
{
	// Check connection
	if (!isConnected())
	{
		return;
	}

	// Check queue
	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue == 0) {
		std::cout <<"Serial: Empty data buffer queue.";
		return;
	}

	// Read buffer
	DWORD bytesread;

	if (!ReadFile(this->handler, &buffer, length, &bytesread, NULL))
	{
		std::cout <<"Serial: Error occured reading data.";
		return;
	}
}

void SerialPort::flush()
{
	PurgeComm(handler, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	std::cout << "Serial: Buffer was flushed" << std::endl;
}

void SerialPort::readAllBytes()
{	
	int buffer;

	while (buffer = readByte())
	{
		if (buffer)
			std::cout << buffer << std::endl;// printf("%c", buffer);
	}

}

void SerialPort::readContinuousData()
{
	
	std::cout <<"Serial: Starting continuous buffer reading; press ESC to stop.\n";
	//Sleep(2000);

	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
			break;

		readAllBytes();
	}

}

// ###################################################################################################
// Poller & Listener

bool SerialPort::poll(int length)
{
	
	ClearCommError(this->handler, &this->errors, &this->status);
	
	if (this->status.cbInQue >= length)
		return true;
	else
		return false;
}


void SerialPort::listen(int length, int refresh)
{
	
	std::cout <<"Serial: Initializing listener, press ESC to stop.";
	
	// Pause listener if running
	if (this->listening)
		this->listening = false;

	// Clear buffer
	this->flush();

	// Set settings
	this->refreshRate = refresh;
	this->dataLength = length;

	// Start listening
	this->listening = true;

	// Initialize Listener thread if needed
	if (this->listenerThread == nullptr)
		this->listenerThread = new std::thread(&SerialPort::listener, this);
	
}

void SerialPort::listener()
{
	using namespace std::literals::chrono_literals;
	auto threadID = std::this_thread::get_id();

	std::cout << "Listener thread created.\n";

	while (this->connected)
	{

		if (!this->listening)
			continue;

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			this->listening = false;
			std::cout <<"Listener has been paused.";
			continue;
		}

		if (this->poll(dataLength))
		{

			uint8_t buffer[BUFFER_MAX_LENGTH];

			// Read buffer
			readBuffer(*buffer, dataLength);
			buffer[dataLength] = '\0';

			// Handle recieved data
			if (this->dataLength == 1)
			{
				printf("BYTE: 0x%X\n", buffer[0]);
				//std::cout << std::endl << ">> ";
			}
			else
			{
				std::cout << "More than 1 byte";
				return;
			}

		}

		std::this_thread::sleep_for(50ms);
	}
}