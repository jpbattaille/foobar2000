#include "dsp_iir.h"

class dsp_lpf : public dsp_iir
{
public:
	dsp_lpf(dsp_preset const& in) : dsp_iir(in)
	{
	};
	static const int default_type = LPF;
	static GUID dsp_lpf::g_get_guid()
	{
		return guid_lpf; // {3A2B7602 - 6C11 - 46E3 - 8D34 - 7FF30CFBE507}
	}

	static void dsp_lpf::g_get_name(pfc::string_base& p_out) { p_out = "Low Shelf Filter"; }
	static bool dsp_lpf::g_get_default_preset(dsp_preset& p_out)
	{
		dsp_iir_default::make_preset_default(g_get_guid(), p_out);
		return true;
	}
	static	void dsp_lpf::g_show_config_popup(const dsp_preset& p_data, HWND p_parent, dsp_preset_edit_callback& p_callback)
	{
		popup_lpf::RunConfigPopup(p_data, p_parent, p_callback);
	}
};

static dsp_factory_t<dsp_lpf> g_dsp_lpf_factory;



//class uielem_lpf : public uielem
