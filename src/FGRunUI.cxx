// FGRunUI.cxx -- FlightGear Run User Interface
//
// Written by Bernie Bright, started Oct 2002.
//
// Copyright (C) 2002  Bernie Bright - bbright@bigpond.net.au
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id$

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <FL/Fl_File_Chooser.h>
#include <FL/Fl_Text_Buffer.h>
#include <FL/filename.h>

#ifdef HAVE_STRING_H
#  include <cstring>
#endif
#ifdef HAVE_STDLIB_H
#  include <cstdlib>
#endif

#include "FGRunUI.h"

using std::cout;
using std::cerr;
using std::vector;
using std::ofstream;
using std::string;

FGRunUI::FGRunUI()
    : UserInterface()
      , default_aircraft("c172")
      , default_airport("KSFO")
      , modflag(false)
      , io_options_list_value(0)
{
    // Ensure the "General" tab is displayed.
    tabs->value( general_tab );
    output_text->buffer( new Fl_Text_Buffer );
}

FGRunUI::~FGRunUI()
{
    output_window->hide();
}

/**
 * 
 */
int
FGRunUI::set_choice( Fl_Choice* w, const char* s )
{
    const Fl_Menu_Item* m = w->menu();
    int i = 0;
    int nest = 0;
    for (i = 0; i < m->size(); ++i)
    {
	if (m[i].submenu())
	{
	    ++nest;
	}
	else if (m[i].label() != 0)
	{
	    if (strcmp( s, m[i].label() ) == 0)
	    {
		w->value( i );
		return 1;
	    }
	}
	else
	{
	    --nest;
	}
    }
    return 0;
}


void
FGRunUI::update_aircraft()
{
    aircraft->clear();
    cout << "Searching for aircraft...\n";
    string acdir = fg_root->value();
    acdir += "/Aircraft";

    dirent **files;
    int num_files = fl_filename_list( acdir.c_str(), &files, fl_numericsort );
    if (num_files < 0)
    {
	return;
    }

    for (int i = 0; i < num_files; ++i)
    {
	if (fl_filename_match( files[i]->d_name, "*-set.xml"))
	{
	    // Extract aircraft name from filename.
	    char* p = strstr( files[i]->d_name, "-set.xml" );
	    if (p != 0)
	    {
		*p = 0;
	    }

	    int index = aircraft->add( files[i]->d_name, 0, 0, 0, 0 );

	    // Select the default aircraft
	    if (strcmp(files[i]->d_name, default_aircraft.c_str() ) == 0)
	    {
		aircraft->value(index);
	    }
	}
	free( files[i] );
    }

    free( files );
}

/**
 * Recursively search through scenery directories looking for airport files.
 */
static void
scan_for_airports( const char* scenery, vector<string>& apts )
{
    dirent **files;
    int num_files = fl_filename_list( scenery, &files, fl_numericsort );
    if (num_files < 0)
	return;

    for (int i = 0; i < num_files; ++i)
    {
	if (fl_filename_match( files[i]->d_name,
			       "[ew][0-9][0-9][0-9][ns][0-9][0-9]"))
	{
	    string dir = scenery;
	    dir += "/";
	    dir += files[i]->d_name;
	    if (fl_filename_isdir( dir.c_str() ))
	    {
		scan_for_airports( dir.c_str(), apts );
	    }
	}
	else if (fl_filename_match( files[i]->d_name, "???.btg.gz") ||
		 fl_filename_match( files[i]->d_name, "????.btg.gz")
		 )
	{
	    char* p = strstr( files[i]->d_name, ".btg");
	    if (p != 0)
		*p = 0;
	    apts.push_back( string( files[i]->d_name ) );
	}
	free( files[i] );
    }

    free( files );
}

/**
 * Functor to add a airports to a Fl_Choice widget.
 */
struct AddAirportHelper
{
    Fl_Choice* airport;
    AddAirportHelper( Fl_Choice* apt ) : airport(apt) {}

    void operator()( const string& s ) {
	string x(s, 0, 1);
	x += "/";
	x += s;
	airport->add( x.c_str(), 0, 0, 0, 0 );
    }
};

/**
 * 
 */
void
FGRunUI::update_airports()
{
    airport->clear();
    cout << "Searching for airports...\n";
    vector< string > apts;
    scan_for_airports( fg_scenery->value(), apts );
    std::sort( apts.begin(), apts.end() );
    std::for_each( apts.begin(), apts.end(), AddAirportHelper( airport ) );
    set_choice( airport, default_airport.c_str() );
}

void
FGRunUI::write_dot_fgfsrc()
{
    char fname[ FL_PATH_MAX ];
    fl_filename_expand( fname, "~/.fgfsrc" );
    ofstream ofs( fname );
    if (ofs)
    {
	int i;

	ofs << "--fg-root=" << fg_root->value()
	    << "\n--fg-scenery=" << fg_scenery->value()
	    << "\n--airport-id=" << airport->text()
	    << "\n--aircraft=" << aircraft->text();

	// Only write non-default options.

// 	if (units_meters->value())
// 	    ofs << "\n--unit-meters";

	if (enable_game_mode->value())
	    ofs << "\n--enable-game-mode";

	if (!enable_splash_screen->value())
	    ofs << "\n--disable-splash-screen";

	if (!enable_intro_music->value())
	    ofs << "\n--disable-intro-music";

// 	if (enable_mouse_pointer->value())
// 	    ofs << "\n--enable-mouse-pointer";
// 	else
// 	    ofs << "\n--disable-mouse-pointer";

	if (enable_random_objects->value())
	    ofs << "\n--enable-random-objects";
	else
	    ofs << "\n--disable-random-objects";

	if (enable_clouds->value() == 0)
	    ofs << "\n--disable-clouds";

	if (enable_clouds3d->value())
	    ofs << "\n--enable-clouds3d";
	else
	    ofs << "\n--disable-clouds3d";

	if (!enable_sound->value())
	    ofs << "\n--disable-sound";

	if (strcmp( fog->text(), "disabled" ) == 0)
	    ofs << "\n--fog-disable";
	else if (strcmp( fog->text(), "fast" ) == 0)
	    ofs << "\n--fog-fastest";

	if (shading_flat->value())
	    ofs << "\n--shading-flat";

	if (!enable_panel->value())
	    ofs << "\n--disable-panel";

	if (enable_hud->value())
	    ofs << "\n--enable-hud";

// 	if (enable_antialias_hud->value())
// 	    ofs << "\n--enable-anti-alias-hud";
// 	else
// 	    ofs << "\n--disable-anti-alias-hud";

	if (!enable_skyblend->value())
	    ofs << "\n--disable-skyblend";

	if (enable_wireframe->value())
	    ofs << "\n--enable-wireframe";

	ofs << "\n--fdm=" << fdm->text();
	if (strcmp( fdm->text(), "jsb") == 0 && notrim->value())
	    ofs << "\n--notrim";
	if (in_air->value())
	    ofs << "\n--in-air";

	if (time_offset->value())
	    ofs << "\n--time-offset=" << time_offset_text->value();
	else if (time_match_real->value())
	    ofs << "\n--time-match-real";
	else if (time_match_local->value())
	    ofs << "\n--time-match-local";
	else if (start_date_sys->value())
	    ofs << "\n--start-date-sys=" << start_date_sys_text->value();
	else if (start_date_lat->value())
	    ofs << "\n--start-date-lat=" << start_date_lat_text->value();
	else if (start_date_gmt->value())
	    ofs << "\n--start-date-gmt=" << start_date_gmt_text->value();

	if (strcmp( control->text(), "joystick" ) != 0)
	    ofs << "\n--control=" << control->text();

	if (lon->value()[0] != 0)
	    ofs << "\n--lon=" << lon->value();
	if (lat->value()[0] != 0)
	    ofs << "\n--lat=" << lat->value();
	if (altitude->value()[0] != 0)
	    ofs << "\n--altitude=" << altitude->value();
	if (heading->value()[0] != 0)
	    ofs << "\n--heading=" << heading->value();
	if (roll->value()[0] != 0)
	    ofs << "\n--roll=" << roll->value();
	if (pitch->value()[0] != 0)
	    ofs << "\n--pitch=" << pitch->value();
	if (vc->value()[0] != 0)
	    ofs << "\n--vc=" << vc->value();

	if (httpd->value())
	    ofs << "\n--httpd=" << httpd_port->value();
	if (props->value())
	    ofs << "\n--props=" << props_port->value();
	if (jpg_httpd->value())
	    ofs << "\n--jpg-httpd=" << jpg_httpd_port->value();

	for (i = 1; i <= io_options_list->size(); ++i)
	{
	    ofs << "\n" << io_options_list->text(i);
	}

	for (i = 1; i <= props_list->size(); ++i)
	{
	    ofs << "\n" << props_list->text(i);
	}

	ofs << "\n";

	ofs.close();
    }

}

void
FGRunUI::select_fg_exe()
{
    char* p = fl_file_chooser( "Select fgfs executable", NULL, NULL, 0);
    if (p != 0)
    {
	fg_exe->value( p );
	modflag = true;
    }
}

void
FGRunUI::select_fg_root()
{
    char* p = fl_dir_chooser( "Select FG_ROOT directory",
			      fg_root->value(), 0 );
    if (p != 0)
    {
	fg_root->value( p );

	if (fg_scenery->value()[0] == 0)
	{
	    char dir[1024];
	    snprintf( dir, sizeof(dir), "%s/Scenery", p );
	    fg_scenery->value( dir );
	}

	update_aircraft();
	update_airports();
	modflag = true;
    }
}

void
FGRunUI::select_fg_scenery()
{
    char* p = fl_dir_chooser( "Select FG_SCENERY directory",
			      fg_scenery->value(), 0 );
    if (p != 0)
    {
	fg_scenery->value( p );
	update_airports();
	modflag = true;
    }
}

void
FGRunUI::select_browser()
{
    char* p = fl_file_chooser( "Select FlightGear help browser",
			      browser->value(), 0 );
    if (p != 0)
    {
	browser->value( p );
	modflag = true;
    }
}

void
FGRunUI::add_io_item()
{
    io_options_list_value = 0;
    addOptionDlg->show();
    io_medium->do_callback();
}

void
FGRunUI::edit_io_item()
{
    io_options_list_value = io_options_list->value();
    if (io_options_list_value <= 0)
	return;

    const char* item = io_options_list->text( io_options_list_value );
    char protocol[32];
    char medium[32];
    char direction[32];
    int hz = 0;
    char rem[80];

    int n = sscanf( item, "--%[^=]=%[^,],%[^,],%d,%s",
		    protocol, medium, direction, &hz, rem );
    set_choice( io_protocol, protocol );
    set_choice( io_medium, medium );
    io_medium->do_callback();
    io_hz->value( double(hz) );
    set_choice( io_dir, direction );

    if (strcmp( medium, "file" ) == 0)
    {
	file_name->value( rem );
    }
    else if (strcmp( medium, "serial" ) == 0)
    {
	char port[32];
	char baud[32];
	sscanf( rem, "%[^,],%s", port, baud );
	serial_port->value( port );
	serial_baud->value( baud );
    }
    else if (strcmp( medium, "socket" ) == 0)
    {
	char hostname[128];
	int port;
	char type[32];
	sscanf( rem, "%[^,],%d,%s", hostname, &port, type );
    }

    addOptionDlg->show();
}

void
FGRunUI::delete_io_item()
{
    int i = io_options_list->value();
    if (i <= 0)
	return;

    io_options_list->remove( i );
}

void
FGRunUI::OptionDlg_ok_cb()
{
    char buf[256];
    char buf2[256];

    snprintf( buf2, sizeof(buf2), "--%s=%s,%s,%d",
	      io_protocol->text(),
	      io_medium->text(),
	      io_dir->text(),
	      int(io_hz->value()) );

    if (strcmp(io_medium->text(), "file") == 0)
    {
	snprintf( buf, sizeof(buf), "%s,%s",
		  buf2, file_name->value());
    }
    else if (strcmp(io_medium->text(), "serial") == 0)
    {
	snprintf( buf, sizeof(buf), "%s,%s,%d",
		  buf2,
		  serial_port->value(),
		  int(serial_baud->value()));
    }
    else if (strcmp(io_medium->text(), "socket") == 0)
    {
	snprintf( buf, sizeof(buf), "%s,%s,%d,%s",
		  buf2, socket_hostname->value(),
		  int(socket_port->value()),
		  socket_tcp->value() ? "tcp" : "udp" );
    }

    if (io_options_list_value == 0)
    {
	// Add an item to I/O options.
	io_options_list->add( buf );
    }
    else
    {
	// Replace an item in I/O options.
	io_options_list->remove( io_options_list_value );
	io_options_list->insert( io_options_list_value, buf );
    }

    addOptionDlg->hide();
}

void
FGRunUI::io_medium_cb( Fl_Choice* o, void* )
{
    if (strcmp(o->text(), "file") == 0)
    {
	file_group->show();
	// Disable 'bi' menu option for files.
	menu_io_dir[2].deactivate();
	serial_group->hide();
	socket_group->hide();
    }
    else if (strcmp(o->text(), "serial") == 0)
    {
	serial_group->show();
	file_group->hide();
	socket_group->hide();
	menu_io_dir[2].activate();
    }
    else if (strcmp(o->text(), "socket") == 0)
    {
	socket_group->show();
	file_group->hide();
	serial_group->hide();
	menu_io_dir[2].activate();
    }
}

/**
 * Reset all values to their defaults.
 */
void FGRunUI::reset()
{
    fg_root->value( "" );
    fg_scenery->value( "" );
    default_aircraft = "c172";
    default_airport = "KSFO";
    enable_game_mode->value( 0 );
    enable_splash_screen->value( 1 );
    enable_intro_music->value( 1 );
    //enable_mouse_pointer->value(0);
    enable_random_objects->value( 0 );
    freeze->value( 0 );
    fuel_freeze->value( 0 );
    clock_freeze->value( 0 );
    auto_coordination->value( 0 );
//     units_feet->value( 1 );
#if defined(WIN32)
    browser->value( "webrun.bat" );
#else
    browser->value( "netscape" );
#endif
    set_choice( control, "joystick" );
    enable_hud->value( 0 );
    enable_antialias_hud->value( 0 );
    enable_hud->do_callback();
    enable_panel->value( 1 );
    enable_sound->value( 1 );
    enable_clouds->value( 1 );
    enable_clouds3d->value( 0 );
    enable_skyblend->value( 1 );
    enable_textures->value( 1 );
    enable_wireframe->value( 0 );
    enable_fullscreen->value( 0 );
    set_choice( fog, "nice" );
    shading_flat->value( 0 );
    fov->value( 60.0 );
    set_choice( fdm, "jsb" );
    fdm->do_callback();
    notrim->value( 0 );
    on_ground->setonly();
    model_hz->value( 120 );
    speed->value( 1 );
    time_offset_text->value( 0 );
    start_date_sys_text->value( 0 );
    start_date_lat_text->value( 0 );
    start_date_lat_text->value( 0 );
    time_match_real->setonly();
    time_match_real->do_callback();
}
