/*
 * MidiEffect.cpp - base-class for effects
 *
 * Copyright (c) 2017 gi0e5b06
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

#include <QDomElement>

#include "MidiEffect.h"
#include "MidiEffectChain.h"
#include "MidiEffectControls.h"
#include "MidiEffectView.h"

#include "ConfigManager.h"


MidiEffect::MidiEffect(const Plugin::Descriptor* _desc,
		       Model* _parent,
		       const Descriptor::SubPluginFeatures::Key* _key ) :
	Plugin( _desc, _parent ),
	m_parent( NULL ),
	m_key( _key ? *_key : Descriptor::SubPluginFeatures::Key() ),
	//m_processors( 1 ),
	m_okay( true ),
	m_noRun( false ),
	m_running( false ),
	//m_bufferCount( 0 ),
	m_enabledModel( true, this, tr( "enabled" ) ),
	m_wetDryModel  ( 1.0f, 0.0f, 1.0f, 0.01f, this, tr( "Wet/Dry mix" ) ), //min=-1
	m_gateModel    ( 0.0f, 0.0f, 1.0f, 0.01f, this, tr( "Gate" ) ),
	m_autoQuitModel( 1.0f, 1.0f, 8000.0f, 100.0f, 1.0f, this, tr( "Decay" ) ),
	m_autoQuitDisabled( false )
{
	//m_srcState[0] = m_srcState[1] = NULL;
	//reinitSRC();

	if( ConfigManager::inst()->value( "ui", "disableautoquit").toInt() )
	{
		m_autoQuitDisabled = true;
	}
}


MidiEffect::~MidiEffect()
{
	/*
	for( int i = 0; i < 2; ++i )
	{
		if( m_srcState[i] != NULL )
		{
			src_delete( m_srcState[i] );
		}
	}
	*/
}


void MidiEffect::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	m_enabledModel.saveSettings( _doc, _this, "on" );
	m_wetDryModel.saveSettings( _doc, _this, "wet" );
	m_autoQuitModel.saveSettings( _doc, _this, "autoquit" );
	m_gateModel.saveSettings( _doc, _this, "gate" );
	controls()->saveState( _doc, _this );
}


void MidiEffect::loadSettings( const QDomElement & _this )
{
	m_enabledModel.loadSettings( _this, "on" );
	m_wetDryModel.loadSettings( _this, "wet" );
	m_autoQuitModel.loadSettings( _this, "autoquit" );
	m_gateModel.loadSettings( _this, "gate" );

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
}


MidiEffect * MidiEffect::instantiate( const QString& pluginName,
				      Model* _parent,
				      Descriptor::SubPluginFeatures::Key* _key )
{
	Plugin* p = Plugin::instantiate( pluginName, _parent, _key );
	// check whether instantiated plugin is an effect
	if( dynamic_cast<MidiEffect *>( p ) != NULL )
	{
		// everything ok, so return pointer
		MidiEffect * effect = dynamic_cast<MidiEffect *>( p );
		effect->m_parent = dynamic_cast<MidiEffectChain *>(_parent);
		return effect;
	}

	// not quite... so delete plugin and leave it up to the caller to instantiate a DummyMidiEffect
	delete p;

	return NULL;
}




void MidiEffect::checkGate( double _out_sum )
{
	if( m_autoQuitDisabled )
	{
		return;
	}

	// Check whether we need to continue processing input.  Restart the
	// counter if the threshold has been exceeded.
	if( _out_sum - gate() <= typeInfo<float>::minEps() )
	{
		incrementBufferCount();
		if( bufferCount() > timeout() )
		{
			stopRunning();
			resetBufferCount();
		}
	}
	else
	{
		resetBufferCount();
	}
}


PluginView* MidiEffect::instantiateView(QWidget* _parent)
{
	return new MidiEffectView(this, _parent);
}


/*
void MidiEffect::reinitSRC()
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
*/


/*
void MidiEffect::resample( int _i, const sampleFrame * _src_buf,
							sample_rate_t _src_sr,
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
		qFatal( "MidiEffect::resample(): error while resampling: %s\n",
							src_strerror( error ) );
	}
}
*/