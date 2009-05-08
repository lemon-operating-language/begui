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
#include "Button.h"

namespace begui {

class FrameWindow : public Container
{
public:
	enum Style {
		MULTIPLE,				// a frame window with multiple childern
	//	MULTIPLE_TRANSPARENT,	// transparent main window (mac-style)
		SINGLE					// a frame window containing controls only
	};
	enum State {
		VISIBLE,
		MINIMIZED,
		MAXIMIZED,
		FULLSCREEN
	};
	enum Event {
		MINIMIZE,
		MAXIMIZE,
		RESTORE,
		CLOSE
	};
	class InternalContainer : public Container {
	public:
		virtual void onRender() { };
	};

private:
	std::string				m_title;
	Menu					*m_pMenu;
	Dialog					*m_pModalDialog;
	Style					m_style;	// the style of the frame window (single/multiple doc)
	bool					m_bOwnDraw;	// true if the window has to draw and handle its own caption and borders
	State					m_state;

	static FrameWindow	*m_pInstance;
	FrameWindow();

	ResourceManager::ImageRef	m_windowFace, m_captionFace;
	Rect<int>	m_windowActiveArea, m_captionActiveArea;
	Rect<int>	m_windowResizableArea, m_captionResizableArea;
	Button		m_closeBtn, m_minBtn, m_maxBtn;
	int			m_captionTextYPos;
	int			m_captionBarWidth;
	Color		m_captionTextColor;
	int			m_captionTextPadLeft;
	Rect<int>	m_clientArea;
	Rect<int>	m_borderSize;

	InternalContainer	m_container;

	// event hooks
	Functor1<FrameWindow::Event>	m_onMinimize, m_onMaximize, m_onRestore, m_onClose;

public:
	static FrameWindow* inst()	{ if (!m_pInstance) m_pInstance = new FrameWindow(); return m_pInstance; }
	virtual ~FrameWindow();

	bool create(int width, int height, const std::string &title, Style style=MULTIPLE, bool ownDraw=true);
	void resize(int width, int height);
	void resetViewport();

	void showModalDialog(Dialog* dlg);
	void closeModalDialog();

	// redirect container methods to the dummy container
	virtual void addComponent(Component *pC)	{ m_container.addComponent(pC); }
	virtual void remComponent(Component *pC)	{ m_container.remComponent(pC); }
	virtual int			getChildrenNum() const	{ return m_container.getChildrenNum(); }
	virtual Component*	getChild(int pos)		{ return m_container.getChild(pos); }

	virtual void frameUpdate();
	virtual void frameRender();
	virtual void onRender();

	inline int			getWidth() const		{ return m_right - m_left; }
	inline int			getHeight() const		{ return m_bottom - m_top; }
	inline Menu&		getMenu()				{ return *m_pMenu; }
	inline Rect<int>	getClientArea() const	{ return m_clientArea; }

	virtual bool isPtInside(int x, int y) { return true; }
	
	// Change access to main event handlers to public
	virtual bool onMouseDown(int x, int y, int button);
	virtual bool onMouseMove(int x, int y, int prevx, int prevy);
	virtual bool onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	// handle event hooks
	virtual void setEventHook(Event evt, const Functor1<Event> &fun);

	virtual void onCaptionBtn(int id);

protected:
	void calcClientArea();
};

};