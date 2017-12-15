#pragma once

class ProcessData
{
public:
	ProcessData(void);

	bool InitRunningMettingInfo();
	bool InitRunningMettingLog();

public:
	TCHAR	main_process_name_[64];
	DWORD	main_process_id_;
	DWORD	metting_process_id_;
	DWORD	cpthost_process_id_;

	TCHAR	product_name_[64];
	TCHAR	log_file_names_[24][MAX_PATH];
	DWORD	log_file_count_;

};
