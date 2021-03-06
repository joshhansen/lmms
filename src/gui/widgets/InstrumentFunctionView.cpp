/*
 * InstrumentFunctionViews.cpp - view for instrument-functions-tab
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

#include "InstrumentFunctionView.h"

#include "ComboBox.h"
#include "GroupBox.h"
#include "InstrumentFunction.h"
#include "Knob.h"
#include "LcdSpinBox.h"
#include "LedCheckBox.h"
#include "Note.h"
#include "TempoSyncKnob.h"

#include <QHBoxLayout>
#include <QLabel>
//#include "ToolTip.h"
#include "gui_templates.h"

InstrumentFunctionView::InstrumentFunctionView(InstrumentFunction* cc,
                                               const QString&      _caption,
                                               QWidget*            _parent,
                                               bool                _arrow,
                                               bool                _panel) :
      GroupBox(_caption, _parent, true, _arrow, _panel),
      //      QWidget(_parent),
      ModelView(NULL, this)
//, m_groupBox(new GroupBox(_caption))
{
    // m_groupBox->setContentsMargins(0, 6, 0, 0);
    // m_panel = contentWidget();

    // QHBoxLayout* topLayout = new QHBoxLayout(this);
    // topLayout->setContentsMargins(0, 0, 0, 0);
    // topLayout->setSpacing(0);
    // topLayout->addWidget(m_groupBox);

    LcdSpinBox* minLcd = new LcdSpinBox(1, "11blue", this, "min_gen");
    LcdSpinBox* maxLcd = new LcdSpinBox(1, "11blue", this, "max_gen");
    // minLcd->move(186, 0);
    // maxLcd->move(210, 0);
    addTopWidget(minLcd, 3);
    addTopWidget(maxLcd, 4);
    minLcd->setModel(cc->minNoteGenerationModel());
    maxLcd->setModel(cc->maxNoteGenerationModel());
}

InstrumentFunctionView::~InstrumentFunctionView()
{
    // delete m_groupBox;
    // m_groupBox = NULL;
}

InstrumentFunctionNoteStackingView::InstrumentFunctionNoteStackingView(
        InstrumentFunctionNoteStacking* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("STACKING"), parent),
      m_cc(cc), m_chordsComboBox(new ComboBox()),
      m_chordRangeKnob(new Knob(knobBright_26))
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    QLabel* chordLabel = new QLabel(tr("Chord:"));
    chordLabel->setFont(pointSize<8>(chordLabel->font()));

    m_chordRangeKnob->setLabel(tr("RANGE"));
    m_chordRangeKnob->setHintText(tr("Chord range:"), " " + tr("octave(s)"));
    m_chordRangeKnob->setWhatsThis(
            tr("Use this knob for setting the chord range in octaves. "
               "The selected chord will be played within specified "
               "number of octaves."));

    mainLayout->addWidget(chordLabel, 0, 0);
    mainLayout->addWidget(m_chordsComboBox, 1, 0);
    mainLayout->addWidget(m_chordRangeKnob, 0, 1, 2, 1, Qt::AlignHCenter);
}

InstrumentFunctionNoteStackingView::~InstrumentFunctionNoteStackingView()
{
}

void InstrumentFunctionNoteStackingView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionNoteStacking>();
    ledButton()->setModel(&m_cc->m_enabledModel);
    m_chordsComboBox->setModel(&m_cc->m_chordsModel);
    m_chordRangeKnob->setModel(&m_cc->m_chordRangeModel);
}

InstrumentFunctionArpeggioView::InstrumentFunctionArpeggioView(
        InstrumentFunctionArpeggio* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("ARPEGGIO"), parent),
      m_cc(cc), m_arpComboBox(new ComboBox()),
      m_arpRangeKnob(new Knob(knobBright_26)),
      m_arpCycleKnob(new Knob(knobBright_26)),
      m_arpSkipKnob(new Knob(knobBright_26)),
      m_arpMissKnob(new Knob(knobBright_26)),
      m_arpTimeKnob(new TempoSyncKnob(knobBright_26)),
      m_arpGateKnob(new Knob(knobBright_26)),
      m_arpDirectionComboBox(new ComboBox()),
      m_arpModeComboBox(new ComboBox())
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    /*m_groupBox->*/ setWhatsThis(
            tr("An arpeggio is a method playing (especially plucked) "
               "instruments, which makes the music much livelier. "
               "The strings of such instruments (e.g. harps) are "
               "plucked like chords. The only difference is that "
               "this is done in a sequential order, so the notes are "
               "not played at the same time. Typical arpeggios are "
               "major or minor triads, but there are a lot of other "
               "possible chords, you can select."));

    m_arpRangeKnob->setLabel(tr("RANGE"));
    m_arpRangeKnob->setHintText(tr("Arpeggio range:"), " " + tr("octave(s)"));
    m_arpRangeKnob->setWhatsThis(
            tr("Use this knob for setting the arpeggio range in octaves. "
               "The selected arpeggio will be played within specified "
               "number of octaves."));

    m_arpCycleKnob->setLabel(tr("CYCLE"));
    m_arpCycleKnob->setHintText(tr("Cycle notes:") + " ",
                                " " + tr("note(s)"));
    m_arpCycleKnob->setWhatsThis(tr(
            "Jumps over n steps in the arpeggio and cycles around "
            "if we're over the note range. If the total note range is evenly "
            "divisible by the number of steps jumped over you will get stuck "
            "in a shorter arpeggio or even on one note."));

    m_arpSkipKnob->setLabel(tr("SKIP"));
    m_arpSkipKnob->setHintText(tr("Skip rate:"), tr("%"));
    m_arpSkipKnob->setWhatsThis(
            tr("The skip function will make the arpeggiator pause one step "
               "randomly. From its start in full counter clockwise "
               "position and no effect it will gradually progress to "
               "full amnesia at maximum setting."));

    m_arpMissKnob->setLabel(tr("MISS"));
    m_arpMissKnob->setHintText(tr("Miss rate:"), tr("%"));
    m_arpMissKnob->setWhatsThis(
            tr("The miss function will make the arpeggiator miss the "
               "intended note."));

    m_arpTimeKnob->setLabel(tr("TIME"));
    m_arpTimeKnob->setHintText(tr("Arpeggio time:"), " " + tr("ms"));
    m_arpTimeKnob->setWhatsThis(
            tr("Use this knob for setting the arpeggio time in "
               "milliseconds. The arpeggio time specifies how long "
               "each arpeggio-tone should be played."));

    m_arpGateKnob->setLabel(tr("GATE"));
    m_arpGateKnob->setHintText(tr("Arpeggio gate:"), tr("%"));
    m_arpGateKnob->setWhatsThis(
            tr("Use this knob for setting the arpeggio gate. The "
               "arpeggio gate specifies the percent of a whole "
               "arpeggio-tone that should be played. With this you "
               "can make cool staccato arpeggios."));

    QLabel* arpChordLabel = new QLabel(tr("Chord:"));
    arpChordLabel->setFont(pointSize<8>(arpChordLabel->font()));

    QLabel* arpDirectionLabel = new QLabel(tr("Direction:"));
    arpDirectionLabel->setFont(pointSize<8>(arpDirectionLabel->font()));

    QLabel* arpModeLabel = new QLabel(tr("Mode:"));
    arpModeLabel->setFont(pointSize<8>(arpModeLabel->font()));

    mainLayout->addWidget(arpChordLabel, 0, 0);
    mainLayout->addWidget(m_arpComboBox, 1, 0);
    mainLayout->addWidget(arpDirectionLabel, 3, 0);
    mainLayout->addWidget(m_arpDirectionComboBox, 4, 0);
    mainLayout->addWidget(arpModeLabel, 6, 0);
    mainLayout->addWidget(m_arpModeComboBox, 7, 0);

    mainLayout->addWidget(m_arpRangeKnob, 0, 1, 2, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_arpCycleKnob, 0, 2, 2, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_arpSkipKnob, 3, 1, 2, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_arpMissKnob, 3, 2, 2, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_arpGateKnob, 6, 1, 2, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_arpTimeKnob, 6, 2, 2, 1, Qt::AlignHCenter);

    mainLayout->setRowMinimumHeight(2, 3);
    mainLayout->setRowMinimumHeight(5, 3);
}

InstrumentFunctionArpeggioView::~InstrumentFunctionArpeggioView()
{
}

void InstrumentFunctionArpeggioView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionArpeggio>();
    ledButton()->setModel(&m_cc->m_enabledModel);
    m_arpComboBox->setModel(&m_cc->m_arpModel);
    m_arpRangeKnob->setModel(&m_cc->m_arpRangeModel);
    m_arpCycleKnob->setModel(&m_cc->m_arpCycleModel);
    m_arpSkipKnob->setModel(&m_cc->m_arpSkipModel);
    m_arpMissKnob->setModel(&m_cc->m_arpMissModel);
    m_arpTimeKnob->setModel(&m_cc->m_arpTimeModel);
    m_arpGateKnob->setModel(&m_cc->m_arpGateModel);
    m_arpDirectionComboBox->setModel(&m_cc->m_arpDirectionModel);
    m_arpModeComboBox->setModel(&m_cc->m_arpModeModel);
}

InstrumentFunctionNoteHumanizingView::InstrumentFunctionNoteHumanizingView(
        InstrumentFunctionNoteHumanizing* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("HUMANIZING"), parent),
      m_cc(cc), m_volumeRangeKnob(new Knob(knobBright_26)),
      m_panRangeKnob(new Knob(knobBright_26)),
      m_tuneRangeKnob(new Knob(knobBright_26)),
      m_offsetRangeKnob(new Knob(knobBright_26)),
      m_shortenRangeKnob(new Knob(knobBright_26))
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(5, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    m_volumeRangeKnob->setLabel(tr("VOL%"));
    m_volumeRangeKnob->setHintText(tr("Volume change:"), "" + tr("%"));
    m_volumeRangeKnob->setWhatsThis(
            tr("Use this knob for setting the volume change in %."));

    m_panRangeKnob->setLabel(tr("PAN%"));
    m_panRangeKnob->setHintText(tr("Pan change:"), "" + tr("%"));
    m_panRangeKnob->setWhatsThis(
            tr("Use this knob for setting the pan change in %."));

    m_tuneRangeKnob->setLabel(tr("PITCH%"));
    m_tuneRangeKnob->setHintText(tr("Pitch change:"), "" + tr("%"));
    m_tuneRangeKnob->setWhatsThis(
            tr("Use this knob for setting the tune change in %."));

    m_offsetRangeKnob->setLabel(tr("DELAY%"));
    m_offsetRangeKnob->setHintText(tr("Start delay:"), "" + tr("%"));
    m_offsetRangeKnob->setWhatsThis(
            tr("Use this knob for setting the delay in %."));

    m_shortenRangeKnob->setLabel(tr("SHORT%"));
    m_shortenRangeKnob->setHintText(tr("Shortening:"), "" + tr("%"));
    m_shortenRangeKnob->setWhatsThis(tr("Use this knob for shortening a %."));

    mainLayout->addWidget(m_volumeRangeKnob, 0, 0, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_panRangeKnob, 0, 1, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_tuneRangeKnob, 0, 2, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_offsetRangeKnob, 0, 3, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_shortenRangeKnob, 0, 4, 1, 1, Qt::AlignHCenter);
}

InstrumentFunctionNoteHumanizingView::~InstrumentFunctionNoteHumanizingView()
{
}

void InstrumentFunctionNoteHumanizingView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionNoteHumanizing>();
    ledButton()->setModel(&m_cc->m_enabledModel);
    m_volumeRangeKnob->setModel(&m_cc->m_volumeRangeModel);
    m_panRangeKnob->setModel(&m_cc->m_panRangeModel);
    m_tuneRangeKnob->setModel(&m_cc->m_tuneRangeModel);
    m_offsetRangeKnob->setModel(&m_cc->m_offsetRangeModel);
    m_shortenRangeKnob->setModel(&m_cc->m_shortenRangeModel);
}

InstrumentFunctionNoteDuplicatesRemovingView::
        InstrumentFunctionNoteDuplicatesRemovingView(
                InstrumentFunctionNoteDuplicatesRemoving* cc,
                QWidget*                                  parent) :
      InstrumentFunctionView(
              cc, tr("DUPLICATES REMOVING"), parent, false, false),
      m_cc(cc)
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);
}

InstrumentFunctionNoteDuplicatesRemovingView::
        ~InstrumentFunctionNoteDuplicatesRemovingView()
{
}

void InstrumentFunctionNoteDuplicatesRemovingView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionNoteDuplicatesRemoving>();
    ledButton()->setModel(&m_cc->m_enabledModel);
}

InstrumentFunctionNoteFilteringView::InstrumentFunctionNoteFilteringView(
        InstrumentFunctionNoteFiltering* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("FILTERING"), parent),
      m_cc(cc), m_configComboBox(new ComboBox()),
      m_actionComboBox(new ComboBox())
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(5, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    mainLayout->addWidget(m_configComboBox, 0, 0, 1, 2);
    mainLayout->addWidget(m_actionComboBox, 0, 2, 1, 2);

    for(int i = 0; i < 12; ++i)
    {
        m_noteSelectionLed[i] = new LedCheckBox(
                Note::findKeyName(i).replace("-1", ""), NULL, "");
        mainLayout->addWidget(m_noteSelectionLed[i], i / 4 + 1, i % 4, 1, 1);
    }
}

InstrumentFunctionNoteFilteringView::~InstrumentFunctionNoteFilteringView()
{
    for(int i = 0; i < 12; ++i)
        delete m_noteSelectionLed[i];
}

void InstrumentFunctionNoteFilteringView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionNoteFiltering>();
    ledButton()->setModel(&m_cc->m_enabledModel);

    ComboBoxModel* old_m = m_configComboBox->model();
    if(old_m != &m_cc->m_configModel)
    {
        if(old_m)
            disconnect(old_m, SIGNAL(dataChanged()), this,
                       SLOT(modelChanged()));
        m_configComboBox->setModel(&m_cc->m_configModel);
        connect(&m_cc->m_configModel, SIGNAL(dataChanged()), this,
                SLOT(modelChanged()));
    }

    const int j = m_cc->m_configModel.value();
    m_actionComboBox->setModel(m_cc->m_actionModel[j]);
    for(int i = 0; i < 12; ++i)
        m_noteSelectionLed[i]->setModel(m_cc->m_noteSelectionModel[j][i]);
}

InstrumentFunctionNoteKeyingView::InstrumentFunctionNoteKeyingView(
        InstrumentFunctionNoteKeying* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("KEYING"), parent),
      m_cc(cc), m_volumeRangeKnob(new Knob(knobBright_26)),
      m_volumeBaseKnob(new Knob(knobBright_26)),
      m_volumeMinKnob(new Knob(knobBright_26)),
      m_volumeMaxKnob(new Knob(knobBright_26)),
      m_panRangeKnob(new Knob(knobBright_26)),
      m_panBaseKnob(new Knob(knobBright_26)),
      m_panMinKnob(new Knob(knobBright_26)),
      m_panMaxKnob(new Knob(knobBright_26))
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(5, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    m_volumeRangeKnob->setPointColor(Qt::red);
    m_volumeRangeKnob->setLabel(tr("VOL%"));
    m_volumeRangeKnob->setHintText(tr("Volume change:"), "" + tr("%"));
    m_volumeRangeKnob->setWhatsThis(
            tr("Use this knob for setting the volume change in %."));

    m_panRangeKnob->setPointColor(Qt::magenta);
    m_panRangeKnob->setLabel(tr("PAN%"));
    m_panRangeKnob->setHintText(tr("Pan change:"), "" + tr("%"));
    m_panRangeKnob->setWhatsThis(
            tr("Use this knob for setting the pan change in %."));

    m_volumeBaseKnob->setLabel(tr("BASE"));
    m_volumeMinKnob->setLabel(tr("MIN"));
    m_volumeMaxKnob->setLabel(tr("MAX"));

    m_panBaseKnob->setLabel(tr("BASE"));
    m_panMinKnob->setLabel(tr("MIN"));
    m_panMaxKnob->setLabel(tr("MAX"));

    mainLayout->addWidget(m_volumeRangeKnob, 0, 0, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_volumeBaseKnob, 0, 1, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_volumeMinKnob, 0, 2, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_volumeMaxKnob, 0, 3, 1, 1, Qt::AlignHCenter);

    mainLayout->addWidget(m_panRangeKnob, 1, 0, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_panBaseKnob, 1, 1, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_panMinKnob, 1, 2, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_panMaxKnob, 1, 3, 1, 1, Qt::AlignHCenter);
}

InstrumentFunctionNoteKeyingView::~InstrumentFunctionNoteKeyingView()
{
}

void InstrumentFunctionNoteKeyingView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionNoteKeying>();
    ledButton()->setModel(&m_cc->m_enabledModel);

    m_volumeRangeKnob->setModel(&m_cc->m_volumeRangeModel);
    m_volumeBaseKnob->setModel(&m_cc->m_volumeBaseModel);
    m_volumeMinKnob->setModel(&m_cc->m_volumeMinModel);
    m_volumeMaxKnob->setModel(&m_cc->m_volumeMaxModel);

    m_panRangeKnob->setModel(&m_cc->m_panRangeModel);
    m_panBaseKnob->setModel(&m_cc->m_panBaseModel);
    m_panMinKnob->setModel(&m_cc->m_panMinModel);
    m_panMaxKnob->setModel(&m_cc->m_panMaxModel);
}

InstrumentFunctionNoteOuttingView::InstrumentFunctionNoteOuttingView(
        InstrumentFunctionNoteOutting* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("OUTTING"), parent),
      m_cc(cc), m_volumeKnob(new Knob(knobBright_26)),
      m_panKnob(new Knob(knobBright_26)), m_keyKnob(new Knob(knobBright_26)),
      m_noteKnob(new Knob(knobBright_26))
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(5, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    m_volumeKnob->setLabel(tr("VOL"));
    m_panKnob->setLabel(tr("PAN"));
    m_keyKnob->setLabel(tr("KEY"));
    m_noteKnob->setLabel(tr("NOTE"));

    m_volumeKnob->setPointColor(Qt::red);
    m_panKnob->setPointColor(Qt::magenta);
    m_keyKnob->setPointColor(Qt::cyan);
    m_noteKnob->setPointColor(Qt::darkCyan);

    mainLayout->addWidget(m_volumeKnob, 0, 0, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_panKnob, 0, 1, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_keyKnob, 0, 2, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_noteKnob, 0, 3, 1, 1, Qt::AlignHCenter);
}

InstrumentFunctionNoteOuttingView::~InstrumentFunctionNoteOuttingView()
{
}

void InstrumentFunctionNoteOuttingView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionNoteOutting>();
    ledButton()->setModel(&m_cc->m_enabledModel);

    m_volumeKnob->setModel(&m_cc->m_volumeModel);
    m_panKnob->setModel(&m_cc->m_panModel);
    m_keyKnob->setModel(&m_cc->m_keyModel);
    m_noteKnob->setModel(&m_cc->m_noteModel);
}

InstrumentFunctionGlissandoView::InstrumentFunctionGlissandoView(
        InstrumentFunctionGlissando* cc, QWidget* parent) :
      InstrumentFunctionView(cc, tr("GLISSANDO"), parent),
      m_cc(cc), m_gliTimeKnob(new TempoSyncKnob(knobBright_26)),
      m_gliGateKnob(new Knob(knobBright_26)),
      m_gliAttenuationKnob(new Knob(knobBright_26)),
      m_gliUpModeComboBox(new ComboBox()),
      m_gliDownModeComboBox(new ComboBox())
{
    QGridLayout* mainLayout = new QGridLayout(m_panel);
    mainLayout->setContentsMargins(6, 2, 2, 2);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(1);

    /*m_groupBox->*/ setWhatsThis(
            tr("A glissando is a method playing all, only black or only "
               "white keys between two notes."));

    m_gliTimeKnob->setLabel(tr("TIME"));
    m_gliTimeKnob->setHintText(tr("Glissando time:"), " " + tr("ms"));
    m_gliTimeKnob->setWhatsThis(
            tr("Use this knob for setting the glissando time in "
               "milliseconds. The glissando time specifies how long "
               "each glissando note should be played."));

    m_gliGateKnob->setLabel(tr("GATE"));
    m_gliGateKnob->setHintText(tr("Glissando gate:"), tr("%"));
    m_gliGateKnob->setWhatsThis(
            tr("Use this knob for setting the glissando gate. The "
               "glissando gate specifies the percent of a whole "
               "glissando note that should be played. With this you "
               "can make cool staccato glissandos."));

    m_gliAttenuationKnob->setLabel(tr("MUT%"));
    m_gliAttenuationKnob->setHintText(tr("Attenuation:"), tr("%"));
    m_gliAttenuationKnob->setWhatsThis(
            tr("Use this knob for setting the volume reduction. The "
               "glissando gate specifies the percent of a whole "
               "glissando note that should be played. With this you "
               "can make cool staccato glissandos."));

    QLabel* upModeLabel   = new QLabel(tr("Up mode:"));
    QLabel* downModeLabel = new QLabel(tr("Down mode:"));
    upModeLabel->setFont(pointSize<8>(upModeLabel->font()));
    downModeLabel->setFont(pointSize<8>(downModeLabel->font()));

    mainLayout->addWidget(m_gliGateKnob, 0, 0, 4, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_gliTimeKnob, 0, 1, 4, 1, Qt::AlignHCenter);
    mainLayout->addWidget(m_gliAttenuationKnob, 0, 2, 4, 1, Qt::AlignHCenter);
    mainLayout->addWidget(upModeLabel, 0, 3);
    mainLayout->addWidget(m_gliUpModeComboBox, 1, 3);
    mainLayout->addWidget(downModeLabel, 2, 3);
    mainLayout->addWidget(m_gliDownModeComboBox, 3, 3);

    // mainLayout->setRowMinimumHeight(2, 3);
    // mainLayout->setRowMinimumHeight(5, 3);
}

InstrumentFunctionGlissandoView::~InstrumentFunctionGlissandoView()
{
}

void InstrumentFunctionGlissandoView::modelChanged()
{
    m_cc = castModel<InstrumentFunctionGlissando>();
    ledButton()->setModel(&m_cc->m_enabledModel);
    m_gliTimeKnob->setModel(&m_cc->m_gliTimeModel);
    m_gliGateKnob->setModel(&m_cc->m_gliGateModel);
    m_gliAttenuationKnob->setModel(&m_cc->m_gliAttenuationModel);
    m_gliUpModeComboBox->setModel(&m_cc->m_gliUpModeModel);
    m_gliDownModeComboBox->setModel(&m_cc->m_gliDownModeModel);
}
