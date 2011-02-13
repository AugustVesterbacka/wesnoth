/* $Id$ */
/*
   Copyright (C) 2007 - 2011 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file
 * Implementation of settings.hpp.
 */

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/widgets/settings.hpp"

#include "asserts.hpp"
#include "config_cache.hpp"
#include "filesystem.hpp"
#include "foreach.hpp"
#include "gettext.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/auxiliary/tips.hpp"
#include "gui/widgets/window.hpp"
#include "serialization/parser.hpp"
#include "serialization/preprocessor.hpp"
#include "formula_string_utils.hpp"

namespace gui2 {

bool new_widgets = false;

namespace settings {
	unsigned screen_width = 0;
	unsigned screen_height = 0;

	unsigned gamemap_width = 0;
	unsigned gamemap_height = 0;

	unsigned popup_show_delay = 0;
	unsigned popup_show_time = 0;
	unsigned help_show_time = 0;
	unsigned double_click_time = 0;
	unsigned repeat_button_repeat_time = 0;

	std::string sound_button_click = "";
	std::string sound_toggle_button_click = "";
	std::string sound_toggle_panel_click = "";
	std::string sound_slider_adjust = "";

	std::vector<ttip> tips;

	std::vector<ttip> get_tips()
	{
		return tips::shuffle(tips);
	}

} // namespace settings

/**
 * Returns the list of registered windows.
 *
 * The function can be used the look for registered windows or to add them.
 */
static std::vector<std::string>& registered_window_types()
{
	static std::vector<std::string> result;
	return result;
}

typedef std::map<
		  std::string
		, boost::function<void(
			  tgui_definition&
			  , const std::string&
			  , const config&
			  , const char *key)> > tregistered_widget_type;

static tregistered_widget_type& registred_widget_type()
{
	static tregistered_widget_type result;
	return result;
}

struct tgui_definition
{
	tgui_definition()
		: id()
		, description()
		, control_definition()
		, windows()
		, window_types()
		, popup_show_delay_(0)
		, popup_show_time_(0)
		, help_show_time_(0)
		, double_click_time_(0)
		, repeat_button_repeat_time_(0)
		, sound_button_click_()
		, sound_toggle_button_click_()
		, sound_toggle_panel_click_()
		, sound_slider_adjust_()
		, tips_()
	{
	}

	std::string id;
	t_string description;

	const std::string& read(const config& cfg);

	/** Activates a gui. */
	void activate() const;

	typedef std::map <std::string /*control type*/,
		std::map<std::string /*id*/, tcontrol_definition_ptr> >
		tcontrol_definition_map;

	tcontrol_definition_map control_definition;

	std::map<std::string, twindow_definition> windows;

	std::map<std::string, twindow_builder> window_types;

	void load_widget_definitions(
			  const std::string& definition_type
			, const std::vector<tcontrol_definition_ptr>& definitions);
private:

	unsigned popup_show_delay_;
	unsigned popup_show_time_;
	unsigned help_show_time_;
	unsigned double_click_time_;
	unsigned repeat_button_repeat_time_;

	std::string sound_button_click_;
	std::string sound_toggle_button_click_;
	std::string sound_toggle_panel_click_;
	std::string sound_slider_adjust_;
	std::vector<ttip> tips_;
};

const std::string& tgui_definition::read(const config& cfg)
{
/*WIKI
 * @page = GUIToolkitWML
 * @order = 1
 *
 * THIS PAGE IS AUTOMATICALLY GENERATED, DO NOT MODIFY DIRECTLY !!!
 *
 * = GUI =
 *
 * The gui class contains the definitions of all widgets and windows used in
 * the game. This can be seen as a skin and it allows the user to define the
 * visual aspect of the various items. The visual aspect can be determined
 * depending on the size of the game window.
 *
 * Widgets have a definition and an instance, the definition contains the
 * general info/looks of a widget and the instance the actual looks. Eg the
 * where the button text is placed is the same for every button, but the
 * text of every button might differ.
 *
 * The default gui has the id ''default'' and must exist, in the default gui
 * there must a definition of every widget with the id ''default'' and every
 * window needs to be defined. If the definition of a widget with a certain
 * id doesn't exist it will fall back to default in the current gui, if it's
 * not defined there either it will fall back to the default widget in the
 * default theme. That way it's possible to slowly create your own gui and
 * test it.
 *
 * The gui has the following data:
 * @begin{table}{config}
 *     id & string & &                  Unique id for this gui (theme). $
 *     description & t_string & &       Unique translatable name for this gui. $
 *
 *     widget_definitions & section & & The definitions of all
 *                                   [[#widget_list|widgets]]. $
 *     window & section & &             The definitions of all
 *                                   [[#window_list|windows]]. $
 *     settings & section & &           The settings for the gui. $
 * @end{table}
 *
 * <span id="widget_list"></span>List of available widgets:
 * @begin{table}{widget_overview}
 *     Button &                       @macro = button_description $
 *     Image &                        @macro = image_description $
 *     Horizontal_listbox &           @macro = horizontal_listbox_description $
 *     Horizontal_scrollbar &         @macro = horizontal_scrollbar_description $
 *     Label &                        @macro = label_description $
 *     Listbox &                      @macro = listbox_description $
 *     Minimap &                      @macro = minimap_description $
 *     Multi_page &                   @macro = multi_page_description $
 *     Panel &                        @macro = panel_description $
 *     Repeating_button &             @macro = repeating_button_description $
 *     Scroll_label &                 @macro = scroll_label_description $
 *     Slider &                       @macro = slider_description $
 *     Spacer &                       @macro = spacer_description $
 *     Stacked_widget &               A stacked widget is a control several
 *                                   widgets can be stacked on top of each
 *                                   other in the same space. This is mainly
 *                                   intended for over- and underlays. (The
 *                                   widget is still experimental.) $
 *     Text_box &                     A single line text box. $
 *     Tree_view &                    @macro = tree_view_description $
 *     Toggle_button &                A kind of button with two 'states' normal
 *                                   and selected. This is a more generic widget
 *                                   which is used for eg checkboxes and
 *                                   radioboxes. $
 *     Toggle_panel &                 Like a toggle button but then as panel so
 *                                   can hold multiple items in a grid.
 *     Tooltip &                      A small tooltip with help. $
 *     Tree_view &                    A tree view widget. $
 *     Vertical_scrollbar &           A vertical scrollbar. $
 *     Window &                       A window. $
 * @end{table}
 *
 * <span id="window_list"></span>List of available windows:
 * @begin{table}{window_overview}
 *     Addon_connect &                The dialog to connect to the addon server
 *                                   and maintain locally installed addons. $
 *     Addon_list &                   Shows the list of the addons to install or
 *                                   update. $
 *     Campaign_selection &           Shows the list of campaigns, to select one
 *                                   to play. $
 *     Language_selection &           The dialog to select the primairy language. $
 *     WML_message_left &             The ingame message dialog with a portrait
 *                                   on the left side. (Used for the WML messages.) $
 *     WML_message_right &            The ingame message dialog with a portrait
 *                                   on the right side. (Used for the WML
 *                                   messages.) $
 *     Message &                      A generic message dialog. $
 *     MP_connect &                   The dialog to connect to the MP server. $
 *     MP_method_selection &          The dialog to select the kind of MP game
 *                                   to play. Official server, local etc. $
 *     MP_server_list &               List of the 'official' MP servers. $
 *     MP_login &                     The dialog to provide a password for registered
 *                                   usernames, request a password reminder or
 *                                   choose a different username. $
 *     MP_cmd_wrapper &               Perform various actions on the selected user
 *                                   (e.g. whispering or kicking). $
 *     MP_create_game &               The dialog to select and create an MP game. $
 *     Title_screen &                 The title screen. $
 *     Editor_new_map &               Creates a new map in the editor. $
 *     Editor_generate_map &          Generates a random map in the editor. $
 *     Editor_resize_map &            Resizes a map in the editor. $
 *     Editor_settings &              The settings specific for the editor. $
 * @end{table}
 *
 */
	id = cfg["id"].str();
	description = cfg["description"];

	VALIDATE(!id.empty(), missing_mandatory_wml_key("gui", "id"));
	VALIDATE(!description.empty(), missing_mandatory_wml_key("gui", "description"));

	DBG_GUI_P << "Parsing gui " << id << '\n';

	/***** Control definitions *****/
	typedef std::pair<
			  const std::string
			, boost::function<void(
				  tgui_definition&
				  , const std::string&
				  , const config&
				  , const char *key)> > thack;

	foreach(thack& widget_type, registred_widget_type()) {
		widget_type.second(*this, widget_type.first, cfg, NULL);
	}

	/***** Window types *****/
	foreach (const config &w, cfg.child_range("window")) {
		std::pair<std::string, twindow_builder> child;
		child.first = child.second.read(w);
		window_types.insert(child);
	}

	if(id == "default") {
		// The default gui needs to define all window types since we're the
		// fallback in case another gui doesn't define the window type.
		for(std::vector<std::string>::const_iterator itor
				= registered_window_types().begin()
				; itor != registered_window_types().end()
				; ++itor) {

            const std::string error_msg("Window not defined in WML: '" +
                                         *itor +
                                         "'. Perhaps a mismatch between data and source versions."
                                         " Try --data-dir <trunk-dir>" );
			VALIDATE(window_types.find(*itor) != window_types.end(), error_msg );
		}
	}

	/***** settings *****/
/*WIKI
 * @page = GUIToolkitWML
 * @order = 1
 *
 * A setting section has the following variables:
 * @begin{table}{config}
 *     popup_show_delay & unsigned & 0 & The time it take before the popup shows
 *                                     if the mouse moves over the widget. 0
 *                                     means show directly. $
 *     popup_show_time & unsigned & 0 &  The time a shown popup remains visible.
 *                                     0 means until the mouse leaves the
 *                                     widget. $
 *     help_show_time & unsigned & 0 &   The time a shown help remains visible.
 *                                     0 means until the mouse leaves the
 *                                     widget. $
 *     double_click_time & unsigned & &   The time between two clicks to still be a
 *                                     double click. $
 *     repeat_button_repeat_time & unsigned & 0 &
 *                                     The time a repeating button waits before
 *                                     the next event is issued if the button
 *                                     is still pressed down. $
 *
 *     sound_button_click & string & "" &
 *                                     The sound played if a button is
 *                                     clicked. $
 *     sound_toggle_button_click & string & "" &
 *                                     The sound played if a toggle button is
 *                                     clicked. $
 *     sound_toggle_panel_click & string & "" &
 *                                     The sound played if a toggle panel is
 *                                     clicked. Normally the toggle panels
 *                                     are the items in a listbox. If a
 *                                     toggle button is in the listbox it's
 *                                     sound is played. $
 *     sound_slider_adjust & string & "" &
 *                                     The sound played if a slider is
 *                                     adjusted. $
 * @end{table}
 */

/**
 * @todo Regarding sounds:
 * Need to evaluate but probably we want the widget definition be able to:
 * - Override the default (and clear it). This will allow toggle buttons in a
 *   listbox to sound like a toggle panel.
 * - Override the default and above per instance of the widget, some buttons
 *   can give a different sound.
 */
	const config &settings = cfg.child("settings");

	popup_show_delay_ = settings["popup_show_delay"];
	popup_show_time_ = settings["popup_show_time"];
	help_show_time_ = settings["help_show_time"];
	double_click_time_ = settings["double_click_time"];

	repeat_button_repeat_time_ = settings["repeat_button_repeat_time"];

	VALIDATE(double_click_time_, missing_mandatory_wml_key("settings", "double_click_time"));

	sound_button_click_ = settings["sound_button_click"].str();
	sound_toggle_button_click_ = settings["sound_toggle_button_click"].str();
	sound_toggle_panel_click_ = settings["sound_toggle_panel_click"].str();
	sound_slider_adjust_ = settings["sound_slider_adjust"].str();

	tips_ = tips::load(cfg);

	return id;
}

void tgui_definition::activate() const
{
	settings::popup_show_delay = popup_show_delay_;
	settings::popup_show_time = popup_show_time_;
	settings::help_show_time = help_show_time_;
	settings::double_click_time = double_click_time_;
	settings::repeat_button_repeat_time = repeat_button_repeat_time_;
	settings::sound_button_click = sound_button_click_;
	settings::sound_toggle_button_click = sound_toggle_button_click_;
	settings::sound_toggle_panel_click = sound_toggle_panel_click_;
	settings::sound_slider_adjust = sound_slider_adjust_;
	settings::tips = tips_;
}

void tgui_definition::load_widget_definitions(
		  const std::string& definition_type
		, const std::vector<tcontrol_definition_ptr>& definitions)
{
	foreach(const tcontrol_definition_ptr& def, definitions) {

		// We assume all definitions are unique if not we would leak memory.
		assert(control_definition[definition_type].find(def->id)
				== control_definition[definition_type].end());

		control_definition[definition_type]
				.insert(std::make_pair(def->id, def));
	}

	utils::string_map symbols;
	symbols["definition"] = definition_type;
	symbols["id"] = "default";
	t_string msg(vgettext(
			  "Widget definition '$definition' "
			  "doesn't contain the definition for '$id'."
			, symbols));

	VALIDATE(control_definition[definition_type].find("default")
			!= control_definition[definition_type].end(), msg);

}

	/** Map with all known windows, (the builder class builds a window). */
	std::map<std::string, twindow_builder> windows;

	/** Map with all known guis. */
	std::map<std::string, tgui_definition> guis;

	/** Points to the current gui. */
	std::map<std::string, tgui_definition>::const_iterator current_gui = guis.end();

void register_window(const std::string& id)
{
	const std::vector<std::string>::iterator itor = std::find(
			  registered_window_types().begin()
			, registered_window_types().end()
			, id);

	if(itor == registered_window_types().end()) {
		registered_window_types().push_back(id);
	}
}

void load_settings()
{
	LOG_GUI_G << "Setting: init gui.\n";

	// Init.
	twindow::update_screen_size();

	// Read file.
	config cfg;
	try {
		preproc_map preproc(
				game_config::config_cache::instance().get_preproc_map());
		scoped_istream stream = preprocess_file(get_wml_location("gui/default.cfg"), &preproc);

		read(cfg, *stream);
	} catch(config::error&) {
		ERR_GUI_P << "Setting: could not read file 'data/gui/default.cfg'.\n";
	}

	// Parse guis
	foreach (const config &g, cfg.child_range("gui")) {
		std::pair<std::string, tgui_definition> child;
		child.first = child.second.read(g);
		guis.insert(child);
	}

	VALIDATE(guis.find("default") != guis.end(), _ ("No default gui defined."));

	current_gui = guis.find("default");
	current_gui->second.activate();
}

tstate_definition::tstate_definition(const config &cfg) :
	canvas()
{
/*WIKI
 * @page = GUIToolkitWML
 * @order = 1_widget
 *
 * == State ==
 *
 * Definition of a state. A state contains the info what to do in a state.
 * Atm this is rather focussed on the drawing part, might change later.
 * Keys:
 * @begin{table}{config}
 *     draw & section & &                 Section with drawing directions for a canvas. $
 * @end{table}
 *
 */

	const config &draw = *(cfg ? &cfg.child("draw") : &cfg);

	VALIDATE(draw, _("No state or draw section defined."));

	canvas.set_cfg(draw);
}

void register_widget(const std::string& id
		, boost::function<void(
			  tgui_definition& gui_definition
			, const std::string& definition_type
			, const config& cfg
			, const char *key)> functor)
{
	registred_widget_type().insert(std::make_pair(id, functor));
}

void load_widget_definitions(
	  tgui_definition& gui_definition
	, const std::string& definition_type
	, const std::vector<tcontrol_definition_ptr>& definitions)
{
	DBG_GUI_P << "Load definition '" << definition_type << "'.\n";
	gui_definition.load_widget_definitions(definition_type, definitions);
}

tresolution_definition_ptr get_control(
		const std::string& control_type, const std::string& definition)
{
	const tgui_definition::tcontrol_definition_map::const_iterator
#ifdef GUI2_EXPERIMENTAL_LISTBOX
		control_definition = (control_type == "list")
				? current_gui->second.control_definition.find("listbox")
				: current_gui->second.control_definition.find(control_type);
#else
		control_definition =
				current_gui->second.control_definition.find(control_type);
#endif

	ASSERT_LOG(control_definition != current_gui->second.control_definition.end(),
			"Type '" << control_type << "' is unknown.");

	std::map<std::string, tcontrol_definition_ptr>::const_iterator
		control = control_definition->second.find(definition);

	if(control == control_definition->second.end()) {
		LOG_GUI_G << "Control: type '" << control_type << "' definition '"
			<< definition << "' not found, falling back to 'default'.\n";
		control = control_definition->second.find("default");
		assert(control != control_definition->second.end());
	}

	for(std::vector<tresolution_definition_ptr>::const_iterator
			itor = (*control->second).resolutions.begin(),
			end = (*control->second).resolutions.end();
			itor != end;
			++itor) {

		if(settings::screen_width <= (**itor).window_width &&
				settings::screen_height <= (**itor).window_height) {

			return *itor;
		} else if (itor == end - 1) {
			return *itor;
		}
	}

	ERROR_LOG(false);
}

std::vector<twindow_builder::tresolution>::const_iterator get_window_builder(
		const std::string& type)
{
	twindow::update_screen_size();

	std::map<std::string, twindow_builder>::const_iterator
		window = current_gui->second.window_types.find(type);

	if(true) { // FIXME Test for default gui.
		if(window == current_gui->second.window_types.end()) {
			throw twindow_builder_invalid_id();
		}
	} else {
		// FIXME Get the definition in the default gui and do an assertion test.
	}

	for(std::vector<twindow_builder::tresolution>::const_iterator
			itor = window->second.resolutions.begin(),
			end = window->second.resolutions.end();
			itor != end;
			++itor) {

		if(settings::screen_width <= itor->window_width &&
				settings::screen_height <= itor->window_height) {

			return itor;
		} else if (itor == end - 1) {
			return itor;
		}
	}

	ERROR_LOG(false);
}

/*WIKI
 * @page = GUIWidgetDefinitionWML
 * @order = ZZZZZZ_footer
 *
 * [[Category: WML Reference]]
 * [[Category: GUI WML Reference]]
 * [[Category: Generated]]
 *
 */

} // namespace gui2
