#pragma once

#include "dsp_iir.h"
// {1DC17CA0-0023-4266-AD59-691D566AC291}



class uielem_iir : public CDialogImpl<uielem_iir>, public ui_element_instance {
public:
	uielem_iir(ui_element_config::ptr cfg, ui_element_instance_callback::ptr cb);
	enum { IDD = IDD_IIR_LAYOUT };
	 GUID g_get_guid_iir = guid_iir;
	

	BEGIN_MSG_MAP(uielem_iir)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_IIRENABLED, BN_CLICKED, OnEnabledToggle)
		MSG_WM_HSCROLL(OnScroll)
		COMMAND_HANDLER_EX(IDC_IIRTYPE1, CBN_SELCHANGE, OnChange)
		MESSAGE_HANDLER(WM_USER, OnEditControlChange)
	END_MSG_MAP();



	void initialize_window(HWND parent) { WIN32_OP(Create(parent) != NULL); };
	HWND get_wnd() { return m_hWnd; };
	void set_configuration(ui_element_config::ptr config);
	ui_element_config::ptr get_configuration() { return makeConfig(); };
	static GUID g_get_guid();

	static void g_get_name(pfc::string_base& out);
	static ui_element_config::ptr g_get_default_configuration();
	static const char* g_get_description();

	static GUID g_get_subclass();

	ui_element_min_max_info get_min_max_info();

private:
	LRESULT OnEditControlChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnChange(UINT scrollid, int id, CWindow window);

	void GetEditText();


	void SetIIREnabled(bool state);
	bool IsIIREnabled();

	void IIRDisable();


	void IIREnable(int p_freq, int p_gain, int p_type, float p_qual, bool IIR_enabled);

	void OnEnabledToggle(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnScroll(UINT scrollID, int pos, CWindow window);

	void OnChange2(UINT, int id, CWindow);

	void DSPConfigChange(dsp_chain_config const& cfg);

	//set settings if from another control
	void ApplySettings(GUID inGuid);
	void OnConfigChanged();

	void GetConfig();

	void SetConfig();

	BOOL OnInitDialog(CWindow, LPARAM);

	void RefreshControls(int p_freq, int p_gain, int p_type);

	bool IIR_enabled;
	int p_freq; //40.0, 13000.0 (Frequency: Hz)
	int p_gain; //gain
	int p_type; //filter type
	float p_qual;
	CEditMod pitch_edit;
	CString pitch_s;
	CTrackBarCtrl slider_freq, slider_gain;
	CButton m_buttonIIREnabled;
	bool m_ownIIRUpdate;

	static uint32_t parseConfig(ui_element_config::ptr cfg);

	static ui_element_config::ptr makeConfig(bool init = false);
	uint32_t shit;
protected:
	const ui_element_instance_callback::ptr m_callback;
};
class myElem_t_iir : public  ui_element_impl_withpopup< uielem_iir > {
	bool get_element_group(pfc::string_base & p_out)
	{
		p_out = "Effect DSP";
		return true;
	}

	bool get_menu_command_description(pfc::string_base & out) {
		out = "Opens a window for IIR filtering control.";
		return true;
	}

};
static service_factory_single_t<myElem_t_iir> g_myElemFactory;

