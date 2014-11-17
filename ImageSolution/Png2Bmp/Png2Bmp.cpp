// Png2Bmp.cpp : 定义控制台应用程序的入口点。
//

#include <Windows.h>
#include <atlstr.h>
#include <GdiPlus.h>
#include <Strsafe.h>

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
	CT2CW pszExtensionW( lpszExtension );

	for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
	{
		CStringW strExtensions( pCodecs[iCodec].FilenameExtension );

		int iStart = 0;
		do
		{
			CStringW strExtension = ::PathFindExtensionW( strExtensions.Tokenize( L";", iStart ) );
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

int _tmain(int argc, _TCHAR* argv[])
{
	GDIPlusInitialize gdpi;

	if (argc < 2)
	{
		return 1;
	}

	TCHAR	file_path_buf[MAX_PATH];
	LPCTSTR	file_out = file_path_buf;
	if(argc < 3)
	{
		::StringCbCopy(file_path_buf, sizeof(file_path_buf), argv[1]);
		::PathRemoveExtension(file_path_buf);
		::PathAddExtension(file_path_buf, TEXT(".bmp"));
	}
	else
	{
		file_out = argv[2];
	}

	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(argv[1], FALSE);

	//Gdiplus::Bitmap* bitmap1 = new Gdiplus::Bitmap()

	CLSID clsid = GetGdiplusEncoderClsid(NULL, &Gdiplus::ImageFormatBMP);
	Gdiplus::Status state = bitmap->Save(file_out, &clsid, NULL);
	return Gdiplus::Ok == state;
}

