#include "StdAfx.h"
#include "Layout.h"

using namespace NSYedaoqLayout;

LayoutSize LayoutSize::Invalid(-1,-1);
LayoutSize LayoutSize::Zero(0,0);
LayoutPoint LayoutPoint::Zero(0,0);

const ResizeInfo ResizeInfo::FillInfo( NSYedaoqLayout::Resize_Zoomed, 100);
const ResizeInfo ResizeInfo::FixedInfo( NSYedaoqLayout::Resize_Fixed, -1);

const AnchorInfo AnchorInfo::AnchorFront(NSYedaoqLayout::Anchor_Front, 1);
const AnchorInfo AnchorInfo::AnchorCenter(NSYedaoqLayout::Anchor_Center, 1);
const AnchorInfo AnchorInfo::AnchorBack(NSYedaoqLayout::Anchor_Back, 1);

LayoutOffset::operator LayoutSize() const
{
	LayoutSize lsRet(0, 0);
	lsRet.LengthAt(Direction) = Value;
	return lsRet;
}

LayoutOffset::operator LayoutPoint() const
{
	LayoutPoint lpRet(0, 0);
	lpRet.CoordAt(Direction) = Value;
	return lpRet;
}

LayoutSize LayoutSize::operator+(const LayoutSize& size) const
{
	return LayoutSize(Width + size.Width, Height + size.Height);	
}

LayoutSize LayoutSize::operator+(const LayoutPoint& pt) const
{
	return LayoutSize(Width + pt.X, Height + pt.Y);	
}

long& LayoutSize::LengthAt(EnumLayoutDirection dir) 
{
	return (dir == NSYedaoqLayout::Direction_Horizon) ? Width : Height; 
}

long LayoutSize::LengthAt(EnumLayoutDirection dir) const
{
	return (dir == NSYedaoqLayout::Direction_Horizon) ? Width : Height; 
}

LayoutPoint LayoutPoint::operator+(const LayoutSize& size) const
{
	return LayoutPoint(X + size.Width, Y + size.Height); 
}

LayoutPoint LayoutPoint::operator+(const LayoutPoint& pt) const
{
	return LayoutPoint(X + pt.X, Y + pt.Y);
}

long& LayoutPoint::CoordAt(EnumLayoutDirection dir) 
{
	return (dir == NSYedaoqLayout::Direction_Horizon) ? X : Y; 
}

long LayoutPoint::CoordAt(EnumLayoutDirection dir) const
{
	return (dir == NSYedaoqLayout::Direction_Horizon) ? X : Y; 
}

const ResizeInfo& CZoomBlankLayout::GetResizeInfo(EnumLayoutDirection dir)
{
	return (dir == NSYedaoqLayout::Direction_Horizon) ? ResizeInfoH_ : ResizeInfoV_;
}

const ResizeInfo& CCtrlLayout::GetResizeInfo(EnumLayoutDirection dir)
{
	UpdateDefaultSize();

	return (dir == NSYedaoqLayout::Direction_Horizon) ?
ResizeInfoH_ : ResizeInfoV_;
}

const AnchorInfo& CCtrlLayout::GetAnchorInfo(EnumLayoutDirection dir)
{
	return (dir == NSYedaoqLayout::Direction_Horizon) ?
AnchorInfoH_ : AnchorInfoV_;
}

int	CCtrlLayout::UpdateDefaultSize()
{
	if((ResizeInfoH_.Mode != NSYedaoqLayout::Resize_Zoomed && ResizeInfoH_.Fixed == -1)
		|| (ResizeInfoV_.Mode != NSYedaoqLayout::Resize_Zoomed && ResizeInfoV_.Fixed == -1))
	{
		RECT rect;
		::GetWindowRect(GetHWnd(), &rect);

		if(ResizeInfoH_.Mode != NSYedaoqLayout::Resize_Zoomed)
		{
			ResizeInfoH_.Fixed = rect.right - rect.left + 2 * AnchorInfoH_.Padding;
		}

		if(ResizeInfoV_.Mode != NSYedaoqLayout::Resize_Zoomed)
		{
			ResizeInfoV_.Fixed = rect.bottom - rect.top + 2 * AnchorInfoV_.Padding;
		}
	}

	return 1;
}

int	CCtrlLayout::Layout(const LayoutPoint& ptBase, const LayoutSize& lyArea)
{
	LayoutSize	ctrlSize = LayoutSize::Zero;
	LayoutPoint ctrlOffset = LayoutPoint::Zero;

	CalLayout(lyArea.Width, GetResizeInfo(NSYedaoqLayout::Direction_Horizon), GetAnchorInfo(NSYedaoqLayout::Direction_Horizon),
		ctrlSize.Width, ctrlOffset.X);

	CalLayout(lyArea.Height, GetResizeInfo(NSYedaoqLayout::Direction_Vertical), GetAnchorInfo(NSYedaoqLayout::Direction_Vertical),
		ctrlSize.Height, ctrlOffset.Y);

	::MoveWindow(GetHWnd(), ptBase.X + ctrlOffset.X, ptBase.Y + ctrlOffset.Y, ctrlSize.Width, ctrlSize.Height, false);
	return 1;
}

int CCtrlLayout::CalLayout(long lArea, const ResizeInfo& resizeInfo, const AnchorInfo& anchorInfo, long& length, long& offset)
{
	switch(resizeInfo.Mode)
	{
	case NSYedaoqLayout::Resize_Auto:
	case NSYedaoqLayout::Resize_Fixed:
		length = max(0, resizeInfo.Fixed - anchorInfo.Padding * 2);
		switch(anchorInfo.Mode)
		{
		case NSYedaoqLayout::Anchor_Front:
			offset = anchorInfo.Padding;
			break;
		case NSYedaoqLayout::Anchor_Center:
			offset = (lArea - length) / 2;
			break;
		case NSYedaoqLayout::Anchor_Back:
			offset = lArea - length - anchorInfo.Padding;
			break;
		default:
			_ASSERT(FALSE);
			break;
		}
		break;
	case NSYedaoqLayout::Resize_Zoomed:
		length = max(0, (lArea - anchorInfo.Padding * 2));
		offset = anchorInfo.Padding;
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	return 1;
}

CFlowLayout::~CFlowLayout()
{
	Clear();
}

int	CFlowLayout::Layout(const LayoutPoint& ptBase, const LayoutSize& lyArea)
{
	LayoutOffset itemOffset(GetDirection(), 0);
	LayoutOffset itemLength(GetDirection(), 0);
	LayoutSize  itemSize	= lyArea;

	long lAreaLength = lyArea.LengthAt(GetDirection());
	long lDynamicLength = max(lAreaLength - GetFixedItemsLength(), 0);

	for (ItemCollection::iterator iter = Items_.begin(); iter != Items_.end(); ++iter)
	{
		if(itemOffset.Value >= lAreaLength)
		{
			//_ASSERT(FALSE);
			break;
		}

		const ResizeInfo& info = (*iter)->GetResizeInfo(GetDirection());
		switch(info.Mode)
		{
		case NSYedaoqLayout::Resize_Auto:
		case NSYedaoqLayout::Resize_Fixed:
			itemSize.LengthAt(GetDirection()) = info.Fixed;
			break;
		case NSYedaoqLayout::Resize_Zoomed:
			itemSize.LengthAt(GetDirection()) = info.Percent * lDynamicLength / 100;
			break;
		default:
			_ASSERT(FALSE);
			itemSize.LengthAt(GetDirection()) = 0;
			break;
		}

		(*iter)->Layout(ptBase + (LayoutPoint)itemOffset, itemSize);
		itemOffset.Value += itemSize.LengthAt(GetDirection());
	}

	return 1;
}

const ResizeInfo& CFlowLayout::GetResizeInfo(EnumLayoutDirection dir)
{
	static ResizeInfo riFill = ResizeInfo::FillInfo;

	if(ResizeInfo_.Mode == NSYedaoqLayout::Resize_Auto)
	{
		UpdateAutoSize();
	}

	if(dir == Direction_)
	{
		return riFill;
	}
	else
	{
		return ResizeInfo_;
	}
}

int	CFlowLayout::UpdateAutoSize()
{
	LayoutOffset offset;
	offset.Value = 0;
	offset.Direction = (GetDirection() == NSYedaoqLayout::Direction_Horizon)
		? NSYedaoqLayout::Direction_Vertical : NSYedaoqLayout::Direction_Horizon;

	for(ItemCollection::iterator iter = Items_.begin(); iter != Items_.end(); ++iter)
	{
		const ResizeInfo& info = (*iter)->GetResizeInfo(offset.Direction);
		switch(info.Mode)
		{
		case NSYedaoqLayout::Resize_Auto:
		case NSYedaoqLayout::Resize_Fixed:
			if(offset.Value < info.Fixed)
			{
				offset.Value = info.Fixed;
			}
			break;
		case NSYedaoqLayout::Resize_Zoomed:
			break;
		default:
			_ASSERT(FALSE);
			break;
		}
	}

	ResizeInfo_.Fixed = offset.Value;
	return 1;
}

long CFlowLayout::GetFixedItemsLength()
{
	long lRet = 0;
	for (ItemCollection::iterator iter = Items_.begin(); iter != Items_.end(); ++iter)
	{
		const ResizeInfo& info = (*iter)->GetResizeInfo(GetDirection());
		switch(info.Mode)
		{
		case NSYedaoqLayout::Resize_Auto:
		case NSYedaoqLayout::Resize_Fixed:
			lRet += info.Fixed;
			break;
		case NSYedaoqLayout::Resize_Zoomed:
			break;
		default:
			_ASSERT(FALSE);
			break;
		}
	}

	return lRet;
}

CFlowLayout* CFlowLayout::AddFlow(EnumLayoutDirection dir, ResizeInfo resizeInfo)
{
	CFlowLayout* pItem = new CFlowLayout(dir, resizeInfo);
	Items_.push_back(pItem);
	return pItem;
}

CCtrlLayout* CFlowLayout::AddCtrl(HWND hWnd,ResizeInfo resizeInfoH, ResizeInfo resizeInfoV, AnchorInfo anchorInfoH, AnchorInfo anchorInfoV)
{
	CCtrlLayout* pItem = new CCtrlLayout(hWnd, resizeInfoH, resizeInfoV, anchorInfoH, anchorInfoV);
	Items_.push_back(pItem);
	return pItem;
}

CZoomBlankLayout* CFlowLayout::AddZoomBlank(long percentH, long percentV)
{
	CZoomBlankLayout* pItem = new CZoomBlankLayout(percentH, percentV);
	Items_.push_back(pItem);
	return pItem;
}

ILayout* CFlowLayout::AddItem(const ILayout& item)
{
	ILayout* pItem = item.Clone();
	Items_.push_back(pItem);
	return pItem;
}

void CFlowLayout::Clear()
{
	for (ItemCollection::iterator iter = Items_.begin(); iter != Items_.end(); ++iter)
	{
		delete *iter;
		*iter = 0;
	}

	Items_.clear();
}

int	CFlowLayout::Count() const
{
	return Items_.size();
}

