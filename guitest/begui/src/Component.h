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
#include "timeseries.h"

namespace begui {

class Component
{
protected:
	TimeSeries<int>	m_left, m_right, m_top, m_bottom;
	Component	*m_pParent;

	bool	m_bAlwaysOnTop;
	bool	m_bVisible;
	bool	m_bHasMouseFocus;
	bool	m_bHasKeybFocus;
	bool	m_bActive;
	bool	m_bEnabled;
	bool	m_bFixedZOrder;

public:
	Component();

	int getLeft() const		{ return m_left; }
	int getTop() const		{ return m_top; }
	int getRight() const	{ return m_right; }
	int getBottom() const	{ return m_bottom; }
	int getWidth() const	{ return m_right - m_left; }
	int getHeight() const	{ return m_bottom - m_top; }

	void setPos(int x, int y)	{ m_right=x+getWidth(); m_bottom=y+getHeight(); m_left=x; m_top=y; }
	void setSize(int w, int h)	{ m_right = m_left+w; m_bottom = m_top + h; }
	void setFixedZOrder(bool fixed)		{ m_bFixedZOrder = fixed; }

	bool hasFixedZOrder() const	{ return m_bFixedZOrder; }
	bool isAlwaysOnTop() const	{ return m_bAlwaysOnTop; }
	bool isVisible() const		{ return m_bVisible; }
	bool isActive() const		{ return m_bActive; }
	bool isEnabled() const		{ return m_bEnabled; }

	bool hasMouseFocus() const	{ return m_bHasMouseFocus; }
	bool hasKeybFocus() const	{ return m_bHasKeybFocus; }
	void getFocus();
	void releaseFocus();
	void getMouseFocus();
	void getKeybFocus();
	void releaseMouseFocus();
	void releaseKeybFocus();

	void setParent(Component *pParent)	{ m_pParent = pParent; }
	void setAlwaysOnTop(bool ontop)	{ m_bAlwaysOnTop = ontop; }
	void setVisible(bool visible)	{ m_bVisible = visible; }

	virtual void frameUpdate() = 0;
	virtual void frameRender() = 0;
	
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);
	virtual void onActivate() { m_bActive = true; };
	virtual void onDeactivate() { m_bActive = false; };

	virtual bool isPtInside(int x, int y);

	// Coordinate transformation
	Vector2	worldToLocal(const Vector2& v) const;
	Vector2 parentToLocal(const Vector2& v) const;
	Vector2 localToWorld(const Vector2& v) const;
	Vector2 localToParent(const Vector2& v) const;

	//helpers
	static void drawBorderedQuad(int left, int top, int right, int bottom,		// position to appear on screen
						int lB, int tB, int rB, int bB,							// border positions
						double texL, double texLB, double texR, double texRB,	// left to right positions in tex
						double texT, double texTB, double texB, double texBB);	// top to bottom
};

};