#ifndef __cpt_data_h_
#define __cpt_data_h_

#pragma once

#pragma pack(push, 1)

struct cpt_rect
{
    cpt_rect():x(0), y(0), width(0), height(0){}
    cpt_rect(int x, int y, int width, int height):x(x), y(y), width(width), height(height){}

	int x;
	int y;
	int width;
	int height;

	bool operator== (const cpt_rect& rect) const
	{
		return (x == rect.x) && 
			(y == rect.y) &&
			(width == rect.width) &&
			(height == rect.height);
	}
    
    bool operator< (const cpt_rect& rect) const
    {
        if(x == rect.x)
        {
            return (y < rect.y);
        }
        else
            return (x < rect.x);
    }
};

struct cpt_point
{
	int x;
	int y;
};

typedef cpt_point cpt_size;

struct cpt_color
{
	float a;
	short r;
	short g;
	short b;
};

enum cpt_error_type
{	
	cpt_host_create_error = -6,
	cpt_host_terminate = -5,
	cpt_fail_to_call_host = -4,
	cpt_invalid_role = -3,
	cpt_send_fail = -2,
	cpt_error = -1,
	cpt_ok = 0,
	cpt_install_fail = 1000,
	cpt_install_fail_no_auth = 1001
};

enum cpt_data_type
{
	cpt_dt_display_device = 200,
	cpt_dt_screen_capture,
	cpt_dt_mouse_pointer,
	cpt_dt_mouse_pos
};

const short filter_include = 1;
const short filter_exclude = 2;

const short filter_windows = 1;
const short filter_process = 2;

struct sc_objects_filter_list
{
	int size;
	short filter_mode;
	short object_type;
	int count;
	void* list[4];
};

enum cpt_sharing_caps
{
	sc_render_frame_rate = 100,
	sc_render_threshold_of_sending_frame,
	sc_render_suspended,
    sc_capturer_ios_use_back_thread,
    sc_capturer_use_retina_scale,
	sc_capturer_use_refresh_rect,
	sc_capturer_scan_rate,
	sc_capturer_filter_objects_for_sharing,
	sc_capturer_use_scan_seed,
	sc_capturer_focus_input_event,
	sc_capturer_disable_aero,
	sc_capturer_enable_border,
	sc_capturer_video_capture,
	sc_capturer_enable_filter,
	sc_capturer_capture_mode,
    sc_capturer_enable_airhost_push_as_data
};

enum sharing_capture_mode
{
	sc_cm_auto = 0,
	sc_cm_bitblt
};

enum ios_sharing_device
{
    isd_none,
    isd_device_screen,
    isd_own_app
};

enum cpt_sharing_msg
{
	sm_capturer_init_finished = 250,
	sm_shared_window_closed,
	sm_shared_window_covered,
	sm_shared_window_paused,
	sm_shared_window_resume,
	sm_shared_process_quit,
	sm_capture_process_quit,
	sm_shared_device_disconnected,
	sm_shared_window_frame_changed,
	sm_shared_object_will_be_shared,
    sm_capture_diagnostic_data,
    sm_wired_ios_device_connected,
    sm_capture_video_content,
#ifdef __IOS__
    sm_ios_capture_device_started,
    sm_ios_capture_device_paused,
    sm_ios_capture_device_resumed,
    sm_ios_capture_device_stopped
#endif
};

enum cpt_input_mouse_event_type
{
	rc_mouse_invalid_event = 0,
	rc_mouse_move = 0x10,
	rc_mouse_left_down,
	rc_mouse_left_up,
	rc_mouse_left_dbl_down,
	rc_mouse_right_down,
	rc_mouse_right_up,
	rc_mouse_right_dbl_down,
	rc_mouse_middle_down,
	rc_mouse_middle_up,
	rc_mouse_middle_dbl_down,
	rc_mouse_wheel,
	rc_mouse_dragged,
	rc_mouse_right_dragged,
	rc_mouse_middle_dragged
};

typedef cpt_input_mouse_event_type cpt_rc_mouse_event_type;

const short rc_mouse_flag_precise_wheel = 0x01;

enum cpt_input_key_event_type
{
	rc_key_down = 0x20,
	rc_key_up,
	rc_key_char,
	rc_key_active,
	rc_key_inactive
};

enum cpt_vkey_code
{
	vkey_backspace = 0x08,
	vkey_return = 0x0D
};

typedef cpt_input_key_event_type cpt_rc_key_event_type;

const int rc_key_flag_extended = 1;
const int rc_key_flag_altdown = 2;
const int rc_key_flag_record_chars = 4; //send through record characters, no down and up events involved
const int rc_key_flag_repeat = 8;
const int rc_key_flag_simulated = 0x10;

enum cpt_rc_clipboard_event_type
{
	rc_clip_text = 0x30
};

const int sm_param_window_nocovered = 0;
const int sm_param_window_covered = 1;
const int sm_param_window_minimized = 2;
const int sm_param_window_region_changing = 4;
const int sm_param_window_in_other_desktop = 8;
const int sm_param_window_client_call = 0x10;
const int sm_param_window_moving = 0x20;

struct sm_param_pause_extra
{
	int desktop_sharing;
};

typedef struct _cpt_common
{
	unsigned int size;
	unsigned short data_type;
}cpt_common;

struct cpt_device_info
{
	cpt_common cmn;
    float scale;
	RECT rect_of_shared_devices;
	RECT rect_of_shared_objects;
	cpt_rect rect_of_valid_area;
};

const unsigned int BI_YUV = 100;
const unsigned int BI_YUV_F = 101;
const unsigned int BI_NV12 = 102;
const unsigned int BI_NV12_F = 103;

struct cpt_screen_data
{
	cpt_common cmn;
	RECT rect_of_source_device;
	unsigned short left;
	unsigned short top;
	BITMAPINFO bi;
	unsigned short visible_rgn_data_len;
	unsigned short invisible_rgn_data_len;
	unsigned short back_rgn_data_len;
	unsigned int updated_rgn_data_len;
	unsigned int index;
	unsigned char region[1];

	//address of region must be 4 bytes aligned.
	LPRGNDATA get_visible_rgn_data()
	{
		return reinterpret_cast<LPRGNDATA>(region);
	}

	LPRGNDATA get_invisible_rgn_data()
	{
		return (LPRGNDATA)((unsigned char*)(get_visible_rgn_data())+visible_rgn_data_len);
	}

	LPRGNDATA get_back_rgn_data()
	{
		return (LPRGNDATA)((unsigned char*)(get_invisible_rgn_data())+invisible_rgn_data_len);
	}

	LPRGNDATA get_updated_rgn_data()
	{
		return (LPRGNDATA)((unsigned char*)(get_back_rgn_data())+back_rgn_data_len);
	}

	unsigned char* get_image_bits()
	{
		return reinterpret_cast<unsigned char*>(get_updated_rgn_data())+updated_rgn_data_len;
	}
};

struct cpt_mouse_pointer_data
{
	cpt_common cmn;
	HICON cursor;
	CURSORSHAPE pointer_shape;
	bool is_alpha;
	//the first half is AND mask and the second half is XOR mask.
	unsigned char pointer_bits[1];
};

struct cpt_mouse_position_data
{
	cpt_common cmn;
	POINT pos;
	int extra_info_of_pos;
};

const POINT INVALID_MOUSE_POS = {-32000, -32000};

const RECT INVALID_FRAME_RECT = {-999999, -999999, -999999, -999999};

#ifdef _WIN32
const wchar_t FRAME_WINDOW_CLASS[] = TEXT("sharing frame");
const wchar_t INDICATOR_WINDOW_CLASS[] = TEXT("unsharing indicator frame");
const wchar_t PROP_INVISIBLE_LAYERED_WINDOW[] = TEXT("PROP_INVISIBLE_LAYERED_WINDOW");
#endif

#pragma pack(pop)
#endif
