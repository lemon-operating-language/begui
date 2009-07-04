#pragma once

#include "common.h"

class ChildWnd1 : public Window
{
private:
	Button	m_button1, m_button2, m_button3, m_button4, m_button5;
	RadioButton m_radio1;
	RadioButton m_radio2;
	RadioButton m_radio3;
	Group m_group;
	Slider m_slider;
	TextBox m_textbox;
	ListBox m_listbox1, m_listbox2, m_listbox3, m_listbox4;
	ComboBox m_combobox1, m_combobox2, m_combobox3;
	TabContainer m_tabs;
	CheckBox	m_checkbox1, m_checkbox2;
	ViewportComponent m_vp;
	Label m_label1;

	// example: a live variable
	double m_sliderPos;

public:
	ChildWnd1() : m_sliderPos(0) { }
	void onCreate();
};

class Application : public BaseApp_Win
{
private:
	ChildWnd1	m_wnd1;
	Dialog		m_dlg1;
	Button		m_dlgBtn1;

	static Application	*m_pInst;

public:
	virtual bool onCreate();
	void onMenu(int id);
	void onRenderVp();

	static Application* inst()	{ if (!m_pInst) m_pInst = new Application(); return m_pInst; }
};