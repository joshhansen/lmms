/*
 * Note.cpp - implementation of class note
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "Note.h"
#include "DetuningHelper.h"


Note::Note( const MidiTime & length, const MidiTime & pos,
	    int key, volume_t volume, panning_t panning,
	    DetuningHelper * detuning ) :
	m_selected( false ),
	m_oldKey( qBound( 0, key, NumKeys ) ),
	m_oldPos( pos ),
	m_oldLength( length ),
	m_isPlaying( false ),
	m_key( qBound( 0, key, NumKeys ) ),
	m_volume( qBound( MinVolume, volume, MaxVolume ) ),
	m_panning( qBound( PanningLeft, panning, PanningRight ) ),
	m_length( length ),
	m_pos( pos ),
	m_detuning( NULL )
{
	if( detuning )
	{
		m_detuning = sharedObject::ref( detuning );
	}
	else
	{
		createDetuning();
	}
}




Note::Note( const Note & note ) :
	SerializingObject( note ),
	m_selected( note.m_selected ),
	m_oldKey( note.m_oldKey ),
	m_oldPos( note.m_oldPos ),
	m_oldLength( note.m_oldLength ),
	m_isPlaying( note.m_isPlaying ),
	m_key( note.m_key),
	m_volume( note.m_volume ),
	m_panning( note.m_panning ),
	m_length( note.m_length ),
	m_pos( note.m_pos ),
	m_detuning( NULL )
{
	if( note.m_detuning )
	{
		m_detuning = sharedObject::ref( note.m_detuning );
	}
}




Note::~Note()
{
        //qInfo("deleting note %p",this);
	if( m_detuning )
	{
		sharedObject::unref( m_detuning );
	}
}




void Note::setLength( const MidiTime & length )
{
	m_length = length;
}




void Note::setPos( const MidiTime & pos )
{
	m_pos = pos;
}




void Note::setKey( const int key )
{
	const int k = qBound( 0, key, NumKeys - 1 );
	m_key = k;
}




void Note::setVolume( volume_t volume )
{
	const volume_t v = qBound( MinVolume, volume, MaxVolume );
	m_volume = v;
}




void Note::setPanning( panning_t panning )
{
	const panning_t p = qBound( PanningLeft, panning, PanningRight );
	m_panning = p;
}




MidiTime Note::quantized( const MidiTime & m, const int qGrid )
{
	float p = ( (float) m / qGrid );
	if( p - floorf( p ) < 0.5f )
	{
		return static_cast<int>( p ) * qGrid;
	}
	return static_cast<int>( p + 1 ) * qGrid;
}




void Note::quantizeLength( const int qGrid )
{
	setLength( quantized( length(), qGrid ) );
	if( length() == 0 )
	{
		setLength( qGrid );
	}
}




void Note::quantizePos( const int qGrid )
{
	setPos( quantized( pos(), qGrid ) );
}




void Note::saveSettings( QDomDocument & doc, QDomElement & parent )
{
	parent.setAttribute( "key", m_key );
	parent.setAttribute( "vol", m_volume );
	parent.setAttribute( "pan", m_panning );
	parent.setAttribute( "len", m_length );
	parent.setAttribute( "pos", m_pos );

	if( m_detuning && m_length )
	{
		m_detuning->saveSettings( doc, parent );
	}
}




void Note::loadSettings( const QDomElement & _this )
{
	const int oldKey = _this.attribute( "tone" ).toInt() + _this.attribute( "oct" ).toInt() * KeysPerOctave;
	m_key = qMax( oldKey, _this.attribute( "key" ).toInt() );
	m_volume = _this.attribute( "vol" ).toInt();
	m_panning = _this.attribute( "pan" ).toInt();
	m_length = _this.attribute( "len" ).toInt();
	m_pos = _this.attribute( "pos" ).toInt();

	if( _this.hasChildNodes() )
	{
		createDetuning();
		m_detuning->loadSettings( _this );
	}
}





void Note::createDetuning()
{
	if( m_detuning == NULL )
	{
		m_detuning = sharedObject::ref(new DetuningHelper);
		(void) m_detuning->automationPattern();
		m_detuning->setRange( -MaxDetuning, MaxDetuning, 0.5f );
		m_detuning->automationPattern()->setProgressionType( AutomationPattern::LinearProgression );
	}
}




bool Note::hasDetuningInfo() const
{
	return m_detuning && m_detuning->hasAutomation();
}



bool Note::withinRange(int tickStart, int tickEnd) const
{
	return pos().getTicks() >= tickStart && pos().getTicks() <= tickEnd
		&& length().getTicks() != 0;
}

static QHash<QString,int> MIDI_KEYS_N2I;
static QVector<QString>   MIDI_KEYS_I2N;

void Note::buildKeyTables()
{
        if(MIDI_KEYS_N2I.size()==0)
        {
                const char* NOK[12]={ "c","c#","d","d#","e","f","f#","g","g#","a","a#","b" };
                for(int o=0; o<11; o++)
                for(int n=0;n<12;n++)
                {
                        int i=o*12+n;
                        if(i>127) continue;
                        QString k=QString("%1%2").arg(NOK[n]).arg(o-1);
                        MIDI_KEYS_N2I.insert(k,i);
                        MIDI_KEYS_I2N.insert(i,k);
                }
        }
}


int Note::findKeyNum(QString& _name)
{
        buildKeyTables();
        int r=MIDI_KEYS_N2I.value(_name.toLower(),-1);
        if((r>=0)||r<=127) return r;
        r=_name.toInt();
        if((r>0)||(r<=127)||(_name=="0")) return r;
        return -1;
}


QString Note::findKeyName(int _num)
{
        if((_num<0)||(_num>127)) return "";
        buildKeyTables();
        return MIDI_KEYS_I2N.value(_num,"");
}
