#include "StdAfx.h"
#include "ASFrameRes.h"
#include "ASRecordPackage.h"

ASFrameRes::ASFrameRes()
: current_frame_index_(-1), record_package_(0), current_screen_image_(0), current_cursor_image_(0), nearby_screen_images_index_(-1)
{
	memset(current_screen_regions_, 0, sizeof(current_screen_regions_));
	memset(nearby_screen_images_, 0, sizeof(nearby_screen_images_));
}

ASFrameRes::~ASFrameRes()
{
	FreeRes();
}

void ASFrameRes::SetRecordPackage( IASRecordPackage* pkg )
{
	record_package_ = pkg;
}

bool ASFrameRes::UpdateForFrame( int frame_idx )
{
	if (frame_idx != current_frame_index_)
	{
		FreeRes();
	}

	current_frame_index_ = frame_idx;

	current_screen_image_ = record_package_->GetFrameScreen(frame_idx, &current_screen_rect_, current_screen_regions_);
	return true;
}

HICON ASFrameRes::CursorImage()
{
	if (NULL == current_cursor_image_)
	{
		current_cursor_image_ = record_package_->GetFrameCursor(current_frame_index_, &current_cursor_pos_);
	}
	return current_cursor_image_;
}

POINT ASFrameRes::CursorPosition()
{
	if (NULL == current_cursor_image_)
	{
		current_cursor_image_ = record_package_->GetFrameCursor(current_frame_index_, &current_cursor_pos_);
	}
	return current_cursor_pos_;
}

HBITMAP* ASFrameRes::NearbyImages()
{
	if (current_frame_index_ != nearby_screen_images_index_)
	{
		UpdateNearbyImages();
	}
	return nearby_screen_images_;
}

void ASFrameRes::FreeRes()
{
	if (current_screen_image_)
	{
		record_package_->FreeFrameScreen(current_screen_image_);
		current_screen_image_ = 0;
	}

	if (current_cursor_image_)
	{
		record_package_->FreeFrameCursor(current_cursor_image_);
		current_cursor_image_ = 0;
	}

	for (int i = 0; i < ARRAYSIZE(current_screen_regions_); ++i)
	{
		if (current_screen_regions_[i])
		{
			DeleteObject(current_screen_regions_[i]);
			current_screen_regions_[i] = 0;
		}
	}
}

struct	frame_range
{
	int first;	
	int last; 

	static inline frame_range range_nearby(int base, int forward, int backward, int minimize, int maxmize)
	{
		frame_range range_target = { base - forward, base + backward };
		if (range_target.first < minimize)	range_target.first = minimize;
		if (range_target.last >= maxmize) range_target.last = maxmize;

		if (range_target.first > range_target.last)
		{
			range_target.first = range_target.last = minimize - 1;
		}

		return range_target;
	}

	static frame_range range_diff(frame_range range1, frame_range range2, frame_range& range1_exceed, frame_range& range2_exceed, frame_range& range_intersect)
	{
		

	}
};

void ASFrameRes::UpdateNearbyImages()
{
	int record_frame_count = record_package_->GetFrameCount();

	HBITMAP bitmaps[ARRAYSIZE(nearby_screen_images_)] = {0};

	for (int i = current_frame_index_ - 3; i <= current_frame_index_ + 3; ++i)
	{
		if (i >= 0 && i < record_frame_count)
		{
			if (i >= nearby_screen_images_index_ - 3 && i <= nearby_screen_images_index_ + 3)
			{
				bitmaps[i - current_frame_index_ + 3] = nearby_screen_images_[i - nearby_screen_images_index_ + 3];
				nearby_screen_images_[i - nearby_screen_images_index_ + 3] = 0;
			}
			else
			{
				bitmaps[i - current_frame_index_ + 3] = record_package_->GetFrameScreen(i, 0, 0);
			}
		}
	}

	for (int i = 0; i < ARRAYSIZE(nearby_screen_images_); ++i)
	{
		if (nearby_screen_images_[i])
		{
			record_package_->FreeFrameScreen(nearby_screen_images_[i]);
		}
	}

	memcpy(nearby_screen_images_, bitmaps, sizeof(nearby_screen_images_));
	nearby_screen_images_index_ = current_frame_index_;
}
