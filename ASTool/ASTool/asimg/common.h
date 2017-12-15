#pragma once
#include "..\..\cpt_data.h"

enum { WM_SET_THUMBS = WM_USER + 1, WM_REFRESH_PANEL, WM_SET_FOCUS_TO_SLIDER_FRAME, WM_SET_TXT_CURSOR_POS, WM_PLAY_SWITCH };
enum EnumRegion { REGION_UPDATE, REGION_VISIBLE, REGION_INVISIBLE, REGION_BACK, REGIONCOUNT };

extern WCHAR				img_log_file_path_[MAX_PATH * 2];
extern HANDLE				img_log_file_handle_;
extern LARGE_INTEGER		log_file_read_pointer_;

extern char*				log_block_buf_;
extern int					log_block_buf_size_;
extern cpt_common*			log_block_data_header_;
extern cpt_screen_data*		log_block_screen_data_;
extern int					log_block_frame_index_;

extern HICON				current_frame_cursor_image_;
extern POINT				current_frame_mouse_position_;


HBITMAP	LoadFrameImage(cpt_screen_data* data);

void save_to_bmp_file(WCHAR* file, BITMAPINFO* pbi, unsigned char* bits);
void save_to_bmp_file1(WCHAR* file, HBITMAP bmp);
void make_export_file_path(int frame_index, TCHAR* buf);