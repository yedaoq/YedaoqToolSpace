#pragma once
#include <vector>
#include "ASRecordPackage.h"

class ASRecordFrameData_CptHost : public IASRecordPackage
{
	struct img_frame_info
	{
		__int64			frame_duration_miniseconds;
		__int64			frame_time_miniseconds;
		LARGE_INTEGER	frame_data_pos;
		POINT			mouse_pos;
		LARGE_INTEGER	cursur_image_pos;
	};

	struct capture_data_cache
	{
		HGDIOBJ				gdiobj;
		char*				buf;
		int					buf_size;
		LARGE_INTEGER		file_pos;

		capture_data_cache()
			: buf(0), buf_size(0)
		{}

		~capture_data_cache();

		bool init(cpt_data_type dt);
		void uninit();

		inline const cpt_common* as_cpt_common() const { return reinterpret_cast<const cpt_common*>(buf); }

		inline cpt_screen_data* as_cpt_screen_data() const { return reinterpret_cast<cpt_screen_data*>(buf); }
		inline cpt_mouse_position_data* as_cpt_mouse_position_data() const { return reinterpret_cast<cpt_mouse_position_data*>(buf); }
		inline cpt_mouse_pointer_data* as_cpt_mouse_pointer_data() const { return reinterpret_cast<cpt_mouse_pointer_data*>(buf); }
		
	};

public:
	ASRecordFrameData_CptHost();

	virtual bool		LoadFrom( LPCTSTR path );

	virtual int			GetFrameCount();
	virtual __int64		GetRecordDurationMiniseconds();
	virtual	FILETIME	GetRecordStartTime();

	virtual bool		IsLiveRecord();

	//virtual int			GetCurrentFrameIdx();

	//virtual bool		SetCurrentFrameByIdx( int idx );

	//virtual bool		MoveToNextFrame();

	virtual int			GetFrameOfTime(__int64 time);

	virtual HBITMAP		GetFrameScreen(int frame_idx, RECT* image_rect, HRGN* image_rgns);
	virtual HICON		GetFrameCursor(int frame_idx, POINT* cursor_pos);
	virtual __int64		GetFrameTime(int frame_idx);

	virtual	void		FreeFrameScreen(HBITMAP res);
	virtual	void		FreeFrameCursor(HICON res);

// 	virtual	bool		GetFrameData(int idx, DWORD mask, ASRecordFrameData* frame_data);
// 	virtual const ASRecordFrameData* GetCurrentFrameData(DWORD mask);

protected:
	bool OpenRecordFile(LPCTSTR path);
	bool LoadRecordFileContent();

	POINT	ParseCursorPosition(const capture_data_cache& buf );
	HICON   ParseCursorImage(const capture_data_cache& buf);
// 	bool	ParseScreenImage(const capture_data_cache& buf, ASRecordFrameData* frame_data);
// 	bool	ParseScreenRegion(const capture_data_cache& buf, ASRecordFrameData* frame_data);
	bool	ParseScreenRegion(const capture_data_cache& buf, HRGN* rgns );

	bool	InitRecordStartTime();

	bool	ReadCaptureData(const LARGE_INTEGER& offset, capture_data_cache& data_buf);

protected:
	HANDLE		img_log_file_handle_;	
	bool		alive_record_;

	std::vector<img_frame_info> img_frames_;
	int					current_frame_index_;
	//ASRecordFrameData	current_frame_data_;

	FILETIME			record_start_time_;

	// 将日志文件读取指针保存为成员变量：对正在写入的文件，可以多次调用LoadRecordFileContent读取更新的内容
	LARGE_INTEGER	record_file_read_pointer_;
	LARGE_INTEGER	record_file_latest_cursorimage_pos_;
	POINT			record_file_latest_mousepos_;

	// read cache
	capture_data_cache	screen_data_cache_;
	capture_data_cache	cursor_data_cache_;

};

