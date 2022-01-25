#include "dsp_iir.h"

CMyDSPPopupIIR::CMyDSPPopupIIR(const dsp_preset& initData, dsp_preset_edit_callback& callback) : m_initData(initData), m_callback(callback) { };


LRESULT CMyDSPPopupIIR::OnEditControlChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == 0x1988)
	{
		GetEditText();
	}
	return 0;
}

void CMyDSPPopupIIR::GetEditText()
{
	CString sWindowText;
	pitch_edit.GetWindowText(sWindowText);
	float pitch2 = _ttof(sWindowText);
	if (pitch_s != sWindowText)
	{

		dsp_preset_impl preset;
		dsp_iir_default::make_preset(p_freq, p_gain, p_type, pitch2, true, g_get_guid(), preset);
		p_qual = pitch2;
		m_callback.on_preset_changed(preset);
	}
}

void CMyDSPPopupIIR::DSPConfigChange(dsp_chain_config const& cfg)
{
	if (m_hWnd != NULL) {
		ApplySettings(g_get_guid());
	}
}

void CMyDSPPopupIIR::ApplySettings(GUID inGuid)
{
	dsp_preset_impl preset2;
	if (static_api_ptr_t<dsp_config_manager>()->core_query_dsp(inGuid, preset2)) {
		bool enabled;
		dsp_iir_default::parse_preset(p_freq, p_gain, p_type, p_qual, enabled, preset2);
		CWindow w = GetDlgItem(IDC_IIRTYPE);
		::SendMessage(w, CB_SETCURSEL, p_type, 0);
		RefreshControls(p_freq, p_gain, p_type);
	}
}

BOOL CMyDSPPopupIIR::OnInitDialog(CWindow, LPARAM)
{

	pitch_edit.AttachToDlgItem(m_hWnd);
	pitch_edit.SubclassWindow(GetDlgItem(IDC_IIRQ));
	slider_freq = GetDlgItem(IDC_IIRFREQ);
	slider_freq.SetRange(dsp_iir_default::FreqMin, dsp_iir_default::FreqMax);
	slider_gain = GetDlgItem(IDC_IIRGAIN);
	slider_gain.SetRange(dsp_iir_default::GainMin, dsp_iir_default::GainMax);
	{

		bool enabled;
		dsp_iir_default::parse_preset(p_freq, p_gain, p_type, p_qual, enabled, m_initData);


		CWindow w = GetDlgItem(IDC_IIRTYPE);
		CreateCombo(w);
		::SendMessage(w, CB_SETCURSEL, p_type, 0);
		RefreshControls(p_freq, p_gain, p_type);

	}
	return TRUE;
}

void CMyDSPPopupIIR::CreateCombo(HWND w) {
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
};

void CMyDSPPopupIIR::OnButton(UINT, int id, CWindow)
{
	GetEditText();
	EndDialog(id);
}

void CMyDSPPopupIIR::OnChange(UINT scrollid, int id, CWindow window)
{
	CWindow w;
	p_freq = slider_freq.GetPos();
	p_gain = slider_gain.GetPos();
	p_type = SendDlgItemMessage(IDC_IIRTYPE, CB_GETCURSEL);
	{
		dsp_preset_impl preset;
		dsp_iir_default::make_preset(p_freq, p_gain, p_type, p_qual, true, g_get_guid(), preset);
		m_callback.on_preset_changed(preset);
	}

	RefreshControls(p_freq, p_gain, p_type);

}
void CMyDSPPopupIIR::OnScroll(UINT scrollid, int id, CWindow window)
{
	OnChange(scrollid, id, window);
}



void CMyDSPPopupIIR::RefreshControls(int p_freq, int p_gain, int p_type)
{
	slider_freq.ShowWindow(dsp_iir_default::HasParameters(p_type));
	slider_gain.ShowWindow(dsp_iir_default::HasParameters(p_type));

	pfc::string_formatter msg;
	if (!dsp_iir_default::HasParameters(p_type))
	{
		msg.reset();
		::uSetDlgItemText(*this, IDC_IIRFREQINFO, msg);
		::uSetDlgItemText(*this, IDC_IIRGAININFO, msg);
		return;

	}

	slider_freq.SetPos(p_freq);
	slider_gain.SetPos(p_gain);
	msg << "Frequency: ";
	msg << pfc::format_int(p_freq) << " Hz";
	::uSetDlgItemText(*this, IDC_IIRFREQINFO, msg);
	msg.reset();
	msg << "Gain: ";
	msg << pfc::format_int(p_gain) << " db";
	::uSetDlgItemText(*this, IDC_IIRGAININFO, msg);

	msg.reset();
	msg << pfc::format_float(p_qual, 0, 3);
	CString sWindowText;
	sWindowText = msg.c_str();
	pitch_s = sWindowText;
	pitch_edit.SetWindowText(sWindowText);
}

void CMyDSPPopupIIR::RunConfigPopup(const dsp_preset& p_data, HWND p_parent, dsp_preset_edit_callback& p_callback)
{
	CMyDSPPopupIIR popup(p_data, p_callback);
	if (popup.DoModal(p_parent) != IDOK) p_callback.on_preset_changed(p_data);
}
;


