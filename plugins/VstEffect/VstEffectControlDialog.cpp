/*
 * VstEffectControlDialog.cpp - dialog for displaying VST-effect GUI
 *
 * Copyright (c) 2006-2011 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <QLayout>
#include <QMdiArea>
#include <QMenu>
#include <QPushButton>

#include "VstEffectControlDialog.h"
#include "VstEffect.h"

#include "PixmapButton.h"
#include "embed.h"
#include "ToolTip.h"

#include <QObject>
#include <QPainter>
#include "gui_templates.h"
#include <QToolBar>
#include <QLabel>


VstEffectControlDialog::VstEffectControlDialog( VstEffectControls * _ctl ) :
	EffectControlDialog( _ctl ),
	//m_pluginWidget( NULL ),
	m_plugin( NULL )//,
	//tbLabel( NULL )
{
	if( _ctl != NULL && _ctl->m_effect != NULL &&
	    _ctl->m_effect->m_plugin != NULL )
	{
		m_plugin = _ctl->m_effect->m_plugin;
		m_plugin->showEditor(true,this);
		//m_pluginWidget = m_plugin->pluginWidget();

		//#ifdef LMMS_BUILD_WIN32
		//if( !m_pluginWidget )
		//	m_pluginWidget = m_plugin->pluginWidget();
		//}
		//#endif
	}

	if( !m_plugin )
	{
		qWarning("VstEffectControlDialog::VstEffectControlDialog() m_plugin is null");
		return;
	}

	QWidget * pw=m_plugin->pluginWidget();

	if( !pw )
	{
		qWarning("VstEffectControlDialog::VstEffectControlDialog() pluginWidget() is null");
		return;
	}

	setWindowTitle( pw->windowTitle() );
	setMinimumHeight( 54 );
	setMinimumWidth( 150 );

	QToolBar * tb = new QToolBar( this );
	tb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	tb->setStyleSheet( "spacing: 2px;" );

	// Show/hide

	PixmapButton * btn = new PixmapButton( tb, "" );//new QPushButton( tr( "Show/hide" ) );
	btn->setCheckable( false );
	//btn->setChecked( true );
	pw->show();
	btn->setCursor( Qt::PointingHandCursor );
	btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap("zoom",16,16) );
	btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap("zoom",16,16) );
	connect( btn, SIGNAL( clicked() ),
		 this, SLOT( toggleVstWidget() ) );
		//emit btn->click();

	/*
	btn->setMinimumWidth(20);//78
	btn->setMaximumWidth(20);
	btn->setMinimumHeight(20);//24
	btn->setMaximumHeight(20);
	*/

	// Configure

	m_managePluginButton = new PixmapButton( tb, "" );
	m_managePluginButton->setCheckable( false );
	m_managePluginButton->setCursor( Qt::PointingHandCursor );
	m_managePluginButton->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "controls_active",16,16 ) );
	m_managePluginButton->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "controls",16,16 ) );
	connect( m_managePluginButton, SIGNAL( clicked() ),  _ctl,
		 SLOT( managePlugin() ) );
	ToolTip::add( m_managePluginButton, tr( "Control VST-plugin from LMMS host" ) );
	m_managePluginButton->setWhatsThis
		( tr( "Click here, if you want to control VST-plugin from host." ) );

	/*
	m_managePluginButton->setMinimumWidth(20); //26 );
	m_managePluginButton->setMaximumWidth(20); //26 );
	m_managePluginButton->setMinimumHeight(20);//21 );
	m_managePluginButton->setMaximumHeight(20);//21 );
	*/

	// Open

	m_openPresetButton = new PixmapButton( tb, "" );
	m_openPresetButton->setCheckable( false );
	m_openPresetButton->setCursor( Qt::PointingHandCursor );
	m_openPresetButton->setActiveGraphic( embed::getIconPixmap("project_open",16,16) );
	m_openPresetButton->setInactiveGraphic( embed::getIconPixmap("project_open",16,16) );
	connect( m_openPresetButton, SIGNAL( clicked() ),
		 _ctl, SLOT( openPreset() ) );
	ToolTip::add( m_openPresetButton, tr( "Open VST-plugin preset" ) );
	m_openPresetButton->setWhatsThis
		( tr( "Click here, if you want to open another *.fxp, *.fxb VST-plugin preset." ) );

	/*
	m_openPresetButton->setMinimumWidth(20);
	m_openPresetButton->setMaximumWidth(20);
	m_openPresetButton->setMinimumHeight(20);
	m_openPresetButton->setMaximumHeight(20);
	*/
	// Previous

	m_rolLPresetButton = new PixmapButton( tb, "" );
	m_rolLPresetButton->setCheckable( false );
	m_rolLPresetButton->setCursor( Qt::PointingHandCursor );
	m_rolLPresetButton->setActiveGraphic( embed::getIconPixmap("stepper-left-press",16,16));
	m_rolLPresetButton->setInactiveGraphic( embed::getIconPixmap("stepper-left",16,16));
	connect( m_rolLPresetButton, SIGNAL( clicked() ),
		 _ctl,	SLOT( rolrPreset() ) );
	connect( m_rolLPresetButton, SIGNAL( clicked() ),
		 this, SLOT( update() ) );
	ToolTip::add( m_rolLPresetButton, tr( "Previous (-)" ) );
	m_rolLPresetButton->setShortcut( Qt::Key_Minus );
	m_rolLPresetButton->setWhatsThis
		( tr( "Click here, if you want to switch to another VST-plugin preset program." ) );

	/*
	m_rolLPresetButton->setMinimumWidth(20);
	m_rolLPresetButton->setMaximumWidth(20);
	m_rolLPresetButton->setMinimumHeight(20);
	m_rolLPresetButton->setMaximumHeight(20);
	*/

	// Next

	m_rolRPresetButton = new PixmapButton( tb, "" );
	m_rolRPresetButton->setCheckable( false );
	m_rolRPresetButton->setCursor( Qt::PointingHandCursor );
	m_rolRPresetButton->setActiveGraphic( embed::getIconPixmap("stepper-right-press",16,16) );
	m_rolRPresetButton->setInactiveGraphic( embed::getIconPixmap("stepper-right",16,16) );
	connect( m_rolRPresetButton, SIGNAL( clicked() ),
		 _ctl, SLOT( rollPreset() ) );
	connect( m_rolRPresetButton, SIGNAL( clicked() ),
		 this, SLOT( update() ) );
	ToolTip::add( m_rolRPresetButton, tr( "Next (+)" ) );
	m_rolRPresetButton->setShortcut( Qt::Key_Plus );
	m_rolRPresetButton->setWhatsThis
		( tr( "Click here, if you want to switch to another VST-plugin preset program." ) );

	/*
	m_rolRPresetButton->setMinimumWidth(20);
	m_rolRPresetButton->setMaximumWidth(20);
	m_rolRPresetButton->setMinimumHeight(20);
	m_rolRPresetButton->setMaximumHeight(20);
	*/

	// Preset Selection

	_ctl->m_selPresetButton = new PixmapButton( tb,"" );//QPushButton( tr( "" ), l )
	_ctl->m_selPresetButton->setCheckable( false );
	_ctl->m_selPresetButton->setCursor( Qt::PointingHandCursor );
	//_ctl->m_selPresetButton->setIcon( embed::getIconPixmap( "stepper-down" ) );
	_ctl->m_selPresetButton->setActiveGraphic( embed::getIconPixmap( "stepper-down",16,16) );
	_ctl->m_selPresetButton->setInactiveGraphic( embed::getIconPixmap( "stepper-down",16,16) );
	_ctl->m_selPresetButton->setWhatsThis
		( tr( "Click here to select presets that are currently loaded in VST." ) );
	_ctl->m_selPresetButton->setMenu(_ctl->menu);

	/*
	_ctl->m_selPresetButton->setMinimumWidth(20);
	_ctl->m_selPresetButton->setMaximumWidth(20);
	_ctl->m_selPresetButton->setMinimumHeight(20);
	_ctl->m_selPresetButton->setMaximumHeight(20);
	*/

	// Preset Save

	m_savePresetButton = new PixmapButton( tb, "" );
	m_savePresetButton->setCheckable( false );
	m_savePresetButton->setCursor( Qt::PointingHandCursor );
	m_savePresetButton->setActiveGraphic( embed::getIconPixmap("project_save",16,16) );
	m_savePresetButton->setInactiveGraphic( embed::getIconPixmap("project_save",16,16) );//,21,21
	connect( m_savePresetButton, SIGNAL( clicked() ), _ctl,	SLOT( savePreset() ) );
	ToolTip::add( m_savePresetButton, tr( "Save preset" ) );
	m_savePresetButton->setWhatsThis
		( tr( "Click here, if you want to save current VST-plugin preset program." ) );

	/*
	m_savePresetButton->setMinimumWidth(20);//21 );
	m_savePresetButton->setMaximumWidth(20);//21 );
	m_savePresetButton->setMinimumHeight(20);//21 );
	m_savePresetButton->setMaximumHeight(20);//21 );
	*/

	/*
	  int newSize = pw->width() + 20;
	  newSize = (newSize < 250) ? 250 : newSize;
	  QWidget* resize = new QWidget(this);
	  resize->resize( newSize, 10 );
	  QWidget* space0 = new QWidget(this);
	  space0->resize(8, 10);
	  QWidget* space1 = new QWidget(this);
	  space1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	*/

	QFont f( "Arial", 10 );

	//tb->resize( newSize , 32 );
	//tb->addWidget(space0);
	tb->addWidget( m_rolLPresetButton );
	tb->addWidget( m_rolRPresetButton );
	tb->addWidget( _ctl->m_selPresetButton );
	tb->addWidget( m_openPresetButton );
	tb->addWidget( m_savePresetButton );
	tb->addWidget( m_managePluginButton );
	tb->addWidget( btn );
	//tb->addWidget(space1);

	QString txLabel=//tr( "Effect by: " ) +
		m_plugin->vendorString() + " [" +
		m_plugin->currentProgramName() +"]";
	QLabel* tbLabel = new QLabel( txLabel, tb );
	tbLabel->setFont( pointSize<7>( f ) );
	tbLabel->setTextFormat(Qt::RichText);
	tbLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );

	//QGridLayout * l = new QGridLayout( this );
	QVBoxLayout * l = new QVBoxLayout( this );
	l->setContentsMargins( 6,6,6,6 );// 10, 10, 10, 10 );
	//l->setVerticalSpacing( 6 );//2 );
	//l->setHorizontalSpacing( 6 );//2 );
	l->setSpacing( 6 );

	l->addWidget(tb);
	l->addWidget( tbLabel );
	//l->addItem( new QSpacerItem( newSize - 20, 30, QSizePolicy::Fixed,
	//			QSizePolicy::Fixed ), 1, 0 );
	//l->addWidget( resize, 2, 0, 1, 1, Qt::AlignCenter );
	l->addWidget( pw );//, 3, 0, 1, 1, Qt::AlignCenter );
	//l->setRowStretch( 5, 1 );
	//l->setColumnStretch( 1, 1 );
	//resize(l->sizeHint());

	/*
	QMdiSubWindow * sw=m_plugin->mdiSubWindow();
	if( sw )
	{
		sw->setWindowTitle( windowTitle() );
		setMinimumHeight( 54 +20);
		setMinimumWidth( 150 +20);
		resize(sizeHint());
	}
	*/
}

void VstEffectControlDialog::toggleVstWidget()
{
	QWidget * pw=m_plugin->pluginWidget();

	if( !pw )
	{
		qWarning("VstEffectControlDialog::toggleVstWidget() pluginWidget() is null");
		return;
	}

	//QSize sz;
	/*
	QWidget* sw=pw;
	while((sw != nullptr)&&(dynamic_cast<SubWindow*>(sw) == nullptr))
		sw=sw->parentWidget();
	*/

	if(pw->isHidden())
	{
		pw->show();
		//sz=sizeHint();
	}
	else
	{
		pw->hide();
		//sz=minimumSize();
	}

	//qWarning("VstEffectControlDialog::toggleVstWidget size: %d x %d",sz.width(),sz.height());
	//resize(sz);
}



void VstEffectControlDialog::paintEvent( QPaintEvent * )
{
	/*
	if( m_plugin != NULL && tbLabel != NULL )
	{
		tbLabel->setText( tr( "Effect by: " ) + m_plugin->vendorString() +
			tr( "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br />" ) +
			m_plugin->currentProgramName() );
	}
	*/
}




VstEffectControlDialog::~VstEffectControlDialog()
{
	//delete m_pluginWidget;
}

