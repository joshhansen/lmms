/*
 * InstrumentFunction.h - models for instrument-functions-tab
 *
 * Copyright (c) 2017-2018 gi0e5b06
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

#ifndef INSTRUMENT_FUNCTION_H
#define INSTRUMENT_FUNCTION_H

#include <QMultiHash>

//#include "AutomatableModel.h"
#include "ComboBoxModel.h"
//#include "JournallingObject.h"
#include "TempoSyncKnobModel.h"

//#include "lmms_basics.h"

class InstrumentTrack;
class NotePlayHandle;
class InstrumentFunctionView;

class InstrumentFunction
      : public Model
      , public JournallingObject
{
    Q_OBJECT

  public:
    virtual ~InstrumentFunction()
    {
    }

    virtual bool processNote(NotePlayHandle* n)                         = 0;
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent) = 0;
    virtual void loadSettings(const QDomElement& _this)                 = 0;
    virtual QString nodeName() const                                    = 0;

    virtual InstrumentFunctionView* createView() = 0;

    IntModel* minNoteGenerationModel()
    {
        return &m_minNoteGenerationModel;
    }
    IntModel* maxNoteGenerationModel()
    {
        return &m_maxNoteGenerationModel;
    }
    // inline int minNoteGeneration() const { return
    // m_minNoteGenerationModel.value(); } inline void
    // setMinNoteGeneration(int _min) {
    // m_minNoteGenerationModel.setValue(_min); } inline int
    // maxNoteGeneration() const { return m_maxNoteGenerationModel.value(); }
    // inline void setMaxNoteGeneration(int _max) {
    // m_maxNoteGenerationModel.setValue(_max); }

  protected:
    InstrumentFunction(Model* _parent, QString _name);
    virtual bool shouldProcessNote(NotePlayHandle* n);

    BoolModel m_enabledModel;
    IntModel  m_minNoteGenerationModel;
    IntModel  m_maxNoteGenerationModel;
};

class InstrumentFunctionNoteStacking : public InstrumentFunction
{
    Q_OBJECT

  public:
    static const int MAX_CHORD_POLYPHONY = 13;

  private:
    typedef int8_t ChordSemiTones[MAX_CHORD_POLYPHONY];

  public:
    InstrumentFunctionNoteStacking(Model* _parent);
    virtual ~InstrumentFunctionNoteStacking();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "chordcreator";
    }

    virtual InstrumentFunctionView* createView();

    struct Chord
    {
      private:
        QString        m_name;
        ChordSemiTones m_semiTones;
        int            m_size;

      public:
        Chord() : m_size(0)
        {
        }

        Chord(const char* n, const ChordSemiTones& semi_tones);

        int size() const
        {
            return m_size;
        }

        bool isScale() const
        {
            return size() > 6;
        }

        bool isEmpty() const
        {
            return size() == 0;
        }

        bool hasSemiTone(int8_t semiTone) const;

        int8_t last() const
        {
            return m_semiTones[size() - 1];
        }

        const QString& getName() const
        {
            return m_name;
        }

        int8_t operator[](int n) const
        {
            return m_semiTones[n];
        }
    };

    struct ChordTable : public QVector<Chord>
    {
      private:
        ChordTable();

        struct Init
        {
            const char*    m_name;
            ChordSemiTones m_semiTones;
        };

        static Init s_initTable[];

      public:
        static const ChordTable& getInstance()
        {
            static ChordTable inst;
            return inst;
        }

        const Chord& getByName(const QString& name,
                               bool           is_scale = false) const;

        const Chord& getScaleByName(const QString& name) const
        {
            return getByName(name, true);
        }

        const Chord& getChordByName(const QString& name) const
        {
            return getByName(name, false);
        }
    };

  private:
    // BoolModel m_chordsEnabledModel;
    ComboBoxModel m_chordsModel;
    FloatModel    m_chordRangeModel;

    friend class InstrumentFunctionNoteStackingView;
};

class InstrumentFunctionArpeggio : public InstrumentFunction
{
    Q_OBJECT
  public:
    enum ArpDirections
    {
        ArpDirUp,
        ArpDirDown,
        ArpDirUpAndDown,
        ArpDirDownAndUp,
        ArpDirRandom,
        NumArpDirections
    };

    InstrumentFunctionArpeggio(Model* _parent);
    virtual ~InstrumentFunctionArpeggio();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "arpeggiator";
    }

    virtual InstrumentFunctionView* createView();

  private:
    enum ArpModes
    {
        FreeMode,
        SortMode,
        SyncMode
    };

    // BoolModel m_arpEnabledModel;
    ComboBoxModel      m_arpModel;
    FloatModel         m_arpRangeModel;
    FloatModel         m_arpCycleModel;
    FloatModel         m_arpSkipModel;
    FloatModel         m_arpMissModel;
    TempoSyncKnobModel m_arpTimeModel;
    FloatModel         m_arpGateModel;
    ComboBoxModel      m_arpDirectionModel;
    ComboBoxModel      m_arpModeModel;

    friend class InstrumentTrack;
    friend class InstrumentFunctionArpeggioView;
};

class InstrumentFunctionNoteHumanizing : public InstrumentFunction
{
    Q_OBJECT

  public:
    InstrumentFunctionNoteHumanizing(Model* _parent);
    virtual ~InstrumentFunctionNoteHumanizing();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "notehumanizing";
    }

    virtual InstrumentFunctionView* createView();

  private:
    FloatModel m_volumeRangeModel;
    FloatModel m_panRangeModel;
    FloatModel m_tuneRangeModel;
    FloatModel m_offsetRangeModel;
    FloatModel m_shortenRangeModel;

    friend class InstrumentFunctionNoteHumanizingView;
};

class InstrumentFunctionNoteDuplicatesRemoving : public InstrumentFunction
{
    Q_OBJECT

  public:
    InstrumentFunctionNoteDuplicatesRemoving(Model* _parent);
    virtual ~InstrumentFunctionNoteDuplicatesRemoving();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "noteduplicatesremoving";
    }

    virtual InstrumentFunctionView* createView();

  private:
    QMultiHash<int64_t, int> m_cache;

    friend class InstrumentFunctionNoteDuplicatesRemovingView;
};

class InstrumentFunctionNoteFiltering : public InstrumentFunction
{
    Q_OBJECT

  public:
    InstrumentFunctionNoteFiltering(Model* _parent);
    virtual ~InstrumentFunctionNoteFiltering();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "notefiltering";
    }

    virtual InstrumentFunctionView* createView();

  private:
    ComboBoxModel  m_configModel;
    ComboBoxModel* m_actionModel[4];
    BoolModel*     m_noteSelectionModel[4][12];

    friend class InstrumentFunctionNoteFilteringView;
};

class InstrumentFunctionNoteKeying : public InstrumentFunction
{
    Q_OBJECT

  public:
    InstrumentFunctionNoteKeying(Model* _parent);
    virtual ~InstrumentFunctionNoteKeying();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "notekeying";
    }

    virtual InstrumentFunctionView* createView();

  private:
    FloatModel m_volumeRangeModel;
    FloatModel m_volumeBaseModel;
    FloatModel m_volumeMinModel;
    FloatModel m_volumeMaxModel;

    FloatModel m_panRangeModel;
    FloatModel m_panBaseModel;
    FloatModel m_panMinModel;
    FloatModel m_panMaxModel;

    friend class InstrumentFunctionNoteKeyingView;
};

class InstrumentFunctionNoteOutting : public InstrumentFunction
{
    Q_OBJECT

  public:
    InstrumentFunctionNoteOutting(Model* _parent);
    virtual ~InstrumentFunctionNoteOutting();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "noteoutting";
    }

    virtual InstrumentFunctionView* createView();

  private:
    FloatModel m_volumeModel;
    FloatModel m_panModel;
    FloatModel m_keyModel;
    FloatModel m_noteModel;

    friend class InstrumentFunctionNoteOuttingView;
};

class InstrumentFunctionGlissando : public InstrumentFunction
{
    Q_OBJECT

  public:
    InstrumentFunctionGlissando(Model* _parent);
    virtual ~InstrumentFunctionGlissando();

    virtual bool processNote(NotePlayHandle* n);
    virtual void saveSettings(QDomDocument& _doc, QDomElement& _parent);
    virtual void loadSettings(const QDomElement& _this);

    inline virtual QString nodeName() const
    {
        return "glissando";
    }

    virtual InstrumentFunctionView* createView();

  public slots:
    void reset();

  private:
    TempoSyncKnobModel m_gliTimeModel;
    FloatModel         m_gliGateModel;
    FloatModel         m_gliAttenuationModel;
    ComboBoxModel      m_gliUpModeModel;
    ComboBoxModel      m_gliDownModeModel;

    int     m_lastKey;
    int64_t m_lastTime;

    friend class InstrumentTrack;
    friend class InstrumentFunctionGlissandoView;
};

#endif
