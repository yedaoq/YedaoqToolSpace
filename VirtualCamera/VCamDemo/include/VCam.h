//VCam.h
//VCamSDK���е����ӿڵ�ͷ�ļ�
//VCamSDK Version 1.0
//ʹ��������������,����ϵavmedia2010@gmail.com
#if !defined(AVMEDIA2010_VCAM_SDK_DLL)
#define AVMEDIA2010_VCAM_SDK_DLL

#ifndef VCAM_DLLAPI
#define VCAM_DLLAPI extern "C" __declspec(dllimport)
#endif

VCAM_DLLAPI	int VCam_Initialize();
VCAM_DLLAPI	int VCam_UnInitialize();
VCAM_DLLAPI	int VCam_OutPutFrame(BYTE *pFrame);//���ͼ��������Ƶ

//�ӿڵ��óɹ�����0��ʧ�ܷ���-1��
#endif//AVMEDIA2010_VCAM_SDK_DLL
