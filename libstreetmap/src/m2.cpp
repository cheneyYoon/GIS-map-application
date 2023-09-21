/* 
 * Copyright 2023 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include "LatLon.h"
#include "m2.h"
#include "m1.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <sstream>
#include "m3.h"
#include "helper.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include "libcurl.h"
#include "drawing.h"
#include "loading.h"
#include "UIhelper.h"
using namespace std;
bool valid;
void act_on_mouse_click(ezgl::application *app, GdkEventButton* /*event*/, double x, double y);
//void weather_callback(GtkWidget* /*widget*/, ezgl::application *app);
void findPath(GtkWidget* /*widget*/, ezgl::application *app);
void changeMap(std::string map_to_load, std::string city_name,ezgl::application *application);

void city_select_callback(GtkComboBox *box, gpointer data);
void initial_setup(ezgl::application *application, bool /*new_window*/);
void getsBar1();
void getsBar2();
void getSearched();

//ui settings
void initial_setup(ezgl::application *application, bool /*new_window*/)
{   
    //for general help
    GtkButton* Help = (GtkButton*) application->get_object("HelpButton");
    g_signal_connect(Help, "clicked", G_CALLBACK(help_callback), application);
    //weather info
    GtkButton* weatherButton = (GtkButton*) application->get_object("WeatherButton");
    g_signal_connect(weatherButton, "clicked", G_CALLBACK(weather_callback), application);
    //to find
    GtkButton* findButton = (GtkButton*) application->get_object("FindButton");
    g_signal_connect(findButton, "clicked", G_CALLBACK(findPath), application);
    //to load different map
    
    GtkComboBox* cityDropDown = (GtkComboBox*) application->get_object("CityDropDown");
    g_signal_connect(cityDropDown, "changed", G_CALLBACK(city_select_callback), application);
    //loading colorblind mode
    GtkButton* colorblindMode = (GtkButton*) application->get_object("ColorblindModeButton");
    g_signal_connect(colorblindMode, "clicked", G_CALLBACK(colorblind_callback), application);
    //clear all path visible
    GtkButton* clearPath = (GtkButton*) application->get_object("ClearButton");
    g_signal_connect(clearPath, "clicked", G_CALLBACK(clear_callback), application);
    //text description of directions for the path
    GtkButton* directions = (GtkButton*) application->get_object("DirectionButton");
    g_signal_connect(directions, "clicked", G_CALLBACK(directions_callback), application);
}


void findPath(GtkWidget* /*widget*/, ezgl::application *app){
    
    //Gets and stores user input
    GtkEntry* sEntry1 = GTK_ENTRY(app->get_object("sBar1"));
    GtkEntry* sEntry2 = GTK_ENTRY(app->get_object("sBar2"));
    
    const gchar* intersectionName1 = gtk_entry_get_text(sEntry1);
    const gchar* intersectionName2 = gtk_entry_get_text(sEntry2);
    
     std::pair<IntersectionIdx, IntersectionIdx> Intersections;
     // Find the StreetIdx values of all streets that match the entered names
     //std::vector<StreetIdx> findIntersectionIdsFromPartialStreetName(std::string street_prefix)
   std::vector<StreetIdx> name1Idx = findIntersectionIdsFromPartialStreetName(intersectionName1);
    std::vector<StreetIdx> name2Idx = findIntersectionIdsFromPartialStreetName(intersectionName2);
    // If there is only one matching street for name1, add it to name1Idxcopy
    // Otherwise, check if all matching streets have the same name. If they do, add all of them to name1Idxcopy.
    // If they don't, add only the streets whose names exactly match s1 to name1Idxcopy. Same for name2
    bool same1 = true;
    bool same2 = true;
    if(name1Idx.size() >1){
        valid = true;
        for(int j = 0; j < name1Idx.size()-1; j++){
            if(getIntersectionName(name1Idx[j]) != getIntersectionName(name1Idx[j+1]))
                same1 = false;
        }        
    }
    
 
    if(map_data.search1clicked){
        name1Idx[0] = map_data.search1id;
    }
    if(map_data.search2clicked){
        name2Idx[0] = map_data.search2id;
    }
   if(same1 && same2 && name1Idx[0] != 0 && name2Idx[0] !=0){
        Intersections.first = name1Idx[0];
        Intersections.second = name2Idx[0];
   }else{
    bool valid = false;
   }
    
    app->refresh_drawing();
    name1Idx.clear();
    name2Idx.clear();
    map_data.Path = findPathBetweenIntersections(Intersections, 15.0000000);
            
}

void city_select_callback(GtkComboBox *box, gpointer data){
    auto app = static_cast<ezgl::application *> (data);
    GtkComboBox* city_drop_down = (GtkComboBox*) app->get_object("CityDropDown");
    gint which_map = gtk_combo_box_get_active(city_drop_down) + 1;
    *box = *box;
    if(!map_data.load_successful) return;
    closeMap();
    switch (which_map) {
        case 1:
            loadMap("/cad2/ece297s/public/maps/beijing_china.streets.bin");
            break;
        case 2:
            loadMap("/cad2/ece297s/public/maps/cairo_egypt.streets.bin");
            break;
        case 3:
            loadMap("/cad2/ece297s/public/maps/cape-town_south_africa.streets.bin");
            break;
        case 4:
            loadMap("/cad2/ece297s/public/maps/hong-kong_china.streets.bin");
            break;
        case 5:
            loadMap("/cad2/ece297s/public/maps/iceland.streets.bin");
            break;
        case 6:
            loadMap("/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin");
            break;
        case 7:
            loadMap("/cad2/ece297s/public/maps/kyiv_ukraine.streets.bin");
            break; 
        case 8:
            loadMap("/cad2/ece297s/public/maps/london_england.streets.bin");
            break;
        case 9:
            loadMap("/cad2/ece297s/public/maps/new-delhi_india.streets.bin");
            break; 
        case 10:
            loadMap("/cad2/ece297s/public/maps/new-york_usa.streets.bin");
            break;
        case 11:
            loadMap("/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin");
            break;
        case 12:
            loadMap("/cad2/ece297s/public/maps/saint-helena.streets.bin");
            break; 
        case 13:
            loadMap("/cad2/ece297s/public/maps/singapore.streets.bin");
            break; 
        case 14:
            loadMap("/cad2/ece297s/public/maps/sydney_australia.streets.bin");
            break;
        case 15:
            loadMap("/cad2/ece297s/public/maps/tehran_iran.streets.bin");
            break;
        case 16:
            loadMap("/cad2/ece297s/public/maps/tokyo_japan.streets.bin");
            break;
        case 17:
            loadMap("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
            break; 
        case 18:
            loadMap("/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin");
            break; 
        default:
            loadMap("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
            break;   
    }
    ezgl::rectangle resize_world({x_from_latlon(map_data.min_lon),y_from_latlon(map_data.min_lat)},
                                 {x_from_latlon(map_data.max_lon),y_from_latlon(map_data.max_lat)});
    app->change_canvas_world_coordinates("MainCanvas", resize_world);
    app->refresh_drawing();
    
}
 

void drawMap() {
   ezgl::application::settings settings;
   settings.main_ui_resource = "libstreetmap/resources/main.ui";
   settings.window_identifier = "MainWindow";
   settings.canvas_identifier = "MainCanvas";
   ezgl::application mapper(settings);
   ezgl::rectangle initial_world({x_from_latlon(map_data.min_lon),y_from_latlon(map_data.min_lat)},
                                 {x_from_latlon(map_data.max_lon),y_from_latlon(map_data.max_lat)});
   map_data.initialArea = initial_world.area();
    ezgl::color background = ezgl::color(220,228,228);
   mapper.add_canvas("MainCanvas", draw_main_canvas, initial_world, background);
   mapper.run(initial_setup, act_on_mouse_click, nullptr, nullptr);
}

//clicking on the map to show effect
void act_on_mouse_click (ezgl::application *app, GdkEventButton */*event*/, double x, double y){
    
    GtkEntry* intersectionFrom = (GtkEntry*) app->get_object("sBar1");
    GtkEntry* intersectionTo = (GtkEntry*) app->get_object("sBar2");
    
    std::cout << "Mouse clicked at (" << x << "," << y << ") \n";  
    LatLon pos = LatLon(latlon_from_y(y), latlon_from_x(x));
    map_data.inter_id = findClosestIntersection(pos);
     std::cout << "Intersection ID:" << map_data.inter_id  << "\n";  
    
    if(gtk_widget_has_focus(GTK_WIDGET(intersectionTo))){
        gtk_entry_set_text(intersectionTo , map_data.intersections_Info[map_data.inter_id].name.c_str());
        map_data.endpoint = map_data.intersections_Info[map_data.inter_id].xy_loc;
        map_data.search1clicked = true;
        map_data.search1id = map_data.inter_id;
    }
    else{
        gtk_entry_set_text(intersectionFrom , map_data.intersections_Info[map_data.inter_id].name.c_str());
        map_data.startpoint = map_data.intersections_Info[map_data.inter_id].xy_loc;
        map_data.search2clicked = true;
        map_data.search2id = map_data.inter_id;

    }
    std::stringstream ss;
    ss << "Closest Intersection: " << map_data.intersections_Info[map_data.inter_id].name << std::endl;
    app->update_message(ss.str()); //message on the ui
    app->refresh_drawing();
}
//function to setup the canvas(draw the bound of maps)
void draw_main_canvas(ezgl::renderer *g){
    ezgl::rectangle r1 = g-> get_visible_world();
    double currArea = r1.area();
   map_data.zoomfactor = (currArea/map_data.initialArea);
   g->set_color(0,0,0);
   g->set_line_width(1);
   draw_features(g);
   draw_streets(g);
   draw_path(g,map_data.Path);
   draw_street_name(g);
   draw_placeholder(g);
   draw_intersections(g);
   draw_POI_name(g);
   draw_road_closure(g);
   draw_parking(g);
   drawScale(g);
   
}



