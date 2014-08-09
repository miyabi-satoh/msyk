///////////////////////////////////////////////////////////////////////////////
/// @file	app.cpp
/// @date	2014/07/26
/// @author	Masayuki
/// @brief	アプリケーションクラス
///////////////////////////////////////////////////////////////////////////////

#include "common.hpp"
#include "app.hpp"
#include "window.hpp"

int mxApp::run()
{
	int nRet = -1;

	::InitCommonControls();

	try {
		this->initialize();

		MSG msg;
		while (m_pMainWnd) {
			if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
				if (::GetMessage(&msg, NULL, 0, 0) <= 0) {
					nRet = msg.wParam;
					break;
				}
				if (!this->preTranslateMessage(&msg)) {
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			else {
				this->doIdle();
			}
		}

		this->finalize();
	}
	catch (const mxException &e) {
		getMainWnd()->messageBox(e.what());
	}

	return nRet;
}
