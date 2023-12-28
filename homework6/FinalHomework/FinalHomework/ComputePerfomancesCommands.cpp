#include "ComputePerfomancesCommands.h"


string SIDInfo::getCurrentUserSID() {
    string currentUserSID;
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        DWORD tokenInfoLength = 0;
        GetTokenInformation(token, TokenUser, NULL, 0, &tokenInfoLength);
        TOKEN_USER* tokenUser = (TOKEN_USER*)malloc(tokenInfoLength);
        if (GetTokenInformation(token, TokenUser, tokenUser, tokenInfoLength, &tokenInfoLength)) {
            LPSTR userSID;
            if (ConvertSidToStringSid(tokenUser->User.Sid, &userSID)) {
                currentUserSID = userSID;
                LocalFree(userSID);
            }
        }
        free(tokenUser);
        CloseHandle(token);
    }
    return currentUserSID;
}

string SIDInfo::getEveryoneGroupSID() {
    string everyoneGroupSID;
    PSID everyoneSID;
    SID_IDENTIFIER_AUTHORITY worldAuth = SECURITY_WORLD_SID_AUTHORITY;
    if (AllocateAndInitializeSid(&worldAuth, 1, SECURITY_WORLD_RID,
        0, 0, 0, 0, 0, 0, 0, &everyoneSID)) {
        LPSTR everyoneSIDStr;
        if (ConvertSidToStringSid(everyoneSID, &everyoneSIDStr)) {
            everyoneGroupSID = everyoneSIDStr;
            LocalFree(everyoneSIDStr);
        }
        FreeSid(everyoneSID);
    }
    return everyoneGroupSID;
}

string SIDInfo::getAdminGroupSID() {
    string adminGroupSID;
    PSID adminSID;
    SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuth, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminSID)) {
        LPSTR adminSIDStr;
        if (ConvertSidToStringSid(adminSID, &adminSIDStr)) {
            adminGroupSID = adminSIDStr;
            LocalFree(adminSIDStr);
        }
        FreeSid(adminSID);
    }
    return adminGroupSID;
}


string SIDInfo::getSIDInfo()
{
    string result = "Current User SID: " + this->getCurrentUserSID() + "\r\n";
    result += "Everyone Group SID: " + this->getEveryoneGroupSID() + "\r\n";
    result += "Administrators Group SID: " + this->getAdminGroupSID() + "\r\n";
	return result;
}

void SIDInfoCommand::execute(myPerfResults& result)
{
	result.sidInfo = this->sidInfo.getSIDInfo();
}


string HTInfo::getLogicalProcessorInformation()
{
    ostringstream stream;

    DWORD returnLength = 0;
    BOOL result = GetLogicalProcessorInformation(nullptr, &returnLength);
    if (result == FALSE && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(returnLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

        if (GetLogicalProcessorInformation(buffer.data(), &returnLength)) {
            for (const auto& info : buffer) {
                bitset<sizeof(ULONG_PTR) * 8> procMask(info.ProcessorMask);
                stream << "ProcessorMask: 0x" << std::hex << procMask << ";\r\nRelationship: ";
                switch (info.Relationship) {
                case RelationProcessorCore:
                    stream << "RelationProcessorCore";
                    break;
                case RelationNumaNode:
                    stream << "RelationNumaNode";
                    break;
                case RelationCache:
                    stream << "RelationCache";
                    break;
                case RelationProcessorPackage:
                    stream << "RelationProcessorPackage";
                    break;
                default:
                    stream << "Other";
                    break;
                }
                stream << std::dec << "\r\n\r\n";
            }
        }
        else {
            stream << "Failed to retrieve processor information.\r\n";
        }
    }
    else {
        stream << "Failed to retrieve buffer size for processor information.\r\n";
    }
    return stream.str();
}

void HTInfoCommand::execute(myPerfResults& result)
{
	result.HTApi = this->htInfo.getLogicalProcessorInformation();
}


string NUMAInfo::getNumaInfo()
{
    ostringstream stream;

    ULONG highestNodeNumber = 0;
    if (GetNumaHighestNodeNumber(&highestNodeNumber)) {
        stream << "Highest NUMA Node Number: " << highestNodeNumber << "; \r\n\r\n";
    }
    else {
        stream << "Failed to retrieve the highest NUMA node number. \r\n\r\n";
        return stream.str();
    }

    for (ULONG nodeId = 0; nodeId <= highestNodeNumber; ++nodeId) {
        ULONGLONG nodeProcessorMask = 0;
        if (GetNumaNodeProcessorMask(nodeId, &nodeProcessorMask)) {
            bitset<sizeof(ULONGLONG) * 8> procMask(nodeProcessorMask);
            stream << "NUMA Node " << nodeId << "; Processor Mask: 0x"
                << hex << procMask.to_ullong() << dec << "; \r\n\r\n";
        }
        else {
            stream << "Failed to retrieve processor mask for NUMA node " << nodeId << "; \r\n\r\n";
        }
    }

    stream << "Total NUMA Node Count: " << highestNodeNumber + 1 << "; \r\n\r\n";

    return stream.str();
}

void NUMAInfoCommmand::execute(myPerfResults& result)
{
	result.NUMPAApi = this->numaInfo.getNumaInfo();
}


/**
* Method responsible for retrieving and format the default CPU set IDs for the current process
*/
string CPUInfo::getProcessDefaultCpuSetIDs()
{
    ostringstream stream;
    DWORD requiredSize = 0;
    GetProcessDefaultCpuSets(GetCurrentProcess(), nullptr, 0, &requiredSize);
    vector<DWORD> cpuSetIds(requiredSize);

    if (GetProcessDefaultCpuSets(GetCurrentProcess(), cpuSetIds.data(), requiredSize, &requiredSize)) {
        stream << "Process Default CPU Set IDs: ";
        for (DWORD id : cpuSetIds) {
            stream << id << " ";
        }
        if (cpuSetIds.size() == 0)
        {
            stream << "None";
        }
        stream << "\r\nNumber of Default CPU Set IDs: " << cpuSetIds.size() << "\r\n\r\n";
    }
    else {
        stream << "Failed to retrieve default CPU set IDs for the process.\r\n\r\n";
    }
    return stream.str();
}

/**
* Method responsible for retrieving and format teh system CPU set information
*/
string CPUInfo::getSystemCpuSetInformation() {
    ostringstream stream;
    DWORD requiredSize = 0;
    GetSystemCpuSetInformation(nullptr, 0, &requiredSize, GetCurrentProcess(), 0);
    vector<BYTE> cpuSetInfoBuffer(requiredSize);
    SYSTEM_CPU_SET_INFORMATION* cpuSetInfo = reinterpret_cast<SYSTEM_CPU_SET_INFORMATION*>(cpuSetInfoBuffer.data());

    if (GetSystemCpuSetInformation(cpuSetInfo, requiredSize, &requiredSize, GetCurrentProcess(), 0)) {
        DWORD numberOfEntries = requiredSize / sizeof(SYSTEM_CPU_SET_INFORMATION);
        stream << "System CPU Set Information:\r\n";
        for (DWORD i = 0; i < numberOfEntries; ++i) {
            stream << "CPU Set ID: " << cpuSetInfo[i].CpuSet.Id << "\r\n";
        }
        stream << "Total Number of CPU Set Entries: " << numberOfEntries << "\r\n\r\n";
    }
    else {
        stream << "Failed to retrieve system CPU set information.\r\n\r\n";
    }
    return stream.str();
}


string CPUInfo::getCPUInfo()
{
    string result = this->getProcessDefaultCpuSetIDs();
    result += this->getSystemCpuSetInformation();
    return result;
}

void CPUInfoCommand::execute(myPerfResults& result)
{
	result.CPUApi = this->cpuInfo.getCPUInfo();
}
