/*
 * Effect.cpp - base-class for effects
 *
 * Copyright (c) 2018 gi0e5b06 (on github.com)
 * Copyright (c) 2006-2007 Danny McRae <khjklujn/at/users.sourceforge.net>
 * Copyright (c) 2006-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include <cmath>

#include <QDomElement>

#include "Effect.h"
#include "EffectChain.h"
#include "EffectControls.h"
#include "EffectView.h"

#include "ConfigManager.h"
#include "WaveForm.h"
//#include "lmms_math.h"


Effect::Effect( const Plugin::Descriptor * _desc,
                Model * _parent,
                const Descriptor::SubPluginFeatures::Key * _key ) :
    Plugin( _desc, _parent ),
    m_wetDryModel  ( 1.0f, 0.0f, 1.0f, 0.01f, this, tr( "Wet/Dry mix" ) ), //min=-1
    m_balanceModel ( 0.0f,-1.0f, 1.0f, 0.01f, this, tr( "Balance" ) ),
    m_gateClosed( false ),
    m_parent( NULL ),
    m_key( _key ? *_key : Descriptor::SubPluginFeatures::Key() ),
    m_processors( 1 ),
    m_okay( true ),
    m_noRun( false ),
    m_running( false ),
    m_bufferCount( 0 ),
    m_enabledModel( true, this, tr( "Effect enabled" ) ),
    m_gateModel    ( 0.0f, 0.0f, 1.0f, 0.01f, this, tr( "Gate" ) ),
    m_autoQuitModel( 0.0f, 0.0f, 8000.0f, 1.0f, 1.0f, this, tr( "Decay" ) ),
    m_autoQuitDisabled( false )
{
        m_gateModel.setScaleLogarithmic(true);

	m_srcState[0] = m_srcState[1] = NULL;
	reinitSRC();

	if( ConfigManager::inst()->value( "ui", "disableautoquit").toInt() )
	{
		m_autoQuitDisabled = true;
	}
        //qInfo("Effect::Effect end constructor");
}




Effect::~Effect()
{
	for( int i = 0; i < 2; ++i )
	{
		if( m_srcState[i] != NULL )
		{
			src_delete( m_srcState[i] );
		}
	}
}




void Effect::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	m_enabledModel.saveSettings( _doc, _this, "on" );
	m_wetDryModel.saveSettings( _doc, _this, "wet" );
	m_autoQuitModel.saveSettings( _doc, _this, "autoquit" );
	m_gateModel.saveSettings( _doc, _this, "gate" );
	m_balanceModel.saveSettings( _doc, _this, "balance" );
	controls()->saveState( _doc, _this );
}




void Effect::loadSettings( const QDomElement & _this )
{
	m_enabledModel.loadSettings( _this, "on" );
	m_wetDryModel.loadSettings( _this, "wet" );
	m_autoQuitModel.loadSettings( _this, "autoquit" );
	m_gateModel.loadSettings( _this, "gate" );
	m_balanceModel.loadSettings( _this, "balance" );

	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( controls()->nodeName() == node.nodeName() )
			{
				controls()->restoreState( node.toElement() );
			}
		}
		node = node.nextSibling();
	}

	/*
	if(m_wetDryModel.minValue()==-1.f)
	{
		m_wetDryModel.setMinValue(0.f);
	}
	*/
	if(m_wetDryModel.value()<0.f)
	{
		m_wetDryModel.setValue(-m_wetDryModel.value());
	}

        if(!isBalanceable())
                m_balanceModel.setValue(0.f);

        m_gateModel.setScaleLogarithmic(true);
}





Effect * Effect::instantiate( const QString& pluginName,
				Model * _parent,
				Descriptor::SubPluginFeatures::Key * _key )
{
	Plugin * p = Plugin::instantiate( pluginName, _parent, _key );
	// check whether instantiated plugin is an effect
	if( dynamic_cast<Effect *>( p ) != NULL )
	{
                //qInfo("Effect::instantiate SUCCESS %s",qPrintable(pluginName));
		// everything ok, so return pointer
		Effect * effect = dynamic_cast<Effect *>( p );
		effect->m_parent = dynamic_cast<EffectChain *>(_parent);
		return effect;
	}

        qWarning("Effect::instantiate FAILED %s",qPrintable(pluginName));
	// not quite... so delete plugin and leave it up to the caller to instantiate a DummyEffect
	delete p;

	return NULL;
}




bool Effect::gateHasClosed(float& _rms, sampleFrame* _buf, const fpp_t _frames)
{
	if(!isAutoQuitEnabled()) return false;
        if(!isRunning()) return false;

        const float g=gate();
	// Check whether we need to continue processing input.  Restart the
	// counter if the threshold has been exceeded.
	if( g>0.f )
	{
                if(g<1.f && _rms<0.f) _rms=computeRMS(_buf,_frames);
                if(_rms < g)
                {
                        incrementBufferCount();
                        if( !m_gateClosed && (bufferCount() > timeout() ))
                        {
                                m_gateClosed=true;
                                stopRunning();
                                resetBufferCount();
                                return true;
                        }
                        else return false;
                }
	}

        resetBufferCount();
        return false;
}


bool Effect::gateHasOpen(float& _rms, sampleFrame* _buf, const fpp_t _frames)
{
	if(!isAutoQuitEnabled()) return false;
        //if(isRunning()) return false;

        const float g=gate();
	// Check whether we need to continue processing input.  Restart the
	// counter if the threshold has been exceeded.
	if( m_gateClosed && g<1.f)
	{
                if(g>0.f && _rms<0.f) _rms=computeRMS(_buf,_frames);
                if(g==0.f || _rms >= g)
                {
                        m_gateClosed=false;
                        if(!isRunning()) startRunning();
                        resetBufferCount();
                        return true;
                }
        }

        return false;
}


float Effect::computeRMS(sampleFrame* _buf, const fpp_t _frames)
{
	//if( !isEnabled() ) return 0.f;
	//if(!isAutoQuitEnabled()) return 1.f;

        float rms = 0.0f;
        fpp_t step= qMax(1,_frames>>5);
        for( fpp_t f = 0; f < _frames; f+=step )
                rms+=_buf[f][0]*_buf[f][0]+_buf[f][1]*_buf[f][1];
        //rms/=_frames;
        rms/= (_frames/step);
        //return fastsqrtf01(qBound(0.f,rms,1.f));
        return WaveForm::sqrt(qBound(0.f,rms,1.f));
}


bool Effect::shouldProcessAudioBuffer(sampleFrame* _buf, const fpp_t _frames,
                                      bool& _smoothBegin, bool& _smoothEnd)
{
	if(!isOkay() || dontRun() || !isEnabled()) return false;

        _smoothBegin=false;
        _smoothEnd  =false;

	if(isAutoQuitEnabled())
        {
                float rms=-1.f;
                if(gateHasOpen(rms,_buf,_frames))
                {
                        //qInfo("%s: gate open",qPrintable(nodeName()));
                        _smoothBegin=true;
                }
                else
                if(gateHasClosed(rms,_buf,_frames))
                {
                        //qInfo("%s: gate closed",qPrintable(nodeName()));
                        _smoothEnd=true;
                }
        }

        if(!isRunning() && !_smoothEnd)
        {
                const ValueBuffer* wetDryBuf = m_wetDryModel.valueBuffer();

                for(fpp_t f=0; f<_frames; ++f)
                {
                        float w = (wetDryBuf ? wetDryBuf->value(f)
                                   : m_wetDryModel.value());
                        float d = 1.0f-w;
                        _buf[f][0]*=d;
                        _buf[f][1]*=d;
                }

                return false;
        }

        return true;
}


bool Effect::shouldKeepRunning(sampleFrame* _buf, const fpp_t _frames)
{
	if(!isRunning()) return false;
	if(!isAutoQuitEnabled()) return true;

        float rms=computeRMS(_buf,_frames);
        if(rms>0.0000001f) return true;

        incrementBufferCount();
        if(bufferCount() > qMax<int>(176,timeout()))
        {
                stopRunning();
                resetBufferCount();
                return false;
        }
	return true;
}


void Effect::computeWetDryLevels(fpp_t _f, fpp_t _frames,
                                 bool _smoothBegin, bool _smoothEnd,
                                 float& _w0,float &_d0,
                                 float& _w1,float &_d1)
{
        const ValueBuffer* wetDryBuf = m_wetDryModel.valueBuffer();

        float w=(wetDryBuf ? wetDryBuf->value(_f)
                 : m_wetDryModel.value());
        float d=1.0f-w;

        int nsb=_frames;
        if(nsb>128) nsb=128;
        if(_smoothBegin && _f<nsb)
                w*=1.f*_f/nsb;
        int nse=_frames;
        if(nse>128) nse=128;
        if(_smoothEnd && _f>=_frames-nse)
                w*=1.f*(_frames-1-_f)/nse;

        if(isGateClosed() &&!_smoothEnd)
        {
                //w=0.f;
                _w0=0.f;
                _w1=0.f;
        }
        else
        if(isBalanceable())
        {
                const ValueBuffer* balanceBuf = m_balanceModel.valueBuffer();
                const float bal = (balanceBuf ? balanceBuf->value(_f)
                                   : m_balanceModel.value());
                const float bal0 = bal < 0.f ? 1.f : 1.f - bal;
                const float bal1 = bal < 0.f ? 1.f + bal : 1.f;
                _w0   = bal0 * w;
                _w1   = bal1 * w;
        }
        else
        {
                _w0=w;
                _w1=w;
        }

        _d0   = d*(1.0f - _w0);
        _d1   = d*(1.0f - _w1);
}


PluginView * Effect::instantiateView( QWidget * _parent )
{
	return new EffectView( this, _parent );
}




void Effect::reinitSRC()
{
	for( int i = 0; i < 2; ++i )
	{
		if( m_srcState[i] != NULL )
		{
			src_delete( m_srcState[i] );
		}
		int error;
		if( ( m_srcState[i] = src_new(
			Engine::mixer()->currentQualitySettings().
							libsrcInterpolation(),
					DEFAULT_CHANNELS, &error ) ) == NULL )
		{
			qFatal( "Error: src_new() failed in effect.cpp!\n" );
		}
	}
}




void Effect::resample( int _i, const sampleFrame * _src_buf, sample_rate_t _src_sr,
                       sampleFrame * _dst_buf, sample_rate_t _dst_sr,
                       f_cnt_t _frames )
{
	if( m_srcState[_i] == NULL )
	{
		return;
	}
	m_srcData[_i].input_frames = _frames;
	m_srcData[_i].output_frames = Engine::mixer()->framesPerPeriod();
	m_srcData[_i].data_in = (float *) _src_buf[0];
	m_srcData[_i].data_out = _dst_buf[0];
	m_srcData[_i].src_ratio = (double) _dst_sr / _src_sr;
	m_srcData[_i].end_of_input = 0;
	int error;
	if( ( error = src_process( m_srcState[_i], &m_srcData[_i] ) ) )
	{
		qFatal( "Effect::resample(): error while resampling: %s\n",
							src_strerror( error ) );
	}
}

