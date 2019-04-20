#include "pch.h"
#include "main.h"

#include <curses.h>
#include <stdio.h>

// Evil global variables
DEVICE_DATA                 deviceData{};
HRESULT                     hr;
USB_DEVICE_DESCRIPTOR       deviceDesc;
BOOL                        bResult;
BOOL                        noDevice;
ULONG                       lengthReceived;
RAZER_ATROX_BUTTON_STATE    buttonState{};
RAZER_ATROX_DATA_PACKET     dataPacket{};

BOOL InitRazerAtrox(WINUSB_INTERFACE_HANDLE hDeviceHandle)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;
	ULONG cbSize = 5;
	ULONG cbSent = 0;

	bResult = WinUsb_WritePipe(hDeviceHandle, 0x01, RAZER_ATROX_INIT, cbSize, &cbSent, 0);
	if (bResult)
	{
		wprintf(L"Wrote to pipe %d: %s \nActual data transferred: %d.\n", 0x01, RAZER_ATROX_INIT, cbSent);		
	}

	// TODO: Handshake?

	return bResult;
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    endwin(); 
    return true;
}

BOOL initPdCurses() 
{
    initscr();
    return true;
}

BOOL initDevice()
{
    while (true) {
        erase();
        for (auto i = 0; i < 10; i++) {
            mvprintw(0, 0, "Searching for device...");
            for (auto j = 0; j < i; j++) mvprintw(0, j + 23, ".");
            hr = OpenDevice(&deviceData, &noDevice);
            if (FAILED(hr)) {
                if (noDevice) mvprintw(1, 0, "Device not connected or driver not installed");
                else mvprintw(1, 0, "Failed looking for device, HRESULT 0x%x", hr);
            }
            else {
                // It seems that querying for the desciptor is basically required to init correctly
                bResult = WinUsb_GetDescriptor(deviceData.WinusbHandle,
                    USB_DEVICE_DESCRIPTOR_TYPE,
                    0,
                    0,
                    (PBYTE)&deviceDesc,
                    sizeof(deviceDesc),
                    &lengthReceived);

                if (FALSE == bResult || lengthReceived != sizeof(deviceDesc)) {
                    CloseDevice(&deviceData);
                    return 0;
                }
                InitRazerAtrox(deviceData.WinusbHandle);
                return true;
            }
            refresh();
            Sleep(1000);
        }        
        mvprintw(2, 0, "Press x to exit, any other key to try again...");
        refresh();
        if (mvgetch(3, 0) == 'x') return false;
    }
}

/*
Blocking data read from end-point 0x81
*/
BOOL ReadInputFromAtrox(WINUSB_INTERFACE_HANDLE hDeviceHandle, RAZER_ATROX_DATA_PACKET &dataPacket)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE) return FALSE;        
    BOOL bResult = WinUsb_ReadPipe(hDeviceHandle, 0x81, dataPacket.data, 30, &dataPacket.transferred, NULL);
    return bResult;
}

RAZER_ATROX_PACKET_TYPES processDatapacket(RAZER_ATROX_DATA_PACKET &dataPacket, RAZER_ATROX_BUTTON_STATE &buttonState) 
{
    if (dataPacket.transferred == 0) return UNKNOWN;
    switch (dataPacket.data[0]) {
    case 0x01: // Dummy packet?
        return DUMMY;
    case 0x03: // Heartbeat packet?
        return HEARTBEAT;
    case 0x07: // Guide button
        buttonState.buttonGuide = dataPacket.data[4] & 0x01;
        return GUIDE;
    case 0x20: // Inputs
        buttonState.buttonA =       dataPacket.data[22] & 0x10;
        buttonState.buttonB =       dataPacket.data[22] & 0x20;
        buttonState.buttonX =       dataPacket.data[22] & 0x01;
        buttonState.buttonY =       dataPacket.data[22] & 0x02;
        buttonState.rightButton =   dataPacket.data[22] & 0x04;
        buttonState.leftButton =    dataPacket.data[22] & 0x08;
        buttonState.rightTrigger =  dataPacket.data[22] & 0x40;
        buttonState.leftTrigger =   dataPacket.data[22] & 0x80;
        buttonState.buttonMenu =    dataPacket.data[04] & 0x04;
        buttonState.buttonView =    dataPacket.data[04] & 0x08;
        buttonState.stickUp =       dataPacket.data[05] & 0x01;
        buttonState.stickDown =     dataPacket.data[05] & 0x02;
        buttonState.stickLeft =     dataPacket.data[05] & 0x04;
        buttonState.stickRight =    dataPacket.data[05] & 0x08;
        return BUTTON_INPUT;
    }
    return UNKNOWN;
}

void mainLoop()
{   
    int failedReadCount = 0;    
    BOOL previousGuideButtonState = false;    
    ULONGLONG guideDown;
    ULONGLONG guideUp;
    guideDown = guideUp = GetTickCount64();
    RAZER_ATROX_PACKET_TYPES packetType = UNKNOWN;
    while (true) {
        erase();
        // Read input
        if (ReadInputFromAtrox(deviceData.WinusbHandle, dataPacket)) packetType = processDatapacket(dataPacket, buttonState);
        else failedReadCount += 1;
        // Process status
        mvaddstr(0, 0, "Status: ");
        switch(packetType) {
        case DUMMY:
            mvaddstr(0, 8, "DUMMY RECEIVED");
            break;
        case HEARTBEAT: 
            mvaddstr(0, 8, "HEARTBEAT RECEIVED");
            break;
        case GUIDE:
        case BUTTON_INPUT:
            mvaddstr(0, 8, "BUTTON STATE CHANGE");
            break;
        }
        // Process pressed buttons
        mvaddstr(1,  0, "Pressed:");
        mvaddstr(1,  9, buttonState.buttonA         ? "A" : "");
        mvaddstr(1, 11, buttonState.buttonB         ? "B" : "");
        mvaddstr(1, 13, buttonState.buttonX         ? "X" : "");
        mvaddstr(1, 15, buttonState.buttonY         ? "Y" : "");
        mvaddstr(1, 17, buttonState.rightButton     ? "RB" : "");
        mvaddstr(1, 20, buttonState.leftButton      ? "LB" : "");
        mvaddstr(1, 23, buttonState.rightTrigger    ? "RT" : "");
        mvaddstr(1, 26, buttonState.leftTrigger     ? "LT" : "");
        mvaddstr(1, 29, buttonState.buttonMenu      ? "Menu" : "");
        mvaddstr(1, 34, buttonState.buttonView      ? "View" : "");
        mvaddstr(1, 39, buttonState.buttonGuide     ? "Guide" : "");
        // Process released buttons
        mvaddstr(2,  0, "Released:");
        mvaddstr(2,  9, !buttonState.buttonA        ? "A" : "");
        mvaddstr(2, 11, !buttonState.buttonB        ? "B" : "");
        mvaddstr(2, 13, !buttonState.buttonX        ? "X" : "");
        mvaddstr(2, 15, !buttonState.buttonY        ? "Y" : "");
        mvaddstr(2, 17, !buttonState.rightButton    ? "RB" : "");
        mvaddstr(2, 20, !buttonState.leftButton     ? "LB" : "");
        mvaddstr(2, 23, !buttonState.rightTrigger   ? "RT" : "");
        mvaddstr(2, 26, !buttonState.leftTrigger    ? "LT" : "");
        mvaddstr(2, 29, !buttonState.buttonMenu     ? "Menu" : "");
        mvaddstr(2, 34, !buttonState.buttonView     ? "View" : "");
        mvaddstr(2, 39, !buttonState.buttonGuide    ? "Guide" : "");
        // Process Stick
        mvaddstr(5,  0, "Stick:");
        mvaddstr(4, 16, buttonState.stickUp         ? "UP" : "");
        mvaddstr(5,  9, buttonState.stickLeft       ? "LEFT" : "");
        mvaddstr(6, 15, buttonState.stickDown       ? "DOWN" : "");
        mvaddstr(5, 21, buttonState.stickRight      ? "RIGHT" : "");
        // Process guide button hold
        mvaddstr(8, 0, "Hold Guide button for 5 or more seconds then release to quit");        
        if (buttonState.buttonGuide != previousGuideButtonState) { // Guide button state has changed            
            if (previousGuideButtonState) { // Button was being pressed, has been released
                guideUp = GetTickCount64(); 
                if ((guideUp - guideDown) >= 5000) return;
            }
            else guideDown = GetTickCount64(); // Button was released, now being pressed
            previousGuideButtonState = buttonState.buttonGuide;
        } 
        mvprintw(9, 0, "Failed read count: %d", failedReadCount);
        if (failedReadCount > 1000) {
            if (!initDevice()) ExitProcess(4);
            failedReadCount = 0;            
        }
        mvaddstr(10, 0, "Data: ");
        for (auto i = 0; i < dataPacket.transferred; i++) mvprintw(10, 6 + (i * 3), "%x", dataPacket.data[i]);        
        refresh();
    }
}

/*
Entrypoint
*/
LONG __cdecl _tmain(LONG Argc, LPTSTR * Argv)
{  	
    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        wprintf(L"Warning! Console Control Handler could not be installed!");
        ExitProcess(1);
    }
    // Init pdcurses
    if (!initPdCurses()) {
        wprintf(L"Failed to init pdcurses");
        ExitProcess(2);
    }
    // Attempt to connect to the Atrox
    if (!initDevice()) {
        ExitProcess(3);
    }

    mainLoop();

    CloseDevice(&deviceData);
    return 0;
}
