/*
 * CompressorGDXControls.h - 
 *
 * Copyright (c) 2017
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

#ifndef COMPRESSORGDX_CONTROLS_H
#define COMPRESSORGDX_CONTROLS_H

#include "EffectControls.h"
#include "CompressorGDXDialog.h"
#include "Knob.h"


class CompressorGDX;


class CompressorGDXControls : public EffectControls
{
	Q_OBJECT
public:
	CompressorGDXControls( CompressorGDX* effect );
	virtual ~CompressorGDXControls()
	{
	}

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );
	inline virtual QString nodeName() const
	{
		return "CompressorGDXControls";
	}

	virtual int controlCount()
	{
		return 4;
	}

	virtual EffectControlDialog* createView()
	{
		return new CompressorGDXDialog( this );
	}


private slots:
	void changeControl();

private:
	CompressorGDX* m_effect;

	FloatModel m_thresholdModel;
	FloatModel m_ratioModel;
	FloatModel m_outGainModel;
	FloatModel m_modeModel;

	friend class CompressorGDXDialog;
	friend class CompressorGDX;

} ;

#endif
