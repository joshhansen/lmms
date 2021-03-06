/*
 * OutputGDXDialog.cpp - control dialog for audio output properties
 *
 * Copyright (c) 2018 gi0e5b06 (on github.com)
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

#include "OutputGDXDialog.h"

#include <QGridLayout>

#include "OutputGDXControls.h"
#include "embed.h"

OutputGDXDialog::OutputGDXDialog(OutputGDXControls* controls) :
      EffectControlDialog(controls)
{
    setAutoFillBackground(true);
    QPalette pal;
    pal.setBrush(backgroundRole(), embed::getIconPixmap("plugin_bg"));
    setPalette(pal);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setHorizontalSpacing(6);
    mainLayout->setVerticalSpacing(6);

    Knob* leftKNB = new Knob(this);
    leftKNB->setModel(&controls->m_leftModel);
    leftKNB->setPointColor(Qt::white);
    leftKNB->setLabel(tr("LEFT"));
    leftKNB->setHintText(tr("Left:"), "");

    Knob* rightKNB = new Knob(this);
    rightKNB->setModel(&controls->m_rightModel);
    rightKNB->setPointColor(Qt::white);
    rightKNB->setLabel(tr("RIGHT"));
    rightKNB->setHintText(tr("Right:"), "");

    Knob* rmsKNB = new Knob(this);
    rmsKNB->setModel(&controls->m_rmsModel);
    rmsKNB->setPointColor(Qt::red);
    rmsKNB->setLabel(tr("RMS"));
    rmsKNB->setHintText(tr("Rms:"), "");

    Knob* volKNB = new Knob(this);
    volKNB->setModel(&controls->m_volModel);
    volKNB->setPointColor(Qt::red);
    volKNB->setLabel(tr("VOL"));
    volKNB->setHintText(tr("Vol:"), "");

    Knob* panKNB = new Knob(this);
    panKNB->setModel(&controls->m_panModel);
    panKNB->setPointColor(Qt::magenta);
    panKNB->setLabel(tr("PAN"));
    panKNB->setHintText(tr("Pan:"), "");

    Knob* frequencyKNB = new Knob(this);
    frequencyKNB->setModel(&controls->m_frequencyModel);
    frequencyKNB->setPointColor(Qt::green);
    frequencyKNB->setLabel(tr("FREQ"));
    frequencyKNB->setHintText(tr("Frequency:"), "");

    int col = 0, row = 0;  // first row
    mainLayout->addWidget(leftKNB, row, ++col, 1, 1,
                          Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->addWidget(rightKNB, row, ++col, 1, 1,
                          Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->addWidget(rmsKNB, row, ++col, 1, 1,
                          Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->addWidget(volKNB, row, ++col, 1, 1,
                          Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->addWidget(panKNB, row, ++col, 1, 1,
                          Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->addWidget(frequencyKNB, row, ++col, 1, 1,
                          Qt::AlignBottom | Qt::AlignHCenter);

    setFixedWidth(234);
}
