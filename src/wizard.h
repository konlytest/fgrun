// generated by Fast Light User Interface Designer (fluid) version 1.0106

#ifndef wizard_h
#define wizard_h
#include <FL/Fl.H>
#include <string>
#include <iostream>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Pixmap.H>
class LogWindow;
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Wizard.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Tile.H>
#include "Fl_Plib.h"
#include "AirportBrowser.h"

class Wizard {
public:
  Wizard() ;
private:
  Fl_Double_Window *win;
  Fl_Wizard *wiz;
  Fl_Help_View *about_;
  Fl_Input *fg_exe_;
  inline void cb_fg_exe__i(Fl_Input*, void*);
  static void cb_fg_exe_(Fl_Input*, void*);
  inline void cb__i(Fl_Button*, void*);
  static void cb_(Fl_Button*, void*);
  Fl_Input *fg_root_;
  inline void cb_fg_root__i(Fl_Input*, void*);
  static void cb_fg_root_(Fl_Input*, void*);
  inline void cb_1_i(Fl_Button*, void*);
  static void cb_1(Fl_Button*, void*);
  Fl_Browser *scenery_dir_list_;
  inline void cb_scenery_dir_list__i(Fl_Browser*, void*);
  static void cb_scenery_dir_list_(Fl_Browser*, void*);
  Fl_Button *scenery_dir_up_;
  inline void cb_scenery_dir_up__i(Fl_Button*, void*);
  static void cb_scenery_dir_up_(Fl_Button*, void*);
  Fl_Button *scenery_dir_down_;
  inline void cb_scenery_dir_down__i(Fl_Button*, void*);
  static void cb_scenery_dir_down_(Fl_Button*, void*);
  Fl_Button *cache_delete_;
  inline void cb_cache_delete__i(Fl_Button*, void*);
  static void cb_cache_delete_(Fl_Button*, void*);
  Fl_Output *cache_file_;
  inline void cb_Add_i(Fl_Button*, void*);
  static void cb_Add(Fl_Button*, void*);
  Fl_Button *scenery_dir_delete_;
  inline void cb_scenery_dir_delete__i(Fl_Button*, void*);
  static void cb_scenery_dir_delete_(Fl_Button*, void*);
  Fl_Browser *aircraft;
  inline void cb_aircraft_i(Fl_Browser*, void*);
  static void cb_aircraft(Fl_Browser*, void*);
  Fl_Plib *preview;
  AirportBrowser *airports_;
  Fl_Output *text;
  inline void cb_Advanced_i(Fl_Button*, void*);
  static void cb_Advanced(Fl_Button*, void*);
public:
  Fl_Group *page[5];
private:
  Fl_Button *next;
  inline void cb_next_i(Fl_Button*, void*);
  static void cb_next(Fl_Button*, void*);
  Fl_Button *prev;
  inline void cb_prev_i(Fl_Button*, void*);
  static void cb_prev(Fl_Button*, void*);
  Fl_Button *cancel;
  inline void cb_cancel_i(Fl_Button*, void*);
  static void cb_cancel(Fl_Button*, void*);
public:
  ~Wizard();
  void show();
  void show( int argc, char* argv[] );
  void init( bool fullscreen );
  void update_preview();
  void preview_aircraft();
private:
  void aircraft_update();
  void next_cb();
  void prev_cb();
  void advanced_options_cb();
  void fg_exe_update_cb();
  void fg_exe_select_cb();
  void fg_root_update_cb();
  void fg_root_select_cb();
  void advanced_cb();
  int write_fgfsrc();
  int write_fgfsrc( std::ostream&, const char* pfx = "\n");
  void run_fgfs( const std::string & );
  static void stdout_cb( int, void* );
  void stdout_cb( int );
  void cancel_cb();
  void delete_cache_file_cb();
  static void airports_cb( Fl_Widget*, void* );
  void airports_cb();
  void scenery_dir_select_cb();
  void scenery_dir_add_cb();
  void scenery_dir_delete_cb();
  void scenery_dir_up_cb();
  void scenery_dir_down_cb();
public:
  void refresh_airports();
  static void refresh_airports( Fl_Widget*, void* );
private:
  Fl_Preferences prefs;
  LogWindow* logwin;
  Fl_Pixmap folder_open_pixmap;
};
#endif
