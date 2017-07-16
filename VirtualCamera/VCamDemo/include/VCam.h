//VCam.h
//VCamSDK所有导出接口的头文件
//VCamSDK Version 1.0
//使用中如遇到问题,请联系avmedia2010@gmail.com
#if !defined(AVMEDIA2010_VCAM_SDK_DLL)
#define AVMEDIA2010_VCAM_SDK_DLL

#ifndef VCAM_DLLAPI
#define VCAM_DLLAPI extern "C" __declspec(dllimport)
#endif

VCAM_DLLAPI	int VCam_Initialize();
VCAM_DLLAPI	int VCam_UnInitialize();
VCAM_DLLAPI	int VCam_OutPutFrame(BYTE *pFrame);//输出图像到虚拟视频

//接口调用成功返回0，失败返回-1。
#endif//AVMEDIA2010_VCAM_SDK_DLL
