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
#include "Button.h"

namespace begui {

class ScrollBar : public Container
{
public:
	enum ScrollDir {
		SCROLL_HORIZONTAL,
		SCROLL_VERTICAL
	};

private:
	ScrollDir	m_scrollDir;
	double		m_minPos, m_maxPos;
	double		m_curPos;
	double		m_percVisible;	// the percentage of content that is currently visible (set to 0 to disable)

	int		m_sliderDragStart;
	double	m_sliderDragStartVal;

	int		m_sliderLen;

	const static int STEPS_PER_PAGE = 4;

	int		m_barSize;	// the width/height of the bar for a vertical/horizontal bar correspondingly
	int		m_buttonWidth, m_buttonHeight;	// the dimensions of the arrow buttons of the scrollbar
	int		m_sliderWidth;	// the size of the handle

	Button	m_incBtn, m_decBtn, m_slider;
	int		m_sliderOffs1, m_sliderOffs2;
	ResourceManager::ImageRef m_barBg;

public:
	ScrollBar();
	virtual ~ScrollBar();

	virtual void	create(int x, int y, int length, ScrollDir dir, double minPos=0, double maxPos=1,
							const std::string &style_name="std");
			void	setScrollPos(double pos);
			double	getScrollPos() const				{ return m_curPos; }
			void	setPercVisible(double perc)	{ ASSERT(perc >= 0 && perc <= 1.0); m_percVisible = perc; }
			void	setBounds(double minPos, double maxPos, double percVisible);

	virtual void onUpdate();
	virtual void onRender();	
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

private:
	void handleClick(int id);
};

};