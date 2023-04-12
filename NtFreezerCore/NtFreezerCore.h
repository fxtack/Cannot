/*
    @File   NtFreezerCore.h
    @Note   NtFreezerCore module header file.

    @Mode   Kernel
    @Author Fxtack
*/

#define RTL_USE_AVL_TABLES 0

#ifndef _NTFZ_CORE_H_
#define _NTFZ_CORE_H_

#include <fltKernel.h>
#include <ntddk.h>
#include <dontuse.h>

#include "NtFreezer.h"

#define NTFZ_CORE_VERSION_MAJOR 0
#define NTFZ_CORE_VERSION_MINOR 1
#define NTFZ_CORE_VERSION_PATCH 0

#define MEM_NPAGED_POOL_TAG_CONFIG_ENTRY 'fzcg'

#define MAX_CONFIG_ENTRY_ALLOCATED 1024

// These routine is used by AVL tree (ConfigTable).
RTL_GENERIC_COMPARE_ROUTINE  configEntryCompareRoutine;  // Compare two config entry.
RTL_GENERIC_ALLOCATE_ROUTINE configEntryAllocateRoutine; // Allocate config entry.
RTL_GENERIC_FREE_ROUTINE     configEntryFreeRoutine;     // Free config entry.

/*************************************************************************
    Global variabls.
*************************************************************************/
typedef struct _NTFZ_CORE_GLOBALS {

    // Filter instances.
    PFLT_FILTER Filter;

    // The communication port.
    PFLT_PORT CorePort;
    PFLT_PORT AdminPort;

    // Maxinum of config entry can be allocated.
    ULONG ConfigEntryMaxAllocated;
    // Amount of allocated config entry.
    __volatile ULONG ConfigEntryAllocated;
    // Memory pool of config entry.
    NPAGED_LOOKASIDE_LIST ConfigEntryFreeMemPool;

    // Config table.
    RTL_AVL_TABLE ConfigTable;

} NTFZ_CORE_GLOBALS, *PNTFZ_CORE_GLOBALS;

extern NTFZ_CORE_GLOBALS Globals;

/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS
NtFreezerPreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
NtFreezerPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
NtFreezerPreOperationNoPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
); 


/*************************************************************************
    Message handler routine.
*************************************************************************/
NTSTATUS NTFZCoreMessageHandlerRoutine(
    _In_opt_ PVOID ConnectionCookie,
    _In_reads_bytes_opt_(InputBytes) PVOID Input,
    _In_ ULONG InputBytes,
    _Out_writes_bytes_to_opt_(OutputBytes, *ReturnBytes) PVOID Output,
    _In_ ULONG OutputBytes,
    _Out_ PULONG ReturnBytes
);

// Query a config from table by index.
NTSTATUS QueryConfigFromTable(
    _In_  PNTFZ_CONFIG QueryConfigEntry,
    _Out_ PNTFZ_CONFIG ResultConfigEntry
);

// Add a config to table.
NTSTATUS AddConfigToTable(
    _In_ PNTFZ_CONFIG InsertConfigEntry
);

// Find the config from table by index and remove it.
NTSTATUS RemoveConfigFromTable(
    _In_ PNTFZ_CONFIG RemoveConfigEntry
);

// Cleanup config table and release configs memory.
NTSTATUS CleanupConfigTable(
    VOID
);

#endif