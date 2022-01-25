#pragma once

#ifndef DSP_IIR_H
#define DSP_IIR_H

#include "../helpers/foobar2000+atl.h"
#include "../../libPPUI/win32_utility.h"
#include "../../libPPUI/win32_op.h" // WIN32_OP()
#include "../helpers/BumpableElem.h"
#include "resource.h"
#include "iirfilters.h"
#include "dsp_guids.h"
class dsp_iir_default
{ public:
	static const int FreqMin = 0;
	static const int Freq = 400;
	static const int FreqMax = 16000;
	static const int FreqRangeTotal = FreqMax - FreqMin;
	static const int GainMin = -20;
	static const int Gain = 5;
	static const int GainMax = 20;
	static const int GainRangeTotal = GainMax - GainMin;
	static const int Type = LPF;
	static constexpr float Qual = 0.707;
	static const bool Enabled = true;
	static void make_preset(int p_freq, int p_gain, int p_type, float p_quality, bool enabled, GUID inGuid, dsp_preset& out) {
		dsp_preset_builder builder;
		builder << p_freq;
		builder << p_gain; //gain
		builder << p_type; //filter type
		builder << p_quality;
		builder << enabled;
		builder.finish(inGuid, out);
	};
	static void make_preset_default(GUID inGuid, dsp_preset& p_out) {
		dsp_iir_default::make_preset(dsp_iir_default::Freq, dsp_iir_default::Gain,
			dsp_iir_default::Type, dsp_iir_default::Qual,
			dsp_iir_default::Enabled, inGuid, p_out);
	}
	static void parse_preset(int& p_freq, int& p_gain, int& p_type, float& p_quality, bool& enabled, const dsp_preset& in)
	{
		try
		{
			dsp_preset_parser parser(in);
			parser >> p_freq;
			parser >> p_gain; //gain
			parser >> p_type; //filter type
			parser >> p_quality;
			parser >> enabled;
		}
		catch (exception_io_data) {
			p_freq = dsp_iir_default::Freq; p_gain = dsp_iir_default::Gain;
			p_type = dsp_iir_default::Type; p_quality = dsp_iir_default::Qual;
			enabled = dsp_iir_default::Enabled;
		}
	};
	static BOOL HasParameters(int p_type)
	{
		return !((p_type == RIAA_CD) || (p_type == RIAA_phono));
	}
	};


class CEditMod : public CWindowImpl<CEditMod, CEdit >
{
public:
	BEGIN_MSG_MAP(CEditMod)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
	END_MSG_MAP()

	CEditMod(HWND hWnd = NULL) { }
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		switch (wParam)
		{
		case '\r': //Carriage return
			::PostMessage(m_parent, WM_USER, 0x1988, 0L);
			return 0;
			break;
		}
		return DefWindowProc(uMsg, wParam, lParam);
	}
	void AttachToDlgItem(HWND parent)
	{
		m_parent = parent;
	}
private:
	UINT m_dlgItem;
	HWND m_parent;
};

class dsp_iir : public dsp_impl_base {	
	
	
protected:
	int m_rate, m_ch, m_ch_mask;
	int p_freq; //40.0, 13000.0 (Frequency: Hz)
	int p_gain; //gain
	int p_type; //filter type
	float p_qual;
	bool iir_enabled;
	pfc::array_t<IIRFilter> m_buffers;

public:
	dsp_iir::dsp_iir(dsp_preset const& in);	
	static GUID g_get_guid();
	static void g_get_name(pfc::string_base& p_out) ;
	static void g_show_config_popup(const dsp_preset& p_data, HWND p_parent, dsp_preset_edit_callback& p_callback) ;
	static bool g_have_config_popup();
	void on_endofplayback(abort_callback&) {};
	void on_endoftrack(abort_callback&) {};
	bool on_chunk(audio_chunk* chunk, abort_callback&);
	void flush();
	double get_latency();
	static bool g_get_default_preset(dsp_preset& p_out);
	bool need_track_change_mark();

};
static dsp_factory_t<dsp_iir> g_dsp_iir_factory;


class CMyDSPPopupIIR : public CDialogImpl<CMyDSPPopupIIR>
{
public:
	CMyDSPPopupIIR(const dsp_preset& initData, dsp_preset_edit_callback& callback);
	enum { IDD = IDD_IIR1 };
	enum
		{
			FreqMin = 0,
			FreqMax = 40000,
			FreqRangeTotal = FreqMax,
			GainMin = -100,
			GainMax = 100,
			GainRangeTotal = GainMax - GainMin
		};
	const GUID g_get_guid() {
		return guid_iir;
	}

	BEGIN_MSG_MAP(CMyDSPPopupIIR)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnButton)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnButton)
		COMMAND_HANDLER_EX(IDC_IIRTYPE, CBN_SELCHANGE, OnChange)
		MSG_WM_HSCROLL(OnScroll)
		MESSAGE_HANDLER(WM_USER, OnEditControlChange)
	END_MSG_MAP()

	static void RunConfigPopup(const dsp_preset& p_data, HWND p_parent, dsp_preset_edit_callback& p_callback);
private:
	LRESULT OnEditControlChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	void GetEditText();
	void DSPConfigChange(dsp_chain_config const& cfg);	
	void ApplySettings(GUID inGuid);
	BOOL OnInitDialog(CWindow, LPARAM);
	void OnButton(UINT, int id, CWindow);
	void OnChange(UINT scrollid, int id, CWindow window);
	void OnScroll(UINT scrollid, int id, CWindow window)		;
	void RefreshControls(int p_freq, int p_gain, int p_type);
	void CMyDSPPopupIIR::CreateCombo(HWND w);

	int p_freq;
	int p_gain;
	int p_type;
	float p_qual;
	CEditMod pitch_edit;
	CString pitch_s;
	const dsp_preset& m_initData; // modal dialog so we can reference this caller-owned object.
	dsp_preset_edit_callback& m_callback;
	CTrackBarCtrl slider_freq, slider_gain;


};


#endif
