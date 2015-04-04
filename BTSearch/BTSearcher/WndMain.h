#pragma once

class WndMain
{
public:
	WndMain(void);
	~WndMain(void);

	void Show();

protected:
	static INT_PTR CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);

	void OnCommand(WPARAM wParam, LPARAM lParam);

	void OnOK();
	void OnCancel();

protected:
	HWND	wnd_;
};
