#include "uielem_iir.h"

uielem_iir::uielem_iir(ui_element_config::ptr cfg, ui_element_instance_callback::ptr cb) : m_callback(cb) {
	p_freq = dsp_iir_default::Freq;
	p_gain = dsp_iir_default::Gain;
	p_type = HSH;
	p_qual = dsp_iir_default::Qual;
	IIR_enabled = dsp_iir_default::Enabled;

};


void uielem_iir::set_configuration(ui_element_config::ptr config) {
	shit = parseConfig(config);
	if (m_hWnd != NULL) {
		uielem_iir::ApplySettings(g_get_guid_iir);
	}
	m_callback->on_min_max_info_change();
};
GUID uielem_iir::g_get_guid() {
	return guid_iirelem;
}
void uielem_iir::g_get_name(pfc::string_base& out) { out = "IIR Filter UI"; }
ui_element_config::ptr uielem_iir::g_get_default_configuration() {
	return uielem_iir::makeConfig(true);
}
const char* uielem_iir::g_get_description() { return "Modifies the 'IIR Filter' DSP effect."; }
GUID uielem_iir::g_get_subclass() {
	return ui_element_subclass_dsp;
}

ui_element_min_max_info uielem_iir::get_min_max_info() {
	ui_element_min_max_info ret;

	// Note that we play nicely with separate horizontal & vertical DPI.
	// Such configurations have not been ever seen in circulation, but nothing stops us from supporting such.
	CSize DPI = QueryScreenDPIEx(*this);

	if (DPI.cx <= 0 || DPI.cy <= 0) { // sanity
		DPI = CSize(96, 96);
	}


	ret.m_min_width = MulDiv(420, DPI.cx, 96);
	ret.m_min_height = MulDiv(140, DPI.cy, 96);
	ret.m_max_width = 10000;
	ret.m_max_height = 10000;

	// Deal with WS_EX_STATICEDGE and alike that we might have picked from host
	ret.adjustForWindow(*this);

	return ret;
}


LRESULT uielem_iir::OnEditControlChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == 0x1988)
	{
		uielem_iir::GetEditText();
	}
	return 0;
}

void uielem_iir::OnChange(UINT scrollid, int id, CWindow window)
{
	CWindow w;
	p_freq = slider_freq.GetPos();
	p_gain = slider_gain.GetPos();
	p_type = SendDlgItemMessage(IDC_IIRTYPE1, CB_GETCURSEL);
	if (IsIIREnabled())
	{
		dsp_preset_impl preset;
		dsp_iir_default::make_preset(p_freq, p_gain, p_type, p_qual, true, g_get_guid(),preset);
		static_api_ptr_t<dsp_config_manager>()->core_enable_dsp(preset, dsp_config_manager::default_insert_last);

	}
	RefreshControls(p_freq, p_gain, p_type);

}



void uielem_iir::GetEditText()
{
	CString sWindowText;
	pitch_edit.GetWindowText(sWindowText);
	float pitch2 = _ttof(sWindowText);
	if (pitch_s != sWindowText)
	{
		if (IsIIREnabled())
		{
			dsp_preset_impl preset;
			dsp_iir_default::make_preset(p_freq, p_gain, p_type, pitch2, true, g_get_guid(),preset);
			static_api_ptr_t<dsp_config_manager>()->core_enable_dsp(preset, dsp_config_manager::default_insert_last);
			p_qual = pitch2;
		}

	}
}


void uielem_iir::SetIIREnabled(bool state) { m_buttonIIREnabled.SetCheck(state ? BST_CHECKED : BST_UNCHECKED); };
bool uielem_iir::IsIIREnabled()
{
	return m_buttonIIREnabled == NULL || m_buttonIIREnabled.GetCheck() == BST_CHECKED;
};

void uielem_iir::IIRDisable() {
	static_api_ptr_t<dsp_config_manager>()->core_disable_dsp(g_get_guid_iir);
}


void uielem_iir::IIREnable(int p_freq, int p_gain, int p_type, float p_qual, bool IIR_enabled) {
	dsp_preset_impl preset;
	dsp_iir_default::make_preset(p_freq, p_gain, p_type, p_qual, IIR_enabled, g_get_guid(), preset);
	static_api_ptr_t<dsp_config_manager>()->core_enable_dsp(preset, dsp_config_manager::default_insert_last);
}

void uielem_iir::OnEnabledToggle(UINT uNotifyCode, int nID, CWindow wndCtl) {
	pfc::vartoggle_t<bool> ownUpdate(m_ownIIRUpdate, true);
	if (IsIIREnabled()) {
		GetConfig();
		dsp_preset_impl preset;
		dsp_iir_default::make_preset(p_freq, p_gain, p_type, p_qual, IIR_enabled, g_get_guid(),preset);
		//yes change api;
		static_api_ptr_t<dsp_config_manager>()->core_enable_dsp(preset, dsp_config_manager::default_insert_last);
	}
	else {
		static_api_ptr_t<dsp_config_manager>()->core_disable_dsp(guid_iir);
	}

}

void uielem_iir::OnScroll(UINT scrollID, int pos, CWindow window)
{
	pfc::vartoggle_t<bool> ownUpdate(m_ownIIRUpdate, true);
	GetConfig();
	if (IsIIREnabled())
	{
		if (LOWORD(scrollID) != SB_THUMBTRACK)
		{
			IIREnable(p_freq, p_gain, p_type, p_qual, IIR_enabled);
		}
	}
	RefreshControls(p_freq, p_gain, p_type);
}

void uielem_iir::OnChange2(UINT, int id, CWindow)
{
	pfc::vartoggle_t<bool> ownUpdate(m_ownIIRUpdate, true);
	GetConfig();
	if (IsIIREnabled())
	{

		OnConfigChanged();
	}
}

void uielem_iir::DSPConfigChange(dsp_chain_config const& cfg)
{
	if (!m_ownIIRUpdate && m_hWnd != NULL) {
		uielem_iir::ApplySettings(g_get_guid_iir);
	}
}

//set settings if from another control
void uielem_iir::ApplySettings(GUID inGuid)
{
	dsp_preset_impl preset;
	if (static_api_ptr_t<dsp_config_manager>()->core_query_dsp(inGuid, preset)) {
		uielem_iir::SetIIREnabled(true);
		dsp_iir_default::parse_preset(p_freq, p_gain, p_type, p_qual, IIR_enabled, preset);
		uielem_iir::SetIIREnabled(IIR_enabled);
		uielem_iir::SetConfig();
	}
	else {
		uielem_iir::SetIIREnabled(false);
		uielem_iir::SetConfig();
	}
}

void uielem_iir::OnConfigChanged() {
	if (uielem_iir::IsIIREnabled()) {
		uielem_iir::IIREnable(p_freq, p_gain, p_type, p_qual, IIR_enabled);
	}
	else {
		uielem_iir::IIRDisable();
	}

}


void uielem_iir::GetConfig()
{
	p_freq = slider_freq.GetPos();
	p_gain = slider_gain.GetPos();
	p_type = SendDlgItemMessage(IDC_IIRTYPE1, CB_GETCURSEL);
	IIR_enabled = uielem_iir::IsIIREnabled();
	//RefreshControls(p_freq, p_gain, p_type);


}

void uielem_iir::SetConfig()
{

	CWindow w = GetDlgItem(IDC_IIRTYPE1);
	::SendMessage(w, CB_SETCURSEL, p_type, 0);
	uielem_iir::RefreshControls(p_freq, p_gain, p_type);

}

BOOL uielem_iir::OnInitDialog(CWindow, LPARAM)
{
	pitch_edit.AttachToDlgItem(m_hWnd);
	pitch_edit.SubclassWindow(GetDlgItem(IDC_IIRQ));
	slider_freq = GetDlgItem(IDC_IIRFREQ1);
	slider_freq.SetRange(dsp_iir_default::FreqMin, dsp_iir_default::FreqMax);
	slider_gain = GetDlgItem(IDC_IIRGAIN1);
	slider_gain.SetRange(dsp_iir_default::GainMin, dsp_iir_default::GainMax);
	CWindow w = GetDlgItem(IDC_IIRTYPE1);
	uSendMessageText(w, CB_ADDSTRING, 0, "Resonant Lowpass");
	uSendMessageText(w, CB_ADDSTRING, 0, "Resonant Highpass");
	uSendMessageText(w, CB_ADDSTRING, 0, "Bandpass (CSG)");
	uSendMessageText(w, CB_ADDSTRING, 0, "Bandpass (ZPG)");
	uSendMessageText(w, CB_ADDSTRING, 0, "Allpass");
	uSendMessageText(w, CB_ADDSTRING, 0, "Notch");
	uSendMessageText(w, CB_ADDSTRING, 0, "RIAA Tape/Vinyl De-emphasis");
	uSendMessageText(w, CB_ADDSTRING, 0, "Parametric EQ (single band)");
	uSendMessageText(w, CB_ADDSTRING, 0, "Bass Boost");
	uSendMessageText(w, CB_ADDSTRING, 0, "Low shelf");
	uSendMessageText(w, CB_ADDSTRING, 0, "CD De-emphasis");
	uSendMessageText(w, CB_ADDSTRING, 0, "High shelf");


	m_buttonIIREnabled = GetDlgItem(IDC_IIRENABLED);
	m_ownIIRUpdate = false;

	uielem_iir::ApplySettings(g_get_guid_iir);
	return TRUE;
}

void uielem_iir::RefreshControls(int p_freq, int p_gain, int p_type)
{
	slider_freq.ShowWindow(dsp_iir_default::HasParameters(p_type));
	slider_gain.ShowWindow(dsp_iir_default::HasParameters(p_type));

	pfc::string_formatter msg;

	if (!dsp_iir_default::HasParameters(p_type))
	{
		msg.reset();
		::uSetDlgItemText(*this, IDC_IIRFREQINFO1, msg);
		::uSetDlgItemText(*this, IDC_IIRTYPELABEL_Frequency, msg);
		::uSetDlgItemText(*this, IDC_IIRGAININFO1, msg);
		::uSetDlgItemText(*this, IDC_STATIC_Gain, msg);
		return;
	}

	slider_freq.SetPos(p_freq);
	slider_gain.SetPos(p_gain);
	msg.reset();
	::uSetDlgItemText(*this, IDC_IIRTYPELABEL_Frequency, "Frequency (Hz)");
	msg << pfc::format_int(p_freq);
	::uSetDlgItemText(*this, IDC_IIRFREQINFO1, msg);
	msg.reset();
	::uSetDlgItemText(*this, IDC_STATIC_Gain, "Gain (Db)");
	msg << pfc::format_int(p_gain);
	::uSetDlgItemText(*this, IDC_IIRGAININFO1, msg);

	msg.reset();
	msg << pfc::format_float(p_qual, 0, 3);
	CString sWindowText;
	sWindowText = msg.c_str();
	pitch_s = sWindowText;
	pitch_edit.SetWindowText(sWindowText);
}



uint32_t uielem_iir::parseConfig(ui_element_config::ptr cfg) {
	return 1;
}

ui_element_config::ptr uielem_iir::makeConfig(bool init) {
	ui_element_config_builder out;

	if (init)
	{
		uint32_t crap = 1;
		out << crap;
	}
	else
	{
		uint32_t crap = 2;
		out << crap;
	}
	return out.finish(uielem_iir::g_get_guid());
}
uint32_t shit;

const ui_element_instance_callback::ptr m_callback;




