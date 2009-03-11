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
#include "Container.h"
#include "Window.h"
#include "Menu.h"
#include "Dialog.h"

namespace begui {

class FrameWindow : public Container
{
private:
	Menu					*m_pMenu;
	Dialog					*m_pModalDialog;
	bool					m_bShowBackground;

	static FrameWindow	*m_pInstance;
	FrameWindow();
public:
	static FrameWindow* inst()	{ if (!m_pInstance) m_pInstance = new FrameWindow(); return m_pInstance; }
	virtual ~FrameWindow();

	bool create(int width, int height);
	void resize(int width, int height);
	void resetViewport();
	void showBackground(bool bShow)		{ m_bShowBackground = bShow; }

	void showModalDialog(Dialog* dlg);
	void closeModalDialog();

	virtual void frameUpdate() { Container::frameUpdate(); }
	virtual void frameRender();
	virtual void onRender();

	inline int	getWidth() const	{ return m_right - m_left; }
	inline int	getHeight() const	{ return m_bottom - m_top; }
	inline Menu& getMenu()			{ return *m_pMenu; }

	virtual void onMouseDownEx(int x, int y);
	virtual void onMouseMoveEx(int x, int y, int prevx, int prevy);
	virtual void onMouseUpEx(int x, int y);
	virtual bool isPtInside(int x, int y) { return true; }
	
	// Change access to main event handlers to public
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);
};

};