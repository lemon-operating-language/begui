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
	m_reszLeft(0),
	m_reszRight(0),
	m_reszTop(0),
	m_reszBottom(0),
	m_iconPlacement(NEAR_LEFT)
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
	int rL = 0, rR = 0, rT = 0, rB = 0;
	if (style.hasProp("reszLeft") && style.hasProp("reszRight")) {
		rL = style.get_i("reszLeft");
		rR = style.get_i("reszRight");
	}
	if (style.hasProp("reszTop") && style.hasProp("reszBottom")) {
		rT = style.get_i("reszTop");
		rB = style.get_i("reszBottom");
	}
	setResizableArea(rL, rT, rR, rB);

	if (w > 0) {
		m_bAutoSzX = false;
		m_right = x + w;
	}
	else {
		if (style.hasProp("default_width"))
			m_right = x+style.get_i("default_width");
		else
		{
			m_bAutoSzX = true;
			int cw = 40 + Font::stringLength(title) + m_iconSzX;
			if (cw < 80)
				cw = 80;
			m_right = x + cw;
		}
	}

	if (h > 0) {
		m_bAutoSzX = false;
		m_bottom = y+h;
	}
	else {
		m_bAutoSzY = true;
		int default_height = style.get_i("default_height");
		m_bottom = y+default_height;
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

	if (m_reszLeft != m_reszRight || m_reszTop != m_reszBottom)
	{
		float rL = btn_face.m_topLeft.x + (float)m_reszLeft / btn_face.m_texture->getWidth();
		float rR = btn_face.m_topLeft.x + (float)m_reszRight / btn_face.m_texture->getWidth();
		float rT = btn_face.m_topLeft.y + (float)m_reszTop / btn_face.m_texture->getHeight();
		float rB = btn_face.m_topLeft.y + (float)m_reszBottom / btn_face.m_texture->getHeight();
		Component::drawBorderedQuad(offs, offs, w-offs, h-offs,
								m_reszLeft, m_reszTop,
								w-(btn_face.m_width - m_reszRight),
								h-(btn_face.m_height - m_reszBottom),
								btn_face.m_topLeft.x, rL,
								btn_face.m_bottomRight.x, rR,
								btn_face.m_topLeft.y, rT,
								btn_face.m_bottomRight.y, rB);
	}
	else
	{
		glBegin(GL_QUADS);
			glTexCoord2f(btn_face.m_topLeft.x, btn_face.m_topLeft.y);
			glVertex3f(offs, offs, 0);
			glTexCoord2f(btn_face.m_bottomRight.x, btn_face.m_topLeft.y);
			glVertex3f(w-offs, offs, 0);
			glTexCoord2f(btn_face.m_bottomRight.x, btn_face.m_bottomRight.y);
			glVertex3f(w-offs, h-offs, 0);
			glTexCoord2f(btn_face.m_topLeft.x, btn_face.m_bottomRight.y);
			glVertex3f(offs, h-offs, 0);
		glEnd();
	}
	
	int centerx = w/2;
	int centery = h/2;
	int title_w = Font::stringLength(m_title);

	// if there is an icon, render the icon
	int iw = m_iconSzX;
	int ih = m_iconSzY;
	if (m_icon.m_texture) {
		m_icon.m_texture->set();

		int ix = centerx-iw/2, iy = centery-ih/2;

		// if there is text, the icon should be on its left
		if (title_w > 0)
		{
			ix -= title_w/2 + 1;
		}
		
		glBegin(GL_QUADS);
			glTexCoord2f(m_icon.m_topLeft.x, m_icon.m_topLeft.y);
			glVertex2f(ix, iy);
			glTexCoord2f(m_icon.m_bottomRight.x, m_icon.m_topLeft.y);
			glVertex2f(ix+iw, iy);
			glTexCoord2f(m_icon.m_bottomRight.x, m_icon.m_bottomRight.y);
			glVertex2f(ix+iw, iy+ih);
			glTexCoord2f(m_icon.m_topLeft.x, m_icon.m_bottomRight.y);
			glVertex2f(ix, iy+ih);
		glEnd();
	}
	
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	if (m_status == Button::INACTIVE)
		glColor3f(0.3, 0.3, 0.3);
	else
		glColor3f(1,1,1);
	Font::renderString(centerx - title_w/2 + iw/2, h/2+4, m_title);
}

void Button::onMouseDown(int x, int y, int button)
{
	if (m_status != Button::INACTIVE) {
		m_status = Button::DOWN;
	}
}

void Button::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (m_status != Button::INACTIVE && isPtInside(x,y))
		m_status = Button::MOUSE_OVER;
	else
		m_status = Button::UP;
}

void Button::onMouseUp(int x, int y, int button)
{
	if (m_status != Button::INACTIVE) {
		if (m_status == Button::DOWN)
			m_onClick(m_id);
		if (isPtInside(x,y))
			m_status = Button::MOUSE_OVER;
		else
			m_status = Button::UP;
	}
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
	m_reszLeft = m_reszRight = m_reszTop = m_reszBottom = 0;
}

void Button::setResizableArea(int left, int top, int right, int bottom)
{
	m_reszLeft = left;
	m_reszRight = right;
	m_reszTop = top;
	m_reszBottom = bottom;
}