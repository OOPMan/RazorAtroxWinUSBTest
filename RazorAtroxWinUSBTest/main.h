#pragma once

struct PIPE_ID
{
	UCHAR  PipeInId;
	UCHAR  PipeOutId;
}; 

UCHAR RAZER_ATROX_INIT[] = {
	0x05, 0x20, 0x08, 0x01, 0x05
};

struct RAZER_ATROX_DATA_PACKET
{
	UCHAR data[30];
	ULONG transferred;
};

struct RAZER_ATROX_BUTTON_STATE
{
    BOOL buttonX : 1;
    BOOL buttonY : 1;
    BOOL buttonA : 1;
    BOOL buttonB : 1;
    BOOL rightButton : 1;
    BOOL leftButton : 1;
    BOOL rightTrigger : 1;
    BOOL leftTrigger : 1;
    BOOL buttonMenu : 1;
    BOOL buttonView : 1;
    BOOL buttonGuide : 1;
    BOOL stickUp : 1;
    BOOL stickLeft : 1;
    BOOL stickDown : 1;
    BOOL stickRight : 1;
};

enum RAZER_ATROX_PACKET_TYPES
{
    UNKNOWN,
    DUMMY,
    HEARTBEAT,
    GUIDE,
    BUTTON_INPUT
};
