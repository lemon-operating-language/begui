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

#include "Button.h"
#include "Font.h"

using namespace begui;

Button::Button() : 
	m_id(-1),
	m_status(Button::UP),
	m_iconSzX(0),
	m_iconSzY(0),
	m_bAutoSzX(true),
	m_bAutoSzY(true),
	m_iconPlacement(NEAR_LEFT),
	m_activeArea(0,0,0,0),
	m_resizableArea(0,0,0,0)
{
}

void Button::create(int x, int y, const std::string &title, int id, Functor1<int> &callback, 
				const std::string &style_name)
{
	create(x,y, 0,0, title, id, callback, style_name);
}

void Button::create(int x, int y, int w, int h, const std::string &title, int id, Functor1<int> &callback, 
				const std::string &style_name)
{
	m_title = title;
	m_onClick = callback;
	m_id = id;
	m_status = Button::UP;

	m_left = x;
	m_top = y;

	// load the rest of the properties from the property manager
	ResourceManager::Style style = ResourceManager::inst()->getClassDef("Button").style(style_name);
	setFace(UP, ResourceManager::inst()->loadImage(style.get_img("face_up")));
	if (style.hasProp("face_hover"))
		setFace(MOUSE_OVER, ResourceManager::inst()->loadImage(style.get_img("face_hover")));
	if (style.hasProp("face_down"))
		setFace(DOWN, ResourceManager::inst()->loadImage(style.get_img("face_down")));
	ASSERT(style.hasProp("resizable_area"));
	setResizableArea(style.get_rect("resizable_area"));
	
	// get the button's active area
	if (style.hasProp("active_area"))
		m_activeArea = style.get_rect("active_area");
	else
		m_activeArea = Rect<int>(0,0,m_faces[UP].m_width, m_faces[UP].m_height);

	Rect<int> borders = getActiveBorders();
	if (w > 0) {
		m_bAutoSzX = false;
		m_right = x + w;
	}
	else {
		if (style.hasProp("default_width"))
			m_right = x+style.get_i("default_width")-borders.left-borders.right;
		else if (title.length() > 0)
		{
			m_bAutoSzX = true;
			int cw = 40 + Font::stringLength(title) + m_iconSzX;
			if (cw < 80)
				cw = 80;
			m_right = x + cw;
		}
		else {
			if (m_activeArea.getWidth() > 0)
				m_right = x+m_activeArea.getWidth();
			else
				m_right = x+m_faces[UP].m_width;
		}
	}

	if (h > 0) {
		m_bAutoSzX = false;
		m_bottom = y+h;
	}
	else {
		if (style.hasProp("default_height"))
		{
			m_bAutoSzY = true;
			int default_height = style.get_i("default_height")-borders.top-borders.bottom;
			m_bottom = y+default_height;
		}
		else {
			if (m_activeArea.getHeight() > 0)
				m_bottom = y+m_activeArea.getHeight();
			else
				m_bottom = y+m_faces[UP].m_height;
		}
	}
}

void Button::onUpdate()
{
}

void Button::onRender()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	int w = getWidth();
	int h = getHeight();

	// set the texture of a window
	ResourceManager::ImageRef btn_face;
	ASSERT(m_faces[Button::UP].m_texture);
	
	if (m_faces[m_status].m_texture)
		btn_face = m_faces[m_status];
	else if (m_status == DOWN && m_faces[Button::MOUSE_OVER].m_texture)
		btn_face = m_faces[Button::MOUSE_OVER];
	else
		btn_face = m_faces[Button::UP];

	btn_face.m_texture->set();
	//w = btn_face.m_width;
	//h = btn_face.m_height;

	float offs = 0.0f;
	if (m_status == Button::INACTIVE)
		glColor4f(1,1,1, 0.5);
	else if (m_status == Button::MOUSE_OVER && !m_faces[m_status].m_texture)
		glColor4f(1,1,1,0.8f);
	else
		glColor4f(1,1,1,1);
	if (m_status == Button::DOWN && !m_faces[m_status].m_texture)
		offs = 1.0f;

	Rect<int> actBorders = getActiveBorders();
	Component::drawImageWtBorders(btn_face, 
							(int)(offs-actBorders.left),
							(int)(offs-actBorders.top),
							(int)(w-2*offs + actBorders.left + actBorders.right),
							(int)(h-2*offs + actBorders.top + actBorders.bottom),
							m_resizableArea);
	
	int centerx = w/2-m_activeArea.left;
	int centery = h/2-m_activeArea.top;
	int title_w = Font::stringLength(m_title);

	// if there is an icon, render the icon
	int iw = m_iconSzX;
	int ih = m_iconSzY;
	if (m_icon.m_texture)
	{
		int ix = centerx-iw/2, iy = centery-ih/2;

		// if there is text, the icon should be on its left
		if (title_w > 0)
		{
			ix -= title_w/2 + 1;
		}
		
		// render the icon
		Component::drawImage(m_icon, ix, iy, iw, ih);
	}
	
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	if (m_status == Button::INACTIVE)
		glColor3f(0.3f, 0.3f, 0.3f);
	else
		glColor3f(1,1,1);
	Font::renderString(centerx - title_w/2 + iw/2, centery+4, m_title);
}

bool Button::onMouseDown(int x, int y, int button)
{
	if (m_status != Button::INACTIVE) {
		m_status = Button::DOWN;
	}
	return true;
}

bool Button::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (m_status != Button::DOWN || !isPtInside(x,y)) {
		if (m_status != Button::INACTIVE && isPtInside(x,y))
			m_status = Button::MOUSE_OVER;
		else
			m_status = Button::UP;
	}
	return true;
}

bool Button::onMouseUp(int x, int y, int button)
{
	if (m_status != Button::INACTIVE) {
		if (m_status == Button::DOWN)
			m_onClick(m_id);
		if (isPtInside(x,y))
			m_status = Button::MOUSE_OVER;
		else
			m_status = Button::UP;
	}
	return true;
}

void Button::onKeyDown(int key)
{
}

void Button::onKeyUp(int key)
{
}

void Button::setIcon(const ResourceManager::ImageRef &icon, IconPlacement place, int x_sz, int y_sz)
{
	m_icon = icon;
	if (x_sz == 0)
		x_sz = icon.m_width;
	if (y_sz == 0)
		y_sz = icon.m_height;

	if (m_bAutoSzX) {
		m_right = m_right - m_iconSzX + x_sz;
	}

	m_iconSzX = x_sz;
	m_iconSzY = y_sz;
	m_iconPlacement = place;
}

void Button::setFace(State state, const ResourceManager::ImageRef &img)
{
	m_faces[(size_t)state] = img;

	// no resizable area known about this face
	m_resizableArea = Rect<int>(0,0,0,0);
}

void Button::setResizableArea(const Rect<int> &resizable_area)
{
	m_resizableArea = resizable_area;
}

/*Rect<int> Button::getActiveArea() const
{
	return Rect<int>(getLeft(), getTop(),
		getRight() - (m_faces[UP].m_width-m_activeArea.right) - m_activeArea.left,
		getBottom() - (m_faces[UP].m_height-m_activeArea.bottom) - m_activeArea.top);
}*/

bool Button::isPtInside(int x, int y)
{
//	return getActiveArea().contains(x,y);
	return Component::isPtInside(x,y);
}