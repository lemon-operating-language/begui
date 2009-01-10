#pragma once

#include "common.h"

class ChildWnd1 : public Window
{
private:
	Button	m_button1;
	RadioButton m_radio1;
	RadioButton m_radio2;
	Group m_group;
	Slider m_slider;
	TextBox m_textbox;

	// example: a live variable
	double m_sliderPos;

public:
	ChildWnd1() : m_sliderPos(0) { }
	void onCreate();
};

class Application : public BaseApp_Win
{
private:
	ChildWnd1 m_wnd1;

	static Application	*m_pInst;

public:
	bool initialize();
	void onMenu(int id);

	static Application* inst()	{ if (!m_pInst) m_pInst = new Application(); return m_pInst; }

private:
	static void onMenuCB(int id);
};