#include <ntddk.h>
#include <wdm.h>
#include "test.h"


void unload(PDRIVER_OBJECT driverObject) {
	DbgPrint("unloading\n");
	UNREFERENCED_PARAMETER(driverObject);
	UNICODE_STRING sym_name = RTL_CONSTANT_STRING(L"\\??\\TEST_DEV");
	IoDeleteSymbolicLink(&sym_name);
	IoDeleteDevice(driverObject->DeviceObject);

}

NTSTATUS CreateClose(PDEVICE_OBJECT driverObject, PIRP irp) {
	DbgPrint("created device\n");
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS Write(PDEVICE_OBJECT driverObject,PIRP irp) {
	
	KIRQL kirql = 3, old_kirql;
	
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	struct Message* data = (struct Message*)irp->UserBuffer;
	DbgPrint(data->text);
	if (strcmp(data->text, "irq") == 0) {
		KeRaiseIrql(kirql,&old_kirql);

		DbgPrint("IRQL now is set to %d\n",KeGetCurrentIrql());
		KeLowerIrql(old_kirql);
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath) {

	UNREFERENCED_PARAMETER(driverObject);
	UNREFERENCED_PARAMETER(registryPath);
	DbgPrint("loaded\n");
	
	driverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	driverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	driverObject->MajorFunction[IRP_MJ_WRITE] = Write;
	driverObject->DriverUnload = unload;
	;
	PDEVICE_OBJECT dev;
	UNICODE_STRING dev_name = RTL_CONSTANT_STRING(L"\\Device\\TEST_DEVICE");
	UNICODE_STRING sym_name = RTL_CONSTANT_STRING(L"\\??\\TEST_DEV");
	NTSTATUS stat = IoCreateDevice(driverObject, 0, &dev_name, FILE_DEVICE_UNKNOWN, 0, FALSE, &dev);
	if (stat != STATUS_SUCCESS) {
		DbgPrint("failed creating device\n");

	}
	else {
		DbgPrint("created device\n");
		stat = IoCreateSymbolicLink(&sym_name, &dev_name);
		if (stat != STATUS_SUCCESS) {
			DbgPrint("failed creating sym link for device\n");
			IoDeleteDevice(dev);
		}
		else {
			DbgPrint("created sym link for device\n");
		}
	}
	return STATUS_SUCCESS;
}