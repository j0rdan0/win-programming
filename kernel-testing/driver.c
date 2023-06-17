#include <ntddk.h>
#include <wdf.h>
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KernelTestingDeviceAdd;

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    // NTSTATUS variable to record success or failure
    NTSTATUS status = STATUS_SUCCESS;

    // Allocate the driver configuration object
    WDF_DRIVER_CONFIG config;

   
    DbgPrint("[*] driver entry point, registering callback function\n");

    // Initialize the driver configuration object to register the
    // entry point for the EvtDeviceAdd callback,KernelTestingAdd
    WDF_DRIVER_CONFIG_INIT(&config,
        KernelTestingDeviceAdd
    );

    // Finally, create the driver object
    status = WdfDriverCreate(DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );
    return status;
}

NTSTATUS
KernelTestingDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    // We're not using the driver object,
    // so we need to mark it as unreferenced
    UNREFERENCED_PARAMETER(Driver);

    NTSTATUS status;

    // Allocate the device object
    WDFDEVICE hDevice;

    DbgPrint("[*] callback function\n");

    HANDLE proc = NULL;
    CLIENT_ID  cid;
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
    ULONG pid = 96; // Registry proc
    cid.UniqueProcess = (HANDLE)pid;
    cid.UniqueThread = NULL;

    NTSTATUS stat = ZwOpenProcess(&proc, PROCESS_ALL_ACCESS, &obj_attr, &cid);

    switch (stat) {

    case STATUS_INVALID_CID:
        DbgPrint("Invaliv cid\n");
        break;
    case STATUS_INVALID_PARAMETER:
        DbgPrint("Invalid params\n");
        break;
    case STATUS_ACCESS_DENIED:
        DbgPrint("Access denied\n");
        break;
    case STATUS_ACCESS_VIOLATION:
        DbgPrint("Status access violation\n");
        break;
    case STATUS_SUCCESS:
        DbgPrint("[*] got proc handle\n");
        break;
    case STATUS_INVALID_PARAMETER_MIX:
        DbgPrint("Invalid param mix\n");
        break;
    }
    if (proc != NULL) {
        ZwClose(proc);
    }

    // Create the device object
    status = WdfDeviceCreate(&DeviceInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &hDevice
    );
    return status;
}