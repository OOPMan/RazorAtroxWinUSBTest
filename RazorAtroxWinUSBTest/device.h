//
// Define below GUIDs
//
#include <initguid.h>

//
// Device Interface GUID.
// Used by all WinUsb devices that this application talks to.
// Must match "DeviceInterfaceGUIDs" registry value specified in the INF file.
// 98d6aa6f-518a-42f3-a6b5-e752c4996116
//
// 9B65FE78-9976-4E8A-A28D-6C E3 4B E8 32 9D
DEFINE_GUID(GUID_DEVINTERFACE_USBApplication2,	
	0x9b65fe78,0x9976,0x4e8a,0xa2,0x8d,0x6c,0xe3,0x4b,0xe8,0x32,0x9d);
    //0x98d6aa6f,0x518a,0x42f3,0xa6,0xb5,0xe7,0x52,0xc4,0x99,0x61,0x16
	//);

typedef struct _DEVICE_DATA {

    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
    );

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
    );
