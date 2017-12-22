#pragma once
#include "common.h"

struct IASRecordPackage;

class ASFrameRes
{
public:
	ASFrameRes();
	~ASFrameRes();

	void		SetRecordPackage(IASRecordPackage* pkg);

	bool		UpdateForFrame(int frame_idx);
	int			GetFrameIndex() const { return current_frame_index_; }

	__int64		FrameTime() const { return current_frame_time_; }
	HBITMAP		ScreenImage() const { return current_screen_image_; }
	const RECT&	ScreenRect() const { return current_screen_rect_; }
	const HRGN* ScreenRegions() const { return current_screen_regions_; }

	HICON		CursorImage();
	POINT		CursorPosition();

	HBITMAP*	NearbyImages();

protected:
	void		FreeRes();
	void		UpdateNearbyImages();

protected:
	int			current_frame_index_;
	IASRecordPackage* record_package_;

	__int64		current_frame_time_;

	HBITMAP		current_screen_image_;
	RECT		current_screen_rect_;
	HRGN		current_screen_regions_[REGIONCOUNT];

	HICON		current_cursor_image_;
	POINT		current_cursor_pos_;

	HBITMAP		nearby_screen_images_[7];
	int			nearby_screen_images_index_;
};


