/*++

    The MIT License (MIT)

    Copyright (c) 2023 Fxtack

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

Module Name:

    FileGuardCore.c

Abstract:

    Basic declarations of FileGuardCore driver.

Environment:

    Kernel/User mode.

--*/

#ifndef __FILE_GUARD_H__
#define __FILE_GUARD_H__

#pragma warning(push)
#pragma warning(disable:4200 4201)

#define FG_CORE_CONTROL_PORT_NAME L"\\FileGuardControlPort"
#define FG_MONITOR_PORT_NAME      L"\\FileGuardMonitorPort"

/*-------------------------------------------------------------
    Core communication message
-------------------------------------------------------------*/

typedef enum _FG_MESSAGE_TYPE {
    GetCoreVersion,
    AddRules,
    RemoveRules,
    QueryRules,
    CheckMatchedRule,
    CleanupRules
} FG_MESSAGE_TYPE;

typedef struct _FG_CORE_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Patch;
    USHORT Build;
} FG_CORE_VERSION, *PFG_CORE_VERSION;

typedef enum _FG_RUEL_CODE {
    RuleNone,
    RuleAccessDenined,
    RuleReadOnly,
    RuleMaximum
} FG_RUEL_CODE, *PFG_RUEL_CODE;

#define VALID_RULE_CODE(_rule_code_) ((_rule_code_) > RuleNone && (_rule_code_) < RuleMaximum)

typedef struct _FG_RULE {
    FG_RUEL_CODE RuleCode;
    USHORT PathExpressionSize; // The bytes size of `FilePathName`, contain null wide char.
    WCHAR PathExpression[];    // End of null.
} FG_RULE, *PFG_RULE;

//
// Message of user application send to core.
//
typedef struct _FG_MESSAGE {
    FG_MESSAGE_TYPE Type;
    ULONG MessageSize;
    union {
        struct {
            USHORT RulesAmount;
            ULONG RulesSize;
            UCHAR Rules[];
        } DUMMYSTRUCTNAME;
        struct {
            USHORT PathNameSize;
            WCHAR PathName[];
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} FG_MESSAGE, *PFG_MESSAGE;

//
// Message result of core returned.
//
typedef struct _FG_MESSAGE_RESULT {
    ULONG ResultCode;
    ULONG ResultSize;
    union {
        FG_CORE_VERSION CoreVersion;
        ULONG AffectedRulesAmount;
        struct {
            USHORT RulesAmount;
            ULONG RulesSize;
            UCHAR RulesBuffer[];
        } Rules;
        FG_RULE MatchedRule;
    } DUMMYUNIONNAME;
} FG_MESSAGE_RESULT, *PFG_MESSAGE_RESULT;

typedef struct _FG_FILE_ID_DESCRIPTOR {
    
    //
    // The GUID name of volume that file belong to.
    //
    WCHAR VolumeGuidName;
    
    //
    // The ID of the file on the file system volume.
    //
    union {

        //
        // NTFS support 64 bits file ID.
        //
        LARGE_INTEGER FileId64;

        //
        // Other file systems may support 128 bits file ID.
        //
        FILE_ID_128 FileId128;  

    } FileId;

} FG_FILE_ID_DESCRIPTOR, *PFG_FILE_ID_DESCRIPTOR;

#pragma warning(pop)

/*-------------------------------------------------------------
    Monitor structures
-------------------------------------------------------------*/

typedef struct _FG_MONITOR_RECORD {

    //
    // The time when the rules were successfully matched.
    //
    LARGE_INTEGER Time;

    //
    // The rule successfully matched.
    //
    FG_RULE Rule;

} FG_MONITOR_RECORD, *PFG_MONITOR_RECORD;

#define MONITOR_RECORDS_MESSAGE_BODY_BUFFER_SIZE (32 * 1024)

typedef struct _FG_RECORD_MESSAGE_BODY {

    //
    // Bytes size of data.
    //
    ULONG DataSize;

    //
    // Data buffer.
    //
    UCHAR DataBuffer[MONITOR_RECORDS_MESSAGE_BODY_BUFFER_SIZE];

} FG_RECORDS_MESSAGE_BODY, *PFG_RECORDS_MESSAGE_BODY;

typedef struct _FG_MONITOR_RECORDS_MESSAGE {

    //
    // Message header.
    //
    FILTER_MESSAGE_HEADER MessageHeader;

    //
    // Message body.
    //
    FG_RECORDS_MESSAGE_BODY	MessageBody;

} FG_MONITOR_RECORDS_MESSAGE, *PFG_MONITOR_RECORDS_MESSAGE;

#endif