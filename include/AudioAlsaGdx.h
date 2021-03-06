/*
 * AudioAlsaGdx.h - device-class that implements ALSA-PCM-output
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef AUDIO_ALSA_GDX_H
#define AUDIO_ALSA_GDX_H

#include "lmmsconfig.h"

#ifdef LMMS_HAVE_ALSA

// older ALSA-versions might require this
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <QThread>

#include "AudioDevice.h"


class AudioAlsaGdx : public AudioDevice, public QThread
{
	// Public classes and enums
public:
	/**
	 * @brief Contains the relevant information about available ALSA devices
	 */
	class DeviceInfo
	{
	public:
		DeviceInfo(QString const & deviceName, QString const & deviceDescription) :
			m_deviceName(deviceName),
			m_deviceDescription(deviceDescription)
		{}
		~DeviceInfo() {}

		QString const & getDeviceName() const { return m_deviceName; }
		QString const & getDeviceDescription() const { return m_deviceDescription; }

	private:
		QString m_deviceName;
		QString m_deviceDescription;

	};

	typedef std::vector<DeviceInfo> DeviceInfoCollection;

public:
	AudioAlsaGdx( bool & _success_ful, Mixer* mixer );
	virtual ~AudioAlsaGdx();

	inline static QString name()
	{
		return QT_TRANSLATE_NOOP( "setupWidget",
			"ALSA GDX" );
	}

	static QString probeDevice();

	static DeviceInfoCollection getAvailableDevices();

private:
	virtual void startProcessing();
	virtual void stopProcessing();
	virtual void applyQualitySettings();
	virtual void run();

	int setHWParams( const ch_cnt_t _channels, snd_pcm_access_t _access );
	int setSWParams();
	int handleError( int _err );


	snd_pcm_t * m_outHandle;

	snd_pcm_uframes_t m_bufferSize;
	snd_pcm_uframes_t m_periodSize;

	snd_pcm_hw_params_t * m_hwParams;
	snd_pcm_sw_params_t * m_swParams;

	bool m_convertEndian;

} ;

#endif

#endif
