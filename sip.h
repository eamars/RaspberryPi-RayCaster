#ifndef SIP_H_
#define SIP_H_

#include <stdint.h>
#include <stdbool.h>

#define SIP_CMD_VECTOR_TABLE_SZ 256
#define SIP_MAX_RESP_LEN 256
#define SIP_MAX_PAYLOAD_LEN 256

// define callback prototype
typedef int (*callback_func)(uint8_t *payload, uint8_t payload_length); 

typedef enum
{
    NONE = 0,
    SFLAG,
    COMMAND_H,
    COMMAND_L,
    LENGTH_H,
    LENGTH_L,
    PAYLOAD_H,
    PAYLOAD_L,
    CHECKSUM_H,
    CHECKSUM_L,
    EFLAG
} SIPState_t;

typedef enum
{
    NO_ERROR = 0,
    INVALID_SFLAG_ERROR,
    INVALID_EFLAG_ERROR,
    INVALID_CMD_ERROR,
    INVALID_CS_ERROR,
    INVALID_EXEC_ERROR
} SIPError_t;

typedef struct
{
	// public properties
	SIPState_t State;
	SIPError_t Error;
	callback_func CommandVectorTable[SIP_CMD_VECTOR_TABLE_SZ];

	// temp variables
	uint8_t command;
	uint8_t length;
	uint8_t payload_byte;
	uint8_t payload[SIP_MAX_PAYLOAD_LEN];
	uint8_t payload_counter;
	uint8_t checksum;
} SIP_t;

void SIPRegisterCommand(SIP_t *sip, uint8_t command, callback_func f);
void SIPDeregisterCommand(SIP_t *sip, uint8_t command);

void SIPFeedInput(SIP_t *sip, char ch);

uint8_t hexchar_to_uint8(uint8_t ch);

#endif