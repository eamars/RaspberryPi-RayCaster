#include <stdio.h>
#include "sip.h"

#define DEBUG_SIP

void SIPRegisterCommand(SIP_t *sip, uint8_t command, callback_func f)
{
	sip->CommandVectorTable[command] = f;
}

void SIPDeregisterCommand(SIP_t *sip, uint8_t command)
{
	sip->CommandVectorTable[command] = 0;
}

void SIPFeedInput(SIP_t *sip, char ch)
{
	// reset the state to keep sync
	if (ch == '<')
	{
		sip->State = SFLAG;

		// reset variables
		sip->payload_counter = 0;
		sip->Error = NO_ERROR;
	}

	switch(sip->State)
	{
		case SFLAG:
		{
			sip->State = COMMAND_H;

#ifdef DEBUG_SIP
			printf("SIP::SFLAG: 0x%02x\r\n", '<');
#endif
			break;
		}
		case COMMAND_H:
		{
			sip->command = hexchar_to_uint8(ch) << 4;
			sip->State = COMMAND_L;

#ifdef DEBUG_SIP
			printf("SIP::COMMAND_H: 0x%02x\r\n", sip->command);
#endif
			break;
		}
		case COMMAND_L:
		{
			sip->command |= (hexchar_to_uint8(ch) & 0x0f);
			sip->State = LENGTH_H;

#ifdef DEBUG_SIP
            printf("SIP::COMMAND_L: 0x%02x\r\n", sip->command);
#endif
            break;
		}
		case LENGTH_H:
		{
			sip->length = hexchar_to_uint8(ch) << 4;
			sip->State = LENGTH_L;

#ifdef DEBUG_SIP
            printf("SIP::LENGTH_H: 0x%02x\r\n", sip->length);
#endif
            break;
		}
		case LENGTH_L:
		{
			sip->length |= (hexchar_to_uint8(ch) & 0x0f);

			if (sip->length != 0) // if the length is not zero then proceed to payload state
			{
				sip->State = PAYLOAD_H;
			}
			else // otherwise proceed to checksum state
			{
				sip->State = CHECKSUM_H;
			}

#ifdef DEBUG_SIP
            printf("SIP::LENGTH_L: 0x%02x\r\n", sip->length);
#endif
            break;
		}
		case PAYLOAD_H:
		{
			sip->payload_byte = hexchar_to_uint8(ch) << 4;
			sip->State = PAYLOAD_L;

#ifdef DEBUG_SIP
            printf("SIP::PAYLOAD_H: 0x%02x\r\n", sip->payload_byte);
#endif
            break;
		}
		case PAYLOAD_L:
		{
			sip->payload_byte |= (hexchar_to_uint8(ch) & 0x0f);
			sip->payload[sip->payload_counter++] = sip->payload_byte;

			if (sip->payload_counter < sip->length) // append ch to payload until reaching the payload length
			{
				sip->State = PAYLOAD_H;
			}
			else
			{
				sip->State = CHECKSUM_H;
			}

#ifdef DEBUG_SIP
            printf("SIP::PAYLOAD_L: 0x%02x\r\n", sip->payload_byte);
#endif
            break;
		}
		case CHECKSUM_H:
		{
			sip->checksum = hexchar_to_uint8(ch) << 4;
			sip->State = CHECKSUM_L;

#ifdef DEBUG_SIP
            printf("SIP::CHECKSUM_H: 0x%02x\r\n", sip->checksum);
#endif
            break;
		}
		case CHECKSUM_L:
		{
			sip->checksum |= (hexchar_to_uint8(ch) & 0x0f);
			sip->State = EFLAG;

#ifdef DEBUG_SIP
            printf("SIP::CHECKSUM_L: 0x%02x\r\n", sip->checksum);
#endif
            break;
		}
		case EFLAG:
		{
			sip->State = NONE;

#ifdef DEBUG_SIP
            printf("SIP::EFLAG: 0x%02x\r\n", '>');
#endif
            // execute the corresponding function
			if (sip->CommandVectorTable[sip->command] != 0)
			{
				sip->CommandVectorTable[sip->command](
					sip->payload,
					sip->length
				);
			}
			
            break;
		}
		case NONE:
		{
#ifdef DEBUG_SIP
            printf("SIP::NONE\r\n");
#endif
            break;
		}
		default:
			break;
	}
}


uint8_t hexchar_to_uint8(uint8_t ch)
{
	uint8_t val = 0;
 
	if (ch >= '0' && ch <= '9')
	{
		val = ch - '0';
	}
	else if (ch >= 'A' && ch <= 'F')
	{
		val  = ch - 'A';
		val += 10;
	}
	else if (ch >= 'a' && ch <= 'f')
	{
		val = ch - 'a';
		val += 10;
	}
 
	return val;
}