//VideoCaptureModuleDll

#if !defined(MJH_VIDEO_SOURCE_CAPTURE_DLL)
#define MJH_VIDEO_SOURCE_CAPTURE_DLL

#ifndef VIDEO_SOURCE_CAPTURE_DLLAPI
#define VIDEO_SOURCE_CAPTURE_DLLAPI extern "C" __declspec(dllimport)
#endif

VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_Initialize();
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_UnInitialize();
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_GetCameraCount(int &nCameraCount);
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_GetCameraName(WCHAR * strSourceName);
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_SetSelectedCamera(int &nNumber);
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_Open();
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_Close();
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_Play();
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_Stop();
VIDEO_SOURCE_CAPTURE_DLLAPI int VideoCaptureModule_SetFrameSize(int nWidth, int nHeight);//设置视频帧大小
VIDEO_SOURCE_CAPTURE_DLLAPI int VideoCaptureModule_GetFrameSize(int &nWidth, int &nHeight);//得到视频帧大小
VIDEO_SOURCE_CAPTURE_DLLAPI	int VideoCaptureModule_GetFrame(BYTE *pFrame, int nWidth, int nHeight);//得到当前视频帧

//接口调用成功返回0，失败返回-1。
#endif//MJH_VIDEO_SOURCE_CAPTURE_DLL
