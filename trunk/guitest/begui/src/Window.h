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
#include "ResourceManager.h"
#include "Button.h"

namespace begui {
	
class Menu;

class Window : public Container
{
public:
	enum Style {
		MULTIPLE,				// a frame window with multiple childern
	//	MULTIPLE_TRANSPARENT,	// transparent main window (mac-style)
		SINGLE,					// a frame window containing controls only
		TOOLBOX
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

protected:
	std::string		m_title;
	Style			m_style;	// the style of the frame window (single/multiple doc)
	Menu			*m_pMenu;
	bool			m_bResizable;
	bool			m_bMovable;
	bool			m_bHasBorders;
	bool			m_bHasCaption;

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

	Container	m_contents;
	
	// window current state
	State			m_state;
	bool			m_bResizing;
	bool			m_bMoving;

public:
	Window();
	virtual ~Window();

	virtual void create(int left, int top, int width, int height, const std::string &title, 
						const std::string &style_name = "std");
	virtual void addComponent(Component* pC)	{ m_contents.addComponent(pC); }

	virtual void setMovable(bool bMovable)		{ m_bMovable = bMovable; }
	virtual void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
	virtual void setStyle(Style style)			{ m_style = style; }
	virtual void setClientAreaSize(int w, int h);
	
	virtual void onCreate() { };
	virtual void frameUpdate();
	virtual void frameRender();
	virtual void onRender();

	virtual void minimize();
	virtual void maximize();
	virtual void restore();

	virtual bool onMouseDown(int x, int y, int button);
	virtual bool onMouseMove(int x, int y, int prevx, int prevy);
	virtual bool onMouseUp(int x, int y, int button);
	virtual bool isPtInside(int x, int y);

	//
	virtual Menu*		createMainMenu();
	inline Menu&		getMenu()				{ return *m_pMenu; }
	inline Rect<int>	getClientArea() const	{ return m_clientArea; }
	inline Style		getStyle() const		{ return m_style; }
	inline State		getState() const		{ return m_state; }

protected:
	virtual void	onCaptionBtn(int id);

	virtual void	calcClientArea();
};

};