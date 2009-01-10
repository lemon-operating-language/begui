/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of BeGUI library.
//
//    BeGUI is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    BeGUI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with BeGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "common.h"

namespace begui {

class BaseApp_Win
{
private:
	static BaseApp_Win	*m_pInst;

public:
	int run(const std::string &title, size_t width, size_t height);

	static BaseApp_Win* inst()	{ if (!m_pInst) m_pInst = new BaseApp_Win(); return m_pInst; }

	LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	BaseApp_Win() { };
	virtual ~BaseApp_Win();

	bool coreInitialize();
	bool createGLWindow(const char* title, int width, int height, int bits, bool fullscreenflag);
	void killGLWindow();
	
	virtual bool initialize();
	virtual void resize(int width, int height);
	virtual void updateFrame();
	virtual void renderFrame();
};

};