///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  6 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx_html_report_panel.h"

#include "dialog_plot_schematic_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_PLOT_SCHEMATIC_BASE::DIALOG_PLOT_SCHEMATIC_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerDir;
	bSizerDir = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextOutputDirectory = new wxStaticText( this, wxID_ANY, _("Output directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOutputDirectory->Wrap( -1 );
	bSizerDir->Add( m_staticTextOutputDirectory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_outputDirectoryName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_outputDirectoryName->SetToolTip( _("Target directory for plot files. Can be absolute or relative to the schematic main file location.") );
	
	bSizerDir->Add( m_outputDirectoryName, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_browseButton = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerDir->Add( m_browseButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bMainSizer->Add( bSizerDir, 0, wxBOTTOM|wxEXPAND, 10 );
	
	m_optionsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_plotFormatOptChoices[] = { _("Postscript"), _("PDF"), _("SVG"), _("DXF"), _("HPGL") };
	int m_plotFormatOptNChoices = sizeof( m_plotFormatOptChoices ) / sizeof( wxString );
	m_plotFormatOpt = new wxRadioBox( this, wxID_ANY, _("Output Format"), wxDefaultPosition, wxDefaultSize, m_plotFormatOptNChoices, m_plotFormatOptChoices, 1, wxRA_SPECIFY_COLS );
	m_plotFormatOpt->SetSelection( 4 );
	m_optionsSizer->Add( m_plotFormatOpt, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_paperOptionsSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Paper Options") ), wxVERTICAL );
	
	wxString m_PaperSizeOptionChoices[] = { _("Schematic size"), _("A4"), _("A") };
	int m_PaperSizeOptionNChoices = sizeof( m_PaperSizeOptionChoices ) / sizeof( wxString );
	m_PaperSizeOption = new wxRadioBox( m_paperOptionsSizer->GetStaticBox(), wxID_ANY, _("Page Size"), wxDefaultPosition, wxDefaultSize, m_PaperSizeOptionNChoices, m_PaperSizeOptionChoices, 1, wxRA_SPECIFY_COLS );
	m_PaperSizeOption->SetSelection( 1 );
	m_paperOptionsSizer->Add( m_PaperSizeOption, 0, wxALL|wxEXPAND, 5 );
	
	m_paperHPGLSizer = new wxStaticBoxSizer( new wxStaticBox( m_paperOptionsSizer->GetStaticBox(), wxID_ANY, _("HPGL Options") ), wxVERTICAL );
	
	m_staticText4 = new wxStaticText( m_paperHPGLSizer->GetStaticBox(), wxID_ANY, _("Page size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_paperHPGLSizer->Add( m_staticText4, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxString m_HPGLPaperSizeOptionChoices[] = { _("Schematic size"), _("A4"), _("A3"), _("A2"), _("A1"), _("A0"), _("A"), _("B"), _("C"), _("D"), _("E") };
	int m_HPGLPaperSizeOptionNChoices = sizeof( m_HPGLPaperSizeOptionChoices ) / sizeof( wxString );
	m_HPGLPaperSizeOption = new wxChoice( m_paperHPGLSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_HPGLPaperSizeOptionNChoices, m_HPGLPaperSizeOptionChoices, 0 );
	m_HPGLPaperSizeOption->SetSelection( 0 );
	m_paperHPGLSizer->Add( m_HPGLPaperSizeOption, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxString m_plotOriginOptChoices[] = { _("Bottom left"), _("Center on page") };
	int m_plotOriginOptNChoices = sizeof( m_plotOriginOptChoices ) / sizeof( wxString );
	m_plotOriginOpt = new wxRadioBox( m_paperHPGLSizer->GetStaticBox(), wxID_ANY, _("Align"), wxDefaultPosition, wxDefaultSize, m_plotOriginOptNChoices, m_plotOriginOptChoices, 1, wxRA_SPECIFY_COLS );
	m_plotOriginOpt->SetSelection( 0 );
	m_paperHPGLSizer->Add( m_plotOriginOpt, 0, wxALL, 5 );
	
	m_penHPLGWidthTitle = new wxStaticText( m_paperHPGLSizer->GetStaticBox(), wxID_ANY, _("Pen width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_penHPLGWidthTitle->Wrap( -1 );
	m_paperHPGLSizer->Add( m_penHPLGWidthTitle, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_penHPGLWidthCtrl = new wxTextCtrl( m_paperHPGLSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_paperHPGLSizer->Add( m_penHPGLWidthCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	
	m_paperOptionsSizer->Add( m_paperHPGLSizer, 1, wxEXPAND, 5 );
	
	
	m_optionsSizer->Add( m_paperOptionsSizer, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerPlotFormat;
	sbSizerPlotFormat = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("General Options") ), wxVERTICAL );
	
	m_defaultLineWidthTitle = new wxStaticText( sbSizerPlotFormat->GetStaticBox(), wxID_ANY, _("Default line thickness"), wxDefaultPosition, wxDefaultSize, 0 );
	m_defaultLineWidthTitle->Wrap( -1 );
	sbSizerPlotFormat->Add( m_defaultLineWidthTitle, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_DefaultLineSizeCtrl = new wxTextCtrl( sbSizerPlotFormat->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_DefaultLineSizeCtrl->SetToolTip( _("Selection of the default pen thickness used to draw items, when their thickness is set to 0.") );
	
	sbSizerPlotFormat->Add( m_DefaultLineSizeCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxString m_ModeColorOptionChoices[] = { _("Color"), _("Black and white") };
	int m_ModeColorOptionNChoices = sizeof( m_ModeColorOptionChoices ) / sizeof( wxString );
	m_ModeColorOption = new wxRadioBox( sbSizerPlotFormat->GetStaticBox(), wxID_ANY, _("Output Mode"), wxDefaultPosition, wxDefaultSize, m_ModeColorOptionNChoices, m_ModeColorOptionChoices, 1, wxRA_SPECIFY_COLS );
	m_ModeColorOption->SetSelection( 0 );
	m_ModeColorOption->SetToolTip( _("Choose if you want to draw the sheet like it appears on screen,\nor in black and white mode, better to print it when using  black and white printers") );
	
	sbSizerPlotFormat->Add( m_ModeColorOption, 0, wxALL|wxEXPAND, 5 );
	
	m_PlotFrameRefOpt = new wxCheckBox( sbSizerPlotFormat->GetStaticBox(), wxID_ANY, _("Plot border and title block"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PlotFrameRefOpt->SetValue(true); 
	m_PlotFrameRefOpt->SetToolTip( _("Print the frame references.") );
	
	sbSizerPlotFormat->Add( m_PlotFrameRefOpt, 0, wxALL, 5 );
	
	
	m_optionsSizer->Add( sbSizerPlotFormat, 0, wxEXPAND|wxLEFT, 5 );
	
	m_ButtonsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_buttonPlotCurrent = new wxButton( this, wxID_PRINT_CURRENT, _("Plot Current Page"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonPlotCurrent->SetDefault(); 
	m_ButtonsSizer->Add( m_buttonPlotCurrent, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonPlotAll = new wxButton( this, wxID_PRINT_ALL, _("Plot All Pages"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonPlotAll->SetDefault(); 
	m_ButtonsSizer->Add( m_buttonPlotAll, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonQuit = new wxButton( this, wxID_CANCEL, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonsSizer->Add( m_buttonQuit, 0, wxALL|wxEXPAND, 5 );
	
	
	m_optionsSizer->Add( m_ButtonsSizer, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bMainSizer->Add( m_optionsSizer, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* bSizerMsgPanel;
	bSizerMsgPanel = new wxBoxSizer( wxVERTICAL );
	
	m_MessagesBox = new WX_HTML_REPORT_PANEL( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_MessagesBox->SetMinSize( wxSize( 300,150 ) );
	
	bSizerMsgPanel->Add( m_MessagesBox, 1, wxEXPAND | wxALL, 5 );
	
	
	bMainSizer->Add( bSizerMsgPanel, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	
	this->SetSizer( bMainSizer );
	this->Layout();
	bMainSizer->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnCloseWindow ) );
	m_browseButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnOutputDirectoryBrowseClicked ), NULL, this );
	m_plotFormatOpt->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnPlotFormatSelection ), NULL, this );
	m_HPGLPaperSizeOption->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnHPGLPageSelected ), NULL, this );
	m_buttonPlotCurrent->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnButtonPlotCurrentClick ), NULL, this );
	m_buttonPlotAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnButtonPlotAllClick ), NULL, this );
	m_buttonQuit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnButtonCancelClick ), NULL, this );
}

DIALOG_PLOT_SCHEMATIC_BASE::~DIALOG_PLOT_SCHEMATIC_BASE()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnCloseWindow ) );
	m_browseButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnOutputDirectoryBrowseClicked ), NULL, this );
	m_plotFormatOpt->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnPlotFormatSelection ), NULL, this );
	m_HPGLPaperSizeOption->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnHPGLPageSelected ), NULL, this );
	m_buttonPlotCurrent->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnButtonPlotCurrentClick ), NULL, this );
	m_buttonPlotAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnButtonPlotAllClick ), NULL, this );
	m_buttonQuit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_PLOT_SCHEMATIC_BASE::OnButtonCancelClick ), NULL, this );
	
}
