#pragma once

#include <Windows.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "pe_sieve_types.h"
#include "peconv.h"

typedef enum module_scan_status {
	SCAN_ERROR = -1,
	SCAN_NOT_MODIFIED = 0,
	SCAN_MODIFIED = 1
} t_scan_status;

class ModuleScanReport
{
public:
	static t_scan_status get_scan_status(const ModuleScanReport *report)
	{
		if (report == nullptr) {
			return SCAN_ERROR;
		}
		return report->status;
	}

	ModuleScanReport(HANDLE processHandle, HMODULE _module, t_scan_status _status)
	{
		this->pid = GetProcessId(processHandle);
		this->module = _module;
		this->status = _status;
	}

	ModuleScanReport(HANDLE processHandle, HMODULE _module)
	{
		this->pid = GetProcessId(processHandle);
		this->module = _module;
		this->status = SCAN_NOT_MODIFIED;
	}

	virtual ~ModuleScanReport() {}

	const virtual bool toJSON(std::stringstream &outs)
	{
		//outs << "\"pid\" : ";
		//outs << std::hex << pid << ",\n";
		outs << "\"module\" : ";
		outs << "\"" << std::hex << (ULONGLONG) module << "\"" << ",\n";
		outs << "\"status\" : " ;
		outs << status;
		return true;
	}

	virtual size_t generateTags(std::string reportPath) { return 0; }

	HMODULE module;
	DWORD pid;
	t_scan_status status;
};

class UnreachableModuleReport : public ModuleScanReport
{
public:
	UnreachableModuleReport(HANDLE processHandle, HMODULE _module)
		: ModuleScanReport(processHandle, _module, SCAN_MODIFIED)
	{
	}

	const virtual bool toJSON(std::stringstream &outs)
	{
		outs << "\"unreachable_scan\" : ";
		outs << "{\n";
		ModuleScanReport::toJSON(outs);
		outs << "\n}";
		return true;
	}
};

class WorkingSetScanReport : public ModuleScanReport
{
public:
	WorkingSetScanReport(HANDLE processHandle, HMODULE _module, t_scan_status status)
		: ModuleScanReport(processHandle, _module, status)
	{
	}

	const virtual bool toJSON(std::stringstream &outs)
	{
		outs << "\"workingset_scan\" : ";
		outs << "{\n";
		ModuleScanReport::toJSON(outs);
		outs << ",\n";
		outs << "\"is_rwx\" : "; 
		outs << std::dec << is_rwx;
		outs << ",\n";
		outs << "\"is_manually_loaded\" : "; 
		outs << std::dec << is_manually_loaded;
		outs << "\n}";
		return true;
	}
	bool is_rwx;
	bool is_manually_loaded;
};
