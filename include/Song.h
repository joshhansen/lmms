/*
 * Song.h - class song - the root of the model-tree
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

#ifndef SONG_H
#define SONG_H

#include "Controller.h"
#include "Engine.h"
#include "MeterModel.h"
#include "Mixer.h"
#include "TrackContainer.h"
#include "VstSyncController.h"
//#include "ExportFilter.h"
#include "Transportable.h"
#include "MetaData.h"

//#include <QSharedMemory>
//#include <QVector>

#include <cmath>
#include <utility>

class AutomationTrack;
class Pattern;
class TimeLineWidget;


const bpm_t MinTempo = 10;
const bpm_t DefaultTempo = 140;
const bpm_t MaxTempo = 999;
const tick_t MaxSongLength = 9999 * DefaultTicksPerTact;


class EXPORT Song : public TrackContainer, public virtual Transportable
{
	Q_OBJECT
	mapPropertyFromModel( int,getTempo,setTempo,m_tempoModel );
	mapPropertyFromModel( int,masterPitch,setMasterPitch,m_masterPitchModel );
	mapPropertyFromModel( int,masterVolume,setMasterVolume, m_masterVolumeModel );

public:
	enum PlayModes
	{
		Mode_None,
		Mode_PlaySong,
		Mode_PlayBB,
		Mode_PlayPattern,
		Mode_PlayAutomationPattern,
		Mode_Count
	} ;

	void clearErrors();
	void collectError( const QString error );
	bool hasErrors();
	QString errorSummary();

	class PlayPos : public MidiTime
	{
	public:
                PlayPos( const int abs = 0 ) :
                         MidiTime( abs ),
			 m_timeLine( NULL ),
			 m_currentFrame( 0.0f )
		{
		}

                inline void setCurrentFrame( const float f ) // relative
		{
			m_currentFrame = f;
		}

		inline float currentFrame() const // relative
		{
			return m_currentFrame;
		}

                inline float absoluteFrame() const
                {
                        return getTicks()*Engine::framesPerTick()+m_currentFrame;
                }

                inline void setAbsoluteFrame(float _f)
                {
                        setTicks(_f/Engine::framesPerTick());
                        setCurrentFrame(fmodf(_f,Engine::framesPerTick()));
                }

                TimeLineWidget * m_timeLine;

	private:
		float m_currentFrame;

	} ;



	void processNextBuffer();

	inline int getLoadingTrackCount() const
	{
		return m_nLoadingTrack;
	}

	inline int getMilliseconds() const
	{
		return m_elapsedMilliSeconds;
	}

	inline void setToTime( int millis )
	{
		m_elapsedMilliSeconds = millis;
	}

	inline void setToTime( MidiTime const & midiTime )
	{
		m_elapsedMilliSeconds = midiTime.getTimeInMilliseconds(getTempo());
	}

	inline void setToTimeByTicks(tick_t ticks)
	{
		m_elapsedMilliSeconds = MidiTime::ticksToMilliseconds(ticks, getTempo());
	}

	inline int getTacts() const
	{
		return currentTact();
	}

	inline int ticksPerTact() const
	{
		return MidiTime::ticksPerTact(m_timeSigModel);
	}

	// Returns the beat position inside the bar, 0-based
	inline int getBeat() const
	{
		return getPlayPos().getBeatWithinBar(m_timeSigModel);
	}
	// the remainder after bar and beat are removed
	inline int getBeatTicks() const
	{
		return getPlayPos().getTickWithinBeat(m_timeSigModel);
	}
	inline int getTicks() const
	{
		return currentTick();
	}
	inline f_cnt_t getFrames() const
	{
		return currentFrame();
	}
	inline bool isPaused() const
	{
		return m_paused;
	}

	inline bool isPlaying() const
	{
		return m_playing == true && m_exporting == false;
	}

	inline bool isStopped() const
	{
		return m_playing == false && m_paused == false;
	}

	inline bool isExporting() const
	{
		return m_exporting;
	}

	inline void setExportLoop( bool exportLoop )
	{
		m_exportLoop = exportLoop;
	}

	inline bool isRecording() const
	{
		return m_recording;
	}

	bool isExportDone() const;
	std::pair<MidiTime, MidiTime> getExportEndpoints() const;

	inline void setRenderBetweenMarkers( bool renderBetweenMarkers )
	{
		m_renderBetweenMarkers = renderBetweenMarkers;
	}

	inline bool peakNormalizeFlag() const
	{
		return m_peakNormalizeFlag;
	}

	inline void setPeakNormalizeFlag( bool peakNormalizeFlag )
	{
		m_peakNormalizeFlag = peakNormalizeFlag;
	}

	inline PlayModes playMode() const
	{
		return m_playMode;
	}

	inline PlayPos & getPlayPos( PlayModes pm )
	{
		return m_playPos[pm];
	}
	inline const PlayPos & getPlayPos( PlayModes pm ) const
	{
		return m_playPos[pm];
	}
	inline const PlayPos & getPlayPos() const
	{
		return getPlayPos(m_playMode);
	}

	void updateLength();
	tact_t length() const
	{
		return m_length;
	}


	bpm_t getTempo();
	virtual AutomationPattern * tempoAutomationPattern();

	AutomationTrack * globalAutomationTrack()
	{
		return m_globalAutomationTrack;
	}

	//TODO: Add Q_DECL_OVERRIDE when Qt4 is dropped
	AutomatedValueMap automatedValuesAt(MidiTime time, int tcoNum = -1) const;

	// file management
	void createNewProject();
	void createNewProjectFromTemplate( const QString & templ );
	void loadProject( const QString & filename );
	bool guiSaveProject();
	bool guiSaveProjectAs( const QString & filename );
	bool saveProjectFile( const QString & filename );

        QString projectDir();

	const QString & projectFileName() const
	{
		return m_fileName;
	}

	bool isLoadingProject() const
	{
		return m_loadingProject;
	}

       void loadingCancelled()
       {
               m_isCancelled = true;
               Engine::mixer()->clearNewPlayHandles();
       }

       bool isCancelled()
       {
               return m_isCancelled;
       }

	bool isSavingProject() const
	{
		return m_savingProject;
	}

	bool isModified() const
	{
		return m_modified;
	}

	virtual QString nodeName() const
	{
		return "song";
	}

	virtual bool isFixed() const //fixedTCOs()
	{
		return false;
	}

	void addController( Controller * c );
	void removeController( Controller * c );

	const ControllerVector & controllers() const
	{
		return m_controllers;
	}


	MeterModel & getTimeSigModel()
	{
		return m_timeSigModel;
	}

	QString songMetaData(const QString& k);
	void setSongMetaData(const QString& k,const QString& v);

	inline QString songStructure() { return songMetaData("Structure"); }
	inline void setSongStructure(const QString& s) { setSongMetaData("Structure",s); }

	virtual f_cnt_t transportPosition();
	virtual void transportStart();
	virtual void transportStop();
	virtual void transportLocate(f_cnt_t _frame);

public slots:
	void playSong();
	void record();
	void playAndRecord();
	void playBB();
	void playPattern( const Pattern * patternToPlay, bool loop = true );
	void togglePause();
	void stop();

        void freezeTracks();
	void importProject();
	void exportProject( bool multiExport = false );
	void exportProjectTracks();
	void exportProjectMidi();
        void exportProjectVideoLine();

	void startExport();
	void stopExport();

	void setModified();
	void clearProject();

	void addInstrumentTrack();
	void addBBTrack();
	void addSampleTrack();
	void addAutomationTrack();


 protected:
	void clearSongMetaData();
        bool createProjectTree();


private slots:
	void insertBar();
	void removeBar();

	void setTempo();
	void setTimeSignature();

	void masterVolumeChanged();

	void savePos();

	void updateFramesPerTick();



private:
	Song();
	Song( const Song & );
	virtual ~Song();


	inline tact_t currentTact() const
	{
		return m_playPos[m_playMode].getTact();
	}

	inline tick_t currentTick() const
	{
		return m_playPos[m_playMode].getTicks();
	}

	inline f_cnt_t currentFrame() const
	{
		return m_playPos[m_playMode].getTicks() * Engine::framesPerTick() +
			m_playPos[m_playMode].currentFrame();
	}

	void setPlayPos( tick_t ticks, PlayModes playMode );

	void saveControllerStates( QDomDocument & doc, QDomElement & element );
	void restoreControllerStates( const QDomElement & element );

	void removeAllControllers();

	void processAutomations(const TrackList& tracks, MidiTime timeStart, fpp_t frames);

	AutomationTrack * m_globalAutomationTrack;

	IntModel m_tempoModel;
	MeterModel m_timeSigModel;
	int m_oldTicksPerTact;
	//IntModel m_masterVolumeModel;
        FloatModel m_masterVolumeModel;
	//IntModel m_masterPitchModel;
        FloatModel m_masterPitchModel;

	MetaData m_metaData;

	ControllerVector m_controllers;

	int m_nLoadingTrack;

	QString m_fileName;
	QString m_oldFileName;
	bool m_modified;
	bool m_loadOnLaunch;

	volatile bool m_recording;
	volatile bool m_exporting;
	volatile bool m_exportLoop;
	volatile bool m_renderBetweenMarkers;
	volatile bool m_peakNormalizeFlag;
	volatile bool m_playing;
	volatile bool m_paused;

	bool m_loadingProject;
        bool m_isCancelled;
        bool m_savingProject;

	QStringList m_errors;

	PlayModes m_playMode;
	PlayPos m_playPos[Mode_Count];
	tact_t m_length;

	const Pattern* m_patternToPlay;
	bool m_loopPattern;

	double m_elapsedMilliSeconds;
	tick_t m_elapsedTicks;
	tact_t m_elapsedTacts;

	VstSyncController m_vstSyncController;


	friend class LmmsCore;
	friend class SongEditor;
	friend class mainWindow;
	friend class ControllerRackView;

signals:
	void projectLoaded();
	void playbackStateChanged();
	void playbackPositionChanged();
	void lengthChanged( int tacts );
	void tempoChanged( bpm_t newBPM );
	void timeSignatureChanged( int oldTicksPerTact, int ticksPerTact );
	void metaDataChanged( const QString k="*", const QString& v="*" );
	void controllerAdded( Controller * );
	void controllerRemoved( Controller * );
	void updateSampleTracks();
} ;


#endif
