#include "dsp_iir.h"

dsp_iir::dsp_iir(dsp_preset const& in) :m_rate(0), m_ch(0), m_ch_mask(0),
 p_freq(dsp_iir_default::Freq), p_qual(dsp_iir_default::Qual), p_gain(dsp_iir_default::Gain), p_type(dsp_iir_default::Type)
{
	iir_enabled = true;
	dsp_iir_default::parse_preset(p_freq, p_gain, p_type, p_qual, iir_enabled, in);
}


GUID dsp_iir::g_get_guid()
{
	// {FEA092A6-EA54-4f62-B180-4C88B9EB2B67}

	return guid_iir;
}

void dsp_iir::g_get_name(pfc::string_base& p_out) { p_out = "IIR Filter"; }

bool dsp_iir::on_chunk(audio_chunk* chunk, abort_callback&)
{
	if (!iir_enabled)return true;
	if (chunk->get_srate() != m_rate || chunk->get_channels() != m_ch || chunk->get_channel_config() != m_ch_mask)
	{
		m_rate = chunk->get_srate();
		m_ch = chunk->get_channels();
		m_ch_mask = chunk->get_channel_config();
		m_buffers.set_count(0);
		m_buffers.set_count(m_ch);
		for (unsigned i = 0; i < m_ch; i++)
		{
			IIRFilter& e = m_buffers[i];
			e.setFrequency(p_freq);
			e.setQuality(p_qual);
			e.setGain(p_gain);
			e.init(m_rate, p_type);
		}
	}

	for (unsigned i = 0; i < m_ch; i++)
	{
		IIRFilter& e = m_buffers[i];
		audio_sample* data = chunk->get_data() + i;
		for (unsigned j = 0, k = chunk->get_sample_count(); j < k; j++)
		{
			*data = e.Process(*data);
			data += m_ch;
		}
	}

	return true;
}



void dsp_iir::flush()
{
	m_buffers.set_count(0);
	m_rate = 0;
	m_ch = 0;
	m_ch_mask = 0;
}
double dsp_iir::get_latency()
{
	return 0;
}
bool dsp_iir::need_track_change_mark()
{
	return false;
}
bool dsp_iir::g_get_default_preset(dsp_preset& p_out)
{
	dsp_iir_default::make_preset_default(g_get_guid(), p_out);
	return true;
}
void dsp_iir::g_show_config_popup(const dsp_preset& p_data, HWND p_parent, dsp_preset_edit_callback& p_callback)
{
	CMyDSPPopupIIR::RunConfigPopup(p_data, p_parent, p_callback);
}
bool dsp_iir::g_have_config_popup() { return true; }






