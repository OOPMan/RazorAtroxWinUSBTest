#include "pch.h"

#include <stdio.h>

struct PIPE_ID
{
	UCHAR  PipeInId;
	UCHAR  PipeOutId;
};

BOOL GetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pDeviceSpeed)
{
	if (!pDeviceSpeed || hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;

	ULONG length = sizeof(UCHAR);

	bResult = WinUsb_QueryDeviceInformation(hDeviceHandle, DEVICE_SPEED, &length, pDeviceSpeed);
	if (!bResult)
	{
		printf("Error getting device speed: %d.\n", GetLastError());
		goto done;
	}
	// See https://docs.microsoft.com/en-gb/windows/desktop/api/winusb/nf-winusb-winusb_querydeviceinformation
	if (*pDeviceSpeed == LowSpeed)
	{
		printf("Device speed: %d (Full speed or lower).\n", *pDeviceSpeed);
		goto done;
	}
	if (*pDeviceSpeed == HighSpeed)
	{
		printf("Device speed: %d (High speed).\n", *pDeviceSpeed);
		goto done;
	}

done:
	return bResult;
}

BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;

	USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
	ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

	WINUSB_PIPE_INFORMATION  Pipe;
	ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


	bResult = WinUsb_QueryInterfaceSettings(hDeviceHandle, 0, &InterfaceDescriptor);

	if (bResult)
	{
		for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
		{
			bResult = WinUsb_QueryPipe(hDeviceHandle, 0, index, &Pipe);

			if (bResult)
			{
				if (Pipe.PipeType == UsbdPipeTypeControl)
				{
					printf("Endpoint index: %d Pipe type: Control Pipe ID: %d.\n", index, /*Pipe.PipeType,*/ Pipe.PipeId);
				}
				if (Pipe.PipeType == UsbdPipeTypeIsochronous)
				{
					printf("Endpoint index: %d Pipe type: Isochronous Pipe ID: %d.\n", index, /*Pipe.PipeType,*/ Pipe.PipeId);
				}
				if (Pipe.PipeType == UsbdPipeTypeBulk)
				{
					if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
					{
						printf("Endpoint index: %d Pipe type: Bulk Pipe ID: %c.\n", index, /*Pipe.PipeType,*/ Pipe.PipeId);
						pipeid->PipeInId = Pipe.PipeId;
					}
					if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
					{
						printf("Endpoint index: %d Pipe type: Bulk Pipe ID: %c.\n", index, /*Pipe.PipeType,*/ Pipe.PipeId);
						pipeid->PipeOutId = Pipe.PipeId;
					}

				}
				if (Pipe.PipeType == UsbdPipeTypeInterrupt)
				{
					printf("Endpoint index: %d Pipe type: Interrupt Pipe ID: %d.\nPolling Interval: %d\n", index, /*Pipe.PipeType,*/ Pipe.PipeId, Pipe.Interval);
				}
			}
			else
			{
				continue;
			}
		}
	}

done:
	return bResult;
}

UCHAR RAZER_ATROX_INIT[] = {
	0x05, 0x20, 0x08, 0x01, 0x05
};

BOOL WriteToAtroxEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;

	UCHAR szBuffer[] = { 0x05, 0x20, 0x08, 0x01, 0x05 };
	ULONG cbSize = 5;
	ULONG cbSent = 0;

	bResult = WinUsb_WritePipe(hDeviceHandle, 0x01, RAZER_ATROX_INIT, cbSize, &cbSent, 0);
	if (!bResult)
	{
		goto done;
	}

	printf("Wrote to pipe %d: %s \nActual data transferred: %d.\n", 0x01, szBuffer, cbSent);	

done:
	return bResult;

}

BOOL ReadInputFromAtrox(WINUSB_INTERFACE_HANDLE hDeviceHandle)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;
	UCHAR data[30];
	ULONG transferred;

	bResult = WinUsb_ReadPipe(hDeviceHandle, 0x81, data, 30, &transferred, NULL);

	if (bResult) {
		wprintf(L"Button press report:\n\n");
		if (transferred == 6 && data[0] & 0x07) {						
			if (data[2] & 0x02) wprintf(L"Guide button released\n");
			else if (data[2] & 0x01) wprintf(L"Guide button pressed\n");
		}
		else if (transferred == 30 && data[0] & 0x20) {
			if (data[22] & 0x80) wprintf(L"LT pressed\n");
			else wprintf(L"LT released\n");
			if (data[22] & 0x40) wprintf(L"RT pressed\n");
			else wprintf(L"RT released\n");
		}
		else if (transferred == 8 && data[0] & 0x03) {
			wprintf(L"Heartbeat received");
		}
		else if (transferred == 2) {
			wprintf(L"%d %d", data[0], data[1]);
		}
		wprintf(L"\nTransferred length: %d\n\n", transferred);

	}

	return bResult;
}

LONG __cdecl
_tmain(
    LONG     Argc,
    LPTSTR * Argv
    )
/*++

Routine description:

    Sample program that communicates with a USB device using WinUSB

--*/
{
	PIPE_ID				  pipeId;
    DEVICE_DATA           deviceData;
    HRESULT               hr;
    USB_DEVICE_DESCRIPTOR deviceDesc;
    BOOL                  bResult;
    BOOL                  noDevice;
    ULONG                 lengthReceived;
	UCHAR				  deviceSpeed;

    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);

    //
    // Find a device connected to the system that has WinUSB installed using our
    // INF
    //
    hr = OpenDevice(&deviceData, &noDevice);

    if (FAILED(hr)) {

        if (noDevice) {

            wprintf(L"Device not connected or driver not installed\n");

        } else {

            wprintf(L"Failed looking for device, HRESULT 0x%x\n", hr);
        }

        return 0;
    }
	//
	// Query device speed
	// 
	GetUSBDeviceSpeed(deviceData.WinusbHandle, &deviceSpeed);
	//
	// Query device end-points
	//
	QueryDeviceEndpoints(deviceData.WinusbHandle, &pipeId);

    //
    // Get device descriptor
    //
    bResult = WinUsb_GetDescriptor(deviceData.WinusbHandle,
                                   USB_DEVICE_DESCRIPTOR_TYPE,
                                   0,
                                   0,
                                   (PBYTE) &deviceDesc,
                                   sizeof(deviceDesc),
                                   &lengthReceived);

    if (FALSE == bResult || lengthReceived != sizeof(deviceDesc)) {

        wprintf(L"Error among LastError %d or lengthReceived %d\n",
                FALSE == bResult ? GetLastError() : 0,
                lengthReceived);
        CloseDevice(&deviceData);
        return 0;
    }

    //
    // Print a few parts of the device descriptor
    //
    wprintf(L"Device found: VID_%04X&PID_%04X; bcdUsb %04X\n",
            deviceDesc.idVendor,
            deviceDesc.idProduct,
            deviceDesc.bcdUSB);

	//
	// Attempt to send init packet
	//
	if (!WriteToAtroxEndpoint(deviceData.WinusbHandle)) {
		wprintf(L"Init send failed!\n");
		return 1;
	};
	SYSTEMTIME myTime;
	int ticked = 0;
	while (true) {
		while (ticked < 1000) {
			ReadInputFromAtrox(deviceData.WinusbHandle);
			Sleep(4);
			ticked += 4;
		}
		if (!WriteToAtroxEndpoint(deviceData.WinusbHandle)) {
			wprintf(L"Init send failed!\n");
			return 1;
		};				
	}
		
	/*for (auto i = 0; i < 60; i++) {
		wprintf(L"\nSending init packet %d\n", i);
		if (!WriteToAtroxEndpoint(deviceData.WinusbHandle)) {
			wprintf(L"Init send failed!\n");
			return 1;
		};
		Sleep(1000);
	}*/

    CloseDevice(&deviceData);
    return 0;
}
