#pragma once

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
    BOOL buttonX;
    BOOL buttonY;
    BOOL buttonA;
    BOOL buttonB;
    BOOL rightButton;
    BOOL leftButton;
    BOOL rightTrigger;
    BOOL leftTrigger;
    BOOL buttonMenu;
    BOOL buttonView;
    BOOL buttonGuide;
    BOOL stickUp;
    BOOL stickLeft;
    BOOL stickDown;
    BOOL stickRight;
};

enum RAZER_ATROX_PACKET_TYPES
{
    UNKNOWN,
    DUMMY,
    HEARTBEAT,
    GUIDE,
    BUTTON_INPUT
};
