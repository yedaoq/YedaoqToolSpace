#include <Windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#include <Strsafe.h>
#include <atlconv.h>
#include <atlstr.h>

#pragma comment(lib, "gdiplus.lib")

// interface
bool Png2AlphaBitmap( LPCTSTR path_png, LPCTSTR path_bmp );

// impl
class GDIPlusInitialize
{
	ULONG_PTR     gdiplusToken_;
public:

	GDIPlusInitialize();
	~GDIPlusInitialize();
};

GDIPlusInitialize::GDIPlusInitialize()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken_, &gdiplusStartupInput, NULL);
}

GDIPlusInitialize::~GDIPlusInitialize()
{
	if(0 != gdiplusToken_)
		Gdiplus::GdiplusShutdown(gdiplusToken_);
}

CLSID FindGdiplusEncoderClsidByExt( LPCTSTR lpszExtension, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs )
{
	ATL::CT2CW pszExtensionW( lpszExtension );

	for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
	{
		ATL::CStringW strExtensions( pCodecs[iCodec].FilenameExtension );

		int iStart = 0;
		do
		{
			ATL::CStringW strExtension = ::PathFindExtensionW( strExtensions.Tokenize( L";", iStart ) );
			if( iStart != -1 )
			{
				if( strExtension.CompareNoCase( pszExtensionW ) == 0 )
				{
					return( pCodecs[iCodec].Clsid );
				}
			}
		} while( iStart != -1 );
	}

	return( CLSID_NULL );
}

CLSID FindGdiplusEncoderClsidByFormat(REFGUID guidFileType, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs )
{
	for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
	{
		if( pCodecs[iCodec].FormatID == guidFileType )
		{
			return( pCodecs[iCodec].Clsid );
		}
	}

	return( CLSID_NULL );
}


CLSID GetGdiplusEncoderClsid(const WCHAR* lpszFormatName,const CLSID* guidFormatType)
{
	UINT  encoder_num = 0;				// number of image encoders
	UINT  encoder_info_size = 0;        // size of the image encoder array in bytes
	CLSID clsid = CLSID_NULL;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&encoder_num, &encoder_info_size);
	if(encoder_info_size == 0)
		return clsid;  

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(encoder_info_size));
	if(pImageCodecInfo == NULL)
		return clsid; 

	if( Gdiplus::Ok == GetImageEncoders(encoder_num, encoder_info_size, pImageCodecInfo))
	{
		if(lpszFormatName)
		{
			clsid = FindGdiplusEncoderClsidByExt(lpszFormatName, pImageCodecInfo, encoder_num);
		}
		else if(guidFormatType)
		{
			clsid = FindGdiplusEncoderClsidByFormat(*guidFormatType, pImageCodecInfo, encoder_num);
		}
	}

	free(pImageCodecInfo);
	return clsid;
}

bool Png2AlphaBitmap( LPCTSTR path_png, LPCTSTR path_bmp )
{
	static GDIPlusInitialize gdpi;
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(path_png, FALSE);

	//bitmap->

	CLSID clsid = GetGdiplusEncoderClsid(NULL, &Gdiplus::ImageFormatBMP);
	Gdiplus::Status state = bitmap->Save(path_bmp, &clsid, NULL);
	return Gdiplus::Ok == state;
}
