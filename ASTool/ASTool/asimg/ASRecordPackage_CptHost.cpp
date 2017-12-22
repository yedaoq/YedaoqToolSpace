#include "StdAfx.h"
#include "..\..\cpt_data.h"
#include "..\util\util.h"
#include "ASRecordPackage_CptHost.h"

struct file_frame_header
{
	FILETIME	frame_time;
	cpt_common	common;
};

__inline int align_data_32(int width, int bits)
{
	return ((width*bits)+31)/32*4;
}

ASRecordFrameData_CptHost::ASRecordFrameData_CptHost()
: img_log_file_handle_(INVALID_HANDLE_VALUE), alive_record_(false), current_frame_index_(-1)
{
	screen_data_cache_.init(cpt_dt_screen_capture);
	cursor_data_cache_.init(cpt_dt_mouse_pointer);
}

bool ASRecordFrameData_CptHost::LoadFrom( LPCTSTR path )
{
	if (!OpenRecordFile(path))
	{
		SetToolErrorMsg(TEXT("无法打开AS图像日志文件！"));
		return false;
	}

	record_file_read_pointer_.QuadPart = 0;
	record_file_latest_cursorimage_pos_.QuadPart = 0;
	record_file_latest_mousepos_.x = 0;
	record_file_latest_mousepos_.y = 0;

	current_frame_index_ = -1;
	img_frames_.clear();

	if (!LoadRecordFileContent())
	{
		return false;
	}

	return true;
}

int ASRecordFrameData_CptHost::GetFrameCount()
{
	return img_frames_.size();
}

bool ASRecordFrameData_CptHost::IsLiveRecord()
{
	return alive_record_;
}

// int ASRecordFrameData_CptHost::GetCurrentFrameIdx()
// {
// 	return current_frame_index_;
// }

// bool ASRecordFrameData_CptHost::SetCurrentFrameByIdx( int idx )
// {
// 	if (idx < 0 || idx > GetFrameCount() - 1)
// 	{
// 		return false;
// 	}
// 	if (current_frame_index_ != idx)
// 	{
// 		current_frame_index_ = idx;
// 
// 		const img_frame_info& frame_info = img_frames_[idx];
// 		if (!GetFrameData(current_frame_index_, GET_FRAME_CURSOR | GET_FRAME_REGION | GET_FRAME_SCREEN | GET_FRAME_TIME, &current_frame_data_))
// 		{
// 			return false;
// 		}
// 
// 
// 	}
// 	
// 	return true;
// }

// bool ASRecordFrameData_CptHost::MoveToNextFrame()
// {
// 	return SetCurrentFrameByIdx(current_frame_index_ + 1);
// }

bool ASRecordFrameData_CptHost::OpenRecordFile( LPCTSTR path )
{
	alive_record_ = false;
	img_log_file_handle_ = CreateFile(path, FILE_READ_DATA, 0, NULL, OPEN_ALWAYS, NULL, NULL);
	if (INVALID_HANDLE_VALUE == img_log_file_handle_)
	{
		alive_record_ = true;
		img_log_file_handle_ = CreateFile(path, FILE_READ_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, NULL, NULL);
	}

	record_file_read_pointer_.QuadPart = 0;
	record_file_latest_mousepos_.x = record_file_latest_mousepos_.y = 0;
	record_file_latest_cursorimage_pos_.QuadPart = 0;

	InitRecordStartTime();

	return INVALID_HANDLE_VALUE != img_log_file_handle_;
}


bool ASRecordFrameData_CptHost::InitRecordStartTime()
{
	file_frame_header		frame_header;
	DWORD					byte_readed = 0;

	SetFilePointer(img_log_file_handle_, 0, 0, FILE_BEGIN);
	ReadFile(img_log_file_handle_, &frame_header, sizeof(frame_header), &byte_readed, NULL);
	if (byte_readed < sizeof(frame_header))
		return false;

	record_start_time_ = frame_header.frame_time;

	return true;
}

bool ASRecordFrameData_CptHost::LoadRecordFileContent()
{
	img_frame_info			info;
	file_frame_header		frame_header;
	DWORD					byte_readed = 0;

	SetFilePointerEx(img_log_file_handle_, record_file_read_pointer_, NULL, FILE_BEGIN);

	while(true)
	{
		ReadFile(img_log_file_handle_, &frame_header, sizeof(frame_header), &byte_readed, NULL);
		if (byte_readed < sizeof(frame_header)/* || header.common.size > 100000000*/)
		{
			break;
		}

		switch(frame_header.common.data_type)
		{
		case cpt_dt_display_device:
			break;
		case cpt_dt_screen_capture:
			info.frame_data_pos.QuadPart = record_file_read_pointer_.QuadPart + sizeof(FILETIME);
			info.frame_time_miniseconds = GetTimeDeltaMiniseconds(frame_header.frame_time, record_start_time_);
			info.cursur_image_pos.QuadPart = record_file_latest_cursorimage_pos_.QuadPart;
			info.mouse_pos = record_file_latest_mousepos_;
			img_frames_.push_back(info);
			break;
		case cpt_dt_mouse_pointer:
			record_file_latest_cursorimage_pos_.QuadPart = record_file_read_pointer_.QuadPart + sizeof(FILETIME);
			break;
		case cpt_dt_mouse_pos:
			LARGE_INTEGER data_pos;
			data_pos.QuadPart = record_file_read_pointer_.QuadPart + sizeof(FILETIME);
			ReadCaptureData(data_pos, screen_data_cache_);
			record_file_latest_mousepos_ = ParseCursorPosition(screen_data_cache_);
			break;
		default:
			int x= 0;
			break;
		}

		LONG block_size = frame_header.common.size + sizeof(FILETIME);
		record_file_read_pointer_.QuadPart += block_size;

		if (!SetFilePointerEx(img_log_file_handle_, record_file_read_pointer_, NULL, FILE_BEGIN))
		{
			record_file_read_pointer_.QuadPart -= block_size;
			break;
		}
	}

	if (img_frames_.size() <= 0)
	{
		return false;
	}

	//UpdateTimeLine();
	return true;
}

// bool ASRecordFrameData_CptHost::GetFrameData( int idx, DWORD mask, ASRecordFrameData* frame_data )
// {
// 	img_frame_info info = img_frames_[idx];
// 
// 	if (mask & GET_FRAME_CURSOR)
// 	{
// 		frame_data->cursor_pos.x = info.mouse_pos.x;
// 		frame_data->cursor_pos.y = info.mouse_pos.y;
// 		ReadCaptureData(info.cursur_image_pos, screen_data_cache_);
// 		frame_data->cursor_image = ParseCursorImage(screen_data_cache_);
// 	}
// 
// 	if (mask & (GET_FRAME_SCREEN | GET_FRAME_REGION))
// 	{
// 		ReadCaptureData(info.frame_data_pos, screen_data_cache_);
// 		if (mask & GET_FRAME_SCREEN)
// 		{
// 			ParseScreenImage(screen_data_cache_, frame_data);
// 		}
// 		if (mask & GET_FRAME_REGION)
// 		{
// 			ParseScreenRegion(screen_data_cache_, frame_data);
// 		}
// 	}
// 
// 	if (mask & GET_FRAME_TIME)
// 	{
// 		frame_data->frame_time = info.frame_time_miniseconds;
// 	}
// 
// 	return true;
// }

POINT ASRecordFrameData_CptHost::ParseCursorPosition(const capture_data_cache& buf )
{	
	cpt_mouse_position_data* data = buf.as_cpt_mouse_position_data();
	return data->pos;
}

// bool ASRecordFrameData_CptHost::ParseScreenImage( const capture_data_cache& buf, ASRecordFrameData* frame_data )
// {
// 	cpt_screen_data* cpt_screen = buf.as_cpt_screen_data();
// 
// 	BITMAPINFOHEADER bih = cpt_screen->bi.bmiHeader;
// 	frame_data->screen_image = CreateBitmap(bih.biWidth, bih.biHeight < 0 ? -bih.biHeight : bih.biHeight, bih.biPlanes, bih.biBitCount, cpt_screen->get_image_bits());
// 
// 	frame_data->screen_rect.left = cpt_screen->left;
// 	frame_data->screen_rect.top = cpt_screen->top;
// 	frame_data->screen_rect.right = frame_data->screen_rect.left + bih.biWidth;
// 	frame_data->screen_rect.bottom = frame_data->screen_rect.top + abs(bih.biHeight);
// 
// 	return true;
// }
// 
// bool ASRecordFrameData_CptHost::ParseScreenRegion( const capture_data_cache& buf, ASRecordFrameData* frame_data )
// {
// 	cpt_screen_data* cpt_screen = buf.as_cpt_screen_data();
// 
// 	int region_sizes[] = {cpt_screen->updated_rgn_data_len, cpt_screen->visible_rgn_data_len, cpt_screen->invisible_rgn_data_len, cpt_screen->back_rgn_data_len };
// 	LPRGNDATA region_datas[] = { cpt_screen->get_updated_rgn_data(), cpt_screen->get_visible_rgn_data(), cpt_screen->get_invisible_rgn_data(), cpt_screen->get_back_rgn_data() };
// 
// 	for(int i = 0; i < ARRAYSIZE(region_datas); ++i)
// 	{
// 		frame_data->region[i] = ExtCreateRegion(NULL, region_sizes[i], region_datas[i]);
// 	}
// 
// 	return true;
// }

bool ASRecordFrameData_CptHost::ParseScreenRegion( const capture_data_cache& buf, HRGN* rgns )
{
	cpt_screen_data* cpt_screen = buf.as_cpt_screen_data();

	int region_sizes[] = {cpt_screen->updated_rgn_data_len, cpt_screen->visible_rgn_data_len, cpt_screen->invisible_rgn_data_len, cpt_screen->back_rgn_data_len };
	LPRGNDATA region_datas[] = { cpt_screen->get_updated_rgn_data(), cpt_screen->get_visible_rgn_data(), cpt_screen->get_invisible_rgn_data(), cpt_screen->get_back_rgn_data() };

	for(int i = 0; i < ARRAYSIZE(region_datas); ++i)
	{
		rgns[i] = ExtCreateRegion(NULL, region_sizes[i], region_datas[i]);
	}

	return true;
}

bool ASRecordFrameData_CptHost::ReadCaptureData( const LARGE_INTEGER& offset, capture_data_cache& data_buf )
{
	if (0 == offset.QuadPart)
	{
		return false;
	}

	if (data_buf.gdiobj)
	{
		DeleteObject(data_buf.gdiobj);
		data_buf.gdiobj = NULL;
	}

	SetFilePointerEx(img_log_file_handle_, offset, NULL, FILE_BEGIN);

	DWORD bytes_readed = 0;
	ReadFile(img_log_file_handle_, data_buf.buf, sizeof(cpt_common), &bytes_readed, NULL);
	if (bytes_readed < sizeof(cpt_common))
	{
		SetToolErrorMsg(TEXT("读取cpt_common失败！"));
		return false;
	}

	if (data_buf.as_cpt_common()->data_type < cpt_dt_display_device || data_buf.as_cpt_common()->data_type > cpt_dt_mouse_pos)
	{
		throw 0;
	}

	DWORD bytes_needed = data_buf.as_cpt_common()->size - sizeof(cpt_common);
	ReadFile(img_log_file_handle_, data_buf.buf + sizeof(cpt_common), bytes_needed, &bytes_readed, NULL);
	if (bytes_readed < bytes_needed)
	{
		return false;
	}

	data_buf.file_pos.QuadPart = offset.QuadPart;

	return true;
}

HICON ASRecordFrameData_CptHost::ParseCursorImage( const capture_data_cache& buf )
{
	HICON icon = NULL;

	cpt_mouse_pointer_data* data = buf.as_cpt_mouse_pointer_data();

	LPBYTE color_bits = data->pointer_bits + align_data_32(data->pointer_shape.cx, 1)*data->pointer_shape.cy;

	if(data->pointer_shape.BitsPixel == 32)
	{
		HBITMAP bmp_mask = CreateBitmap(data->pointer_shape.cx, data->pointer_shape.cy, 1, 1, data->pointer_bits);

		HDC hdc = ::GetDC(NULL);

		BYTE bis[256];
		BITMAPV5HEADER& bi = *(BITMAPV5HEADER*)bis;

		ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
		bi.bV5Size		= sizeof(BITMAPV5HEADER);
		bi.bV5Width		= data->pointer_shape.cx;
		bi.bV5Height	= -data->pointer_shape.cy;
		bi.bV5Planes	= 1;
		bi.bV5BitCount	= 32;
		bi.bV5Compression = BI_BITFIELDS;
		bi.bV5RedMask   =  0x00FF0000;
		bi.bV5GreenMask =  0x0000FF00;
		bi.bV5BlueMask  =  0x000000FF;
		bi.bV5AlphaMask =  0xFF000000; 
		bi.bV5SizeImage = data->pointer_shape.cbWidth * data->pointer_shape.cy;

		LPBYTE bmp_bits;
		HBITMAP bmp_cursor = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (LPVOID*)&bmp_bits, NULL, 0);
		memcpy(bmp_bits, color_bits, data->pointer_shape.cbWidth * data->pointer_shape.cy);
	/*
			LPDWORD lpbits = (LPDWORD)bmp_bits;
			for(int i = 0; i< cs.cx*cs.cy; i++)
		{
				if((lpbits[i]&0xFF000000)== 0xFF000000)
					lpbits[i] &= 0x6fffffff;
		}
	*/
		::ReleaseDC(NULL, hdc);

		ICONINFO icn_info;
		icn_info.fIcon = FALSE;
		icn_info.xHotspot = data->pointer_shape.xHotSpot;
		icn_info.yHotspot = data->pointer_shape.yHotSpot;
		icn_info.hbmMask = bmp_mask;
		icn_info.hbmColor = bmp_cursor;

		icon = CreateIconIndirect(&icn_info);

		DeleteObject(bmp_mask);
		DeleteObject(bmp_cursor);
	}
	else
	{
		icon = CreateIcon(GetModuleHandle(NULL), data->pointer_shape.cx, data->pointer_shape.cy, data->pointer_shape.Planes, data->pointer_shape.BitsPixel,  data->pointer_bits, color_bits);
	}
	return icon;
}

__int64 ASRecordFrameData_CptHost::GetRecordDurationMiniseconds()
{
	return img_frames_.rbegin()->frame_time_miniseconds;
}

// const ASRecordFrameData* ASRecordFrameData_CptHost::GetCurrentFrameData( DWORD mask )
// {
// 	return &current_frame_data_;
// }

int ASRecordFrameData_CptHost::GetFrameOfTime(__int64 time)
{
	return 0;
}

__int64 ASRecordFrameData_CptHost::GetFrameTime( int frame_idx )
{
	img_frame_info info = img_frames_[frame_idx];
	return info.frame_time_miniseconds;
}

FILETIME ASRecordFrameData_CptHost::GetRecordStartTime()
{
	return record_start_time_;
}

HBITMAP ASRecordFrameData_CptHost::GetFrameScreen( int frame_idx, RECT* image_rect, HRGN* image_rgns )
{
	HBITMAP bitmap_handle = NULL;

	const img_frame_info& info = img_frames_[frame_idx];

	if (info.frame_data_pos.QuadPart != screen_data_cache_.file_pos.QuadPart)
	{
		ReadCaptureData(info.frame_data_pos, screen_data_cache_);
	}
	
	cpt_screen_data* screen_data = screen_data_cache_.as_cpt_screen_data();

	BITMAPINFOHEADER bih = screen_data->bi.bmiHeader;
	bitmap_handle = CreateBitmap(bih.biWidth, bih.biHeight < 0 ? -bih.biHeight : bih.biHeight, bih.biPlanes, bih.biBitCount, screen_data->get_image_bits());

	if (image_rect)
	{
		image_rect->left = screen_data->left;
		image_rect->top = screen_data->top;
		image_rect->right = screen_data->left + screen_data->bi.bmiHeader.biWidth;
		image_rect->bottom = screen_data->top + abs(screen_data->bi.bmiHeader.biHeight);
	}

	if (image_rgns)
	{
		ParseScreenRegion(screen_data_cache_, image_rgns);
	}
	
	return bitmap_handle;
}

HICON ASRecordFrameData_CptHost::GetFrameCursor( int frame_idx, POINT* cursor_pos )
{
	HICON icon = NULL;

	const img_frame_info& info = img_frames_[frame_idx];

	if (cursor_pos)
	{
		cursor_pos->x = info.mouse_pos.x;
		cursor_pos->y = info.mouse_pos.y;
	}

	if (info.cursur_image_pos.QuadPart == cursor_data_cache_.file_pos.QuadPart)
	{
		icon = (HICON)cursor_data_cache_.gdiobj;
	}
	else
	{
		ReadCaptureData(info.cursur_image_pos, cursor_data_cache_);
		icon = ParseCursorImage(cursor_data_cache_);
		cursor_data_cache_.gdiobj = (HANDLE)icon;
	}

	return icon;
}

void ASRecordFrameData_CptHost::FreeFrameScreen( HBITMAP res )
{
	DeleteObject(res);
}

void ASRecordFrameData_CptHost::FreeFrameCursor( HICON res )
{
	if (res != (HICON)cursor_data_cache_.gdiobj)
	{
		DeleteObject(res);
	}
}


ASRecordFrameData_CptHost::capture_data_cache::~capture_data_cache()
{
	uninit();
}

bool ASRecordFrameData_CptHost::capture_data_cache::init(cpt_data_type dt)
{
	switch(dt)
	{
	case cpt_dt_mouse_pointer:
		buf_size = sizeof(cpt_mouse_pointer_data) + 256 * 256 * (4 + 1);
		break;
		
	case cpt_dt_screen_capture:
		buf_size = sizeof(cpt_screen_data) + 3 * (sizeof(RGNDATA) + 4096) + 4096 * 2160 * 4 + 1000;
		break;
	default:
		buf_size = 0;
		buf = 0;
		return false;
	}
	
	if (buf_size > 0)
	{
		buf = (char*)malloc(buf_size);
	}
	
	return 0 != buf;
}

void ASRecordFrameData_CptHost::capture_data_cache::uninit()
{
	if (buf)
	{
		free(buf);
		buf = 0;
		buf_size = 0;
	}
}
