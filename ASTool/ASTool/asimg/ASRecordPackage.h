#pragma once

enum {
	GET_FRAME_SCREEN = 1,
	GET_FRAME_CURSOR = 2,
	GET_FRAME_REGION = 4,
	GET_FRAME_TIME = 8,
};

// struct ASRecordFrameData
// {
// 	__int64 frame_time;
// 	HBITMAP	screen_image;
// 	RECT	screen_rect;
// 	HICON   cursor_image;
// 	POINT	cursor_pos;
// 	HRGN	region[REGIONCOUNT];
// 
// 	ASRecordFrameData()
// 	{
// 		memset(this, sizeof(ASRecordFrameData), 0);
// 	}
// };

struct IASRecordPackage
{
public:
	virtual ~IASRecordPackage(void) = 0 {};

	virtual bool		LoadFrom(LPCTSTR path) = 0;

	virtual int			GetFrameCount() = 0;
	virtual __int64		GetRecordDurationMiniseconds() = 0;
	virtual	FILETIME	GetRecordStartTime() = 0;

	virtual bool		IsLiveRecord() = 0;
	
	//virtual int			GetCurrentFrameIdx() = 0;
	//virtual bool		SetCurrentFrameByIdx(int idx) = 0;
	//virtual bool		MoveToNextFrame() = 0;

	virtual int			GetFrameOfTime(__int64 time) = 0;
	virtual __int64		GetFrameTime(int frame_idx) = 0;

	virtual HBITMAP		GetFrameScreen(int frame_idx, RECT* image_rect, HRGN* image_rgns) = 0;
	virtual HICON		GetFrameCursor(int frame_idx, POINT* cursor_pos) = 0;

	virtual	void		FreeFrameScreen(HBITMAP res) = 0;
	virtual	void		FreeFrameCursor(HICON res) = 0;

// 	virtual	bool		GetFrameData(int idx, DWORD mask, ASRecordFrameData* frame_data) = 0;
// 	virtual const ASRecordFrameData* GetCurrentFrameData(DWORD mask) = 0;

};


