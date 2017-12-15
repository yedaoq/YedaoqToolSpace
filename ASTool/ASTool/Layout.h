#pragma once

#include <list>

namespace NSYedaoqLayout
{
	struct LayoutSize;
	struct LayoutPoint;
	struct LayoutOffset;
	struct AnchorInfo;
	struct ResizeInfo;

	enum EnumLayoutDirection
	{
		Direction_Horizon = 0,
		Direction_Vertical,
	};

	enum EnumResizeMode
	{
		Resize_Fixed = 0,
		Resize_Zoomed,
		Resize_Auto,
	};

	enum EnumAnchorMode
	{
		Anchor_Front = 0,
		Anchor_Center,
		Anchor_Back,
	};

	struct ILayout// : ICloneable
	{
		virtual ~ILayout() {}

		virtual ILayout*			Clone() const = 0;
		virtual const ResizeInfo&	GetResizeInfo(EnumLayoutDirection dir) = 0;
		virtual int					Layout(const LayoutPoint& ptBase, const LayoutSize& lyArea) = 0;
	};

	struct ResizeInfo
	{
	public:
		EnumResizeMode	Mode;  // resize mode

		union
		{
			long		Percent;	// Zoom Percent
			long        Fixed;
		};

		ResizeInfo() {}
		ResizeInfo(EnumResizeMode mode, long val)
			: Mode(mode), Fixed(val)
		{}

		static const ResizeInfo FillInfo;
		static const ResizeInfo FixedInfo;
	};

	struct AnchorInfo
	{
		EnumAnchorMode	Mode;
		long			Padding;

		AnchorInfo() {}
		AnchorInfo(EnumAnchorMode mode, long val)
			: Mode(mode), Padding(val)
		{}

		static const AnchorInfo AnchorFront;
		static const AnchorInfo AnchorCenter;
		static const AnchorInfo AnchorBack;
	};

	struct LayoutOffset
	{
		EnumLayoutDirection Direction;
		long				Value;

		LayoutOffset() {};

		LayoutOffset(EnumLayoutDirection dir, long val)
			: Direction(dir), Value(val)
		{}

		operator LayoutSize() const;
		operator LayoutPoint() const;
	};

	struct LayoutSize
	{
		long Width;
		long Height;

		LayoutSize() {}
		LayoutSize(long w, long h)
			: Width(w), Height(h)
		{}

		LayoutSize operator+(const LayoutSize& size) const;
		LayoutSize operator+(const LayoutPoint& pt) const;

		long& LengthAt(EnumLayoutDirection dir);
		long  LengthAt(EnumLayoutDirection dir) const;

		static LayoutSize Invalid;
		static LayoutSize Zero;
	};

	struct LayoutPoint
	{
		long X;
		long Y;

		LayoutPoint() : X(0), Y(0) {}
		LayoutPoint(long x, long y)
			: X(x), Y(y)
		{}

		LayoutPoint operator+(const LayoutSize& size) const;
		LayoutPoint operator+(const LayoutPoint& pt) const;

		long& CoordAt(EnumLayoutDirection dir);
		long  CoordAt(EnumLayoutDirection dir) const;

		static LayoutPoint Zero;
	};

	class CZoomBlankLayout : public ILayout
	{
	public:
		CZoomBlankLayout(long lPercentH = 100, long lPercentV = 100)
			: ResizeInfoH_(NSYedaoqLayout::Resize_Zoomed, lPercentH), ResizeInfoV_(NSYedaoqLayout::Resize_Zoomed, lPercentV)
		{}

		virtual ILayout*			Clone() const { return new CZoomBlankLayout(ResizeInfoH_.Percent, ResizeInfoV_.Percent); }
		virtual const ResizeInfo&	GetResizeInfo(EnumLayoutDirection dir);
		virtual int					Layout(const LayoutPoint& ptBase, const LayoutSize& lyArea) { return 1; };

	protected:
		ResizeInfo		ResizeInfoH_;
		ResizeInfo		ResizeInfoV_;
	};

	class CCtrlLayout : public ILayout
	{
	public:
		virtual ~CCtrlLayout(void) {}

		CCtrlLayout(HWND hWnd)
			: HWnd_(hWnd)
		{}

		CCtrlLayout(HWND hWnd, ResizeInfo resizeInfoH, ResizeInfo resizeInfoV, AnchorInfo anchorInfoH, AnchorInfo anchorInfoV)
			: HWnd_(hWnd), ResizeInfoH_(resizeInfoH), ResizeInfoV_(resizeInfoV), AnchorInfoH_(anchorInfoH), AnchorInfoV_(anchorInfoV)
		{}

		virtual ILayout*			Clone()	const	{ return new CCtrlLayout(HWnd_, ResizeInfoH_, ResizeInfoV_, AnchorInfoH_, AnchorInfoV_); }

		HWND						GetHWnd()		{ return HWnd_; }

		virtual const ResizeInfo&	GetResizeInfo(EnumLayoutDirection dir);
		virtual const AnchorInfo&	GetAnchorInfo(EnumLayoutDirection dir);

		virtual int					Layout(const LayoutPoint& ptBase, const LayoutSize& lyArea);

	protected:

		virtual int			CalLayout(long lArea, const ResizeInfo& resizeInfo, const AnchorInfo& anchorInfo, long& length, long& offset);
		virtual int			UpdateDefaultSize();

	protected:
		HWND				HWnd_;

		ResizeInfo			ResizeInfoH_;
		ResizeInfo			ResizeInfoV_;
		AnchorInfo			AnchorInfoH_;
		AnchorInfo			AnchorInfoV_;

	};

	class CFlowLayout : public ILayout
	{
	public:
		typedef std::list<ILayout*> ItemCollection;

	public:
		~CFlowLayout();

		CFlowLayout(EnumLayoutDirection dir)
			: Direction_(dir), ResizeInfo_(NSYedaoqLayout::Resize_Auto, -1)
		{}

		CFlowLayout(EnumLayoutDirection dir, const ResizeInfo& resizeInfo)
			: Direction_(dir), ResizeInfo_(resizeInfo)
		{}

		CFlowLayout(EnumLayoutDirection dir, ItemCollection items, const ResizeInfo& resizeInfo)
			: Direction_(dir), Items_(items), ResizeInfo_(resizeInfo)
		{}

		virtual ILayout*			Clone() const		{ return new CFlowLayout(Direction_, Items_, ResizeInfo_); }
		EnumLayoutDirection			GetDirection()		{ return Direction_; }

		virtual const ResizeInfo&	GetResizeInfo(EnumLayoutDirection dir);

		virtual int					Layout(const LayoutPoint& ptBase, const LayoutSize& lyArea);

		ILayout*					AddItem(const ILayout& item);

		CCtrlLayout*				AddCtrl(HWND hWnd, 
			ResizeInfo resizeInfoH = ResizeInfo::FixedInfo, 
			ResizeInfo resizeInfoV = ResizeInfo::FixedInfo, 
			AnchorInfo anchorInfoH = AnchorInfo::AnchorFront,
			AnchorInfo anchorInfoV = AnchorInfo::AnchorFront);

		CFlowLayout*				AddFlow(EnumLayoutDirection dir, ResizeInfo resizeInfo = ResizeInfo(NSYedaoqLayout::Resize_Auto, 0));

		CZoomBlankLayout*			AddZoomBlank(long percentH = 100, long percentV = 100);

		void						Clear();
		int							Count() const;

		//ILayout*					operator[](int idx) const;

	protected:
		virtual int					UpdateAutoSize();
		virtual long				GetFixedItemsLength();

	protected:
		EnumLayoutDirection			Direction_;
		ResizeInfo					ResizeInfo_;

		ItemCollection				Items_;
	};

}

