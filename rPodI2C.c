#include "rPodI2C.h"
#include <stdint.h>
#include "string.h"
#include <stdio.h>
#include <netinet/in.h>
#include <endian.h>
#include <inttypes.h>

uint8_t buffer2[I2C_BUFFER_SIZE];
uint16_t bufferBegin;
uint16_t bufferLength;

void initReceiver()
{
	bufferLength = 0;
	bufferBegin = 0;
}

void receiveBytes(uint8_t* data, uint16_t length)
{
	uint16_t pos = 0;
	int i = 0;

	if (length > I2C_BUFFER_SIZE)
		return; //somehthing's not right, drop this data and hope we catch up
	else {}//All is well, continue on

	//Got some bad data at some point
	if (bufferLength + length > I2C_BUFFER_SIZE)
	{
		bufferLength = 0;
		bufferBegin = 0;
	}

	for (i = 0; i < length; i++)
		buffer2[(i + bufferBegin + bufferLength)%I2C_BUFFER_SIZE] = data[i];
	bufferLength += length;

	processBuffer();
}

void processBuffer()
{
	uint8_t frameBuffer[I2C_BUFFER_SIZE];

	int i = 0;
	int bufferBeginJump = 0;

	//Search for a full frame
	for (i = 0; i < (bufferLength-1); i++)
	{
		//Is this a start code?
		if (buffer2[(bufferBegin + i) % I2C_BUFFER_SIZE] == I2C_CONTROL_CHAR && buffer2[((bufferBegin + i + 1) % I2C_BUFFER_SIZE)] == I2C_FRAME_START)
		{
			//See if we should have the whole header yet
			if (i + 5 < bufferLength)
			{
				//Grab the length of the frame from the header
				uint16_t frameLength = buffer2[(bufferBegin + i + 2) % I2C_BUFFER_SIZE];
				int headerLength = 4;
				if (frameLength == I2C_CONTROL_CHAR){
					frameLength = frameLength * 256 + buffer2[(bufferBegin + i + 4) % I2C_BUFFER_SIZE];
					headerLength++;
					if (buffer2[(bufferBegin + i + 4) % I2C_BUFFER_SIZE] == I2C_CONTROL_CHAR)
						headerLength++;
				}
				else
					frameLength = frameLength*256 + buffer2[(bufferBegin + i + 3) % I2C_BUFFER_SIZE];

				//See if we have the end of the frame in the buffer yet
				if (i + headerLength + frameLength <= bufferLength)
				{
					//Copy the frame into a flat buffer
					//This step isn't 100% necessary but it does make processing the frame a bit easier to follow
					int x;
					for (x = 0; x < (frameLength + headerLength); x++)
					{
						frameBuffer[x] = buffer2[(x+i+bufferBegin) % I2C_BUFFER_SIZE];
					}

					//Process the frame!
					processFrame(frameBuffer, frameLength + 4);

					i += frameLength + headerLength;
					bufferBeginJump += headerLength + frameLength;
				}
				else{
					//We have a start code, but not enough data for a full frame yet
					i = bufferLength;
				}
			}
			else{
				//Don't have the full header yet, nothing to do for now
				i = bufferLength - 1;
			}
		}
		else{
			//Not sure what we got sent, but it's not a start code so advance the buffer.
			bufferBeginJump++;
		}
	}
	bufferBegin += bufferBeginJump;
	bufferBegin = bufferBegin % I2C_BUFFER_SIZE;
	bufferLength -= bufferBeginJump;

}

void processFrame(uint8_t *frameBuffer, uint16_t length)
{

	int x;

	//Check the start and end headers
	if (frameBuffer[0] != I2C_CONTROL_CHAR || frameBuffer[1] != I2C_FRAME_START || frameBuffer[length - 4] != I2C_CONTROL_CHAR || frameBuffer[length - 3] != I2C_FRAME_END)
		return;

	int i = 0;
	uint8_t checksum = 0;
	for (i = 0; i < length-4; i++)
		checksum ^= frameBuffer[i];

	if (frameBuffer[length - 2] != checksum){
		return;
	}
	else {} //All good, continue on

	//Shorten any escaped data now that we've isolated a single frame
	for (i = 2; i < length-1; i++)
	{
		if (frameBuffer[i] == I2C_CONTROL_CHAR && frameBuffer[i + 1] == I2C_CONTROL_CHAR)
		{
			int x;
			for (x = i + 1; x < length - 1; x++)
			{
				frameBuffer[x] = frameBuffer[x + 1];
			}
			frameBuffer[length - 1] = 0x00;
		}
		else{}//no escaped data here, continue on.
	}

	uint16_t position = 4;
	uint64_t rawData;
	while (position < length)
	{
		if (frameBuffer[position] == 0xD5)
		{
			position++;
			switch (frameBuffer[position])
			{
			case I2C_PARAMETER_START:
				{
					position++;

					printf("Parameter %d, ", frameBuffer[position + 1]);

					int dataType = frameBuffer[position] & 0x0F;
					uint8_t dataSize = (frameBuffer[position] & 0xF0) / 16;

					if (dataType == 1 || dataType == 2 || dataType == 3)
					{
						rawData = 0;
						//safety check for memcopy
						if (dataSize <= 8){
							memcpy(&rawData, &frameBuffer[position + 2], dataSize);
						}
						else{ break; } //Somethings not right

						float floatingType = 0;
						double doubleType = 0;	
						uint64_t rawFloatingData = be64toh((uint64_t)rawData);
						memcpy(&doubleType,&rawFloatingData,8);
						rawFloatingData = rawFloatingData >> 32;
						memcpy(&floatingType,&rawFloatingData,4);

						switch (frameBuffer[position])
						{
						case 0x11: printf("Signed 8 bit integer: %d\n", (int8_t)rawData); break;
						case 0x12: printf("Unsigned 8 bit integer: %u\n", (uint8_t)rawData); break;
						case 0x21: printf("Signed 16 bit integer: %d\n", (int16_t)ntohs((uint16_t)rawData)); break;
						case 0x22: printf("Unsigned 16 bit integer: %d\n", ntohs((uint16_t)rawData)); break;
						case 0x41: printf("Signed 32 bit integer: %d\n", ntohl((int32_t)rawData)); break;
						case 0x42: printf("Unigned 32 bit integer: %d\n", ntohl((uint32_t)rawData)); break;
						case 0x81: printf("Signed 64 bit integer: %"PRId64"\n", (int64_t)be64toh((uint64_t)rawData)); break;
						case 0x82: printf("Unsigned 64 bit integer: %"PRId64"\n", be64toh((uint64_t)rawData)); break;
						case 0x43: printf("Signed 32 bit float: %f\n", floatingType); break;
						case 0x83: printf("Signed 64 bit double: %f\n", doubleType); break;
						default:printf("Uh-oh\n\n"); break;
						}
					}
					else if (dataType == 0){

					}
					else{}

					position += dataSize+2;
				}
					break;
				case I2C_FRAME_END:return; break;
			}
		}
		else{
			//Should report an error
			position++;
		}
	}
	
}