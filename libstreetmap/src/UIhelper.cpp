/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */
#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include "LatLon.h"
#include "m3.h"
#include "m2.h"
#include "m1.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <sstream>
#include "helper.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include "libcurl.h"
#include "drawing.h"
#include "loading.h"
#include "UIhelper.h"

void clear_callback(GtkWidget* /*widget*/, ezgl::application* app) {
    GtkEntry* intersectionFrom = (GtkEntry*) app->get_object("sBar1");
    GtkEntry* intersectionTo = (GtkEntry*) app->get_object("sBar2");
    // set the text of the search entry to an empty string
    gtk_entry_set_text(GTK_ENTRY(intersectionFrom), "");
    gtk_entry_set_text(GTK_ENTRY(intersectionTo), "");
    map_data.startpoint = {0, 0};
    map_data.endpoint = {0, 0};
    map_data.Path = {-1};
    app->refresh_drawing();
}

void help_callback(GtkWidget* /*widget*/, ezgl::application* app) {
    std::stringstream ss;
    ss << "Welcome to DriveAssist! We are here to help you navigate our map interface." << std::endl
            << " Here's a brief guide to get you started: " << std::endl;

    ss << std::endl
            << "Search for Routes: On the top left of the map, you will see two search bars where you can" << std::endl
            << "input the two intersections you want to travel between. Once you have entered the two " << std::endl
            << "intersections, click the search icon beside it, and our map will display the fastest route " << std::endl
            << "between the two points. If you want to delete the current search related information, " << std::endl
            << "click the x icon below the search icon." << std::endl;

    ss << std::endl
            << "Searching via Map: Another option to search for a route is to click on the map. If you click" << std::endl
            << "on the map while the first search bar is active, the intersection name will automatically be" << std::endl
            << "entered into the first search bar, and a placeholder icon will be displayed on the map to" << std::endl
            << "indicate the location you have selected. Repeat the same steps for the second search bar," << std::endl
            << "and then you can navigate the same way with the search icon and X icon." << std::endl;

    ss << std::endl
            << "Load Cities: On the top right of the map, you will find a red X button to close the map, " << std::endl
            << "and to its left, there is a drop-down menu where you can select the city you want to load " << std::endl
            << "on the map. Simply click on the menu, and select the city of your choice." << std::endl;

    ss << std::endl
            << "Colorblind Mode and Weather: On the bottom left of the map, you will find two icons - a " << std::endl
            << "color palette and a weather icon. Clicking on the color palette icon will activate colorblind " << std::endl
            << "mode for the map, displaying the map with more contrast in the color scheme. The " << std::endl
            << "weather icon will show you weather-related information about the currently loaded city. " << std::endl;

    ss << std::endl
            << "Zooming: On the bottom right of the map, you will find an icon that, when clicked, will fit " << std::endl
            << "the map to your screen. This is a zoom-fit button that will help you see the entire map on " << std::endl
            << "your screen." << std::endl;

    ss << std::endl
            << "We hope this guide has helped you understand how to use DriveAssist. If you have any" << std::endl
            << "questions or need further assistance, please feel free to click on the Help button on the " << std::endl
            << "top right of the map again. Happy navigating!" << std::endl;

    app->create_popup_message("Help", ss.str().c_str());
}

void colorblind_callback(GtkWidget* /*widget*/, ezgl::application* app) {
    map_data.blind = !(map_data.blind);
    app->refresh_drawing();
}

void directions_callback(GtkWidget* /*widget*/, ezgl::application* app) {
    std::vector<StreetSegmentIdx> comparator;
    std::stringstream ss;
    comparator.push_back(-1);
    if (map_data.Path != comparator) {
        
        std::vector<StreetSegmentIdx> path = map_data.Path;
        //travel time in minutes
        std::reverse(path.begin(), path.end());
        double travel_time = (computePathTravelTime(path, 15.0000000000000000)) / 60;
        int path_size = path.size();
        StreetSegmentIdx startpoint = path.at(0);
        StreetSegmentIdx endpoint = path.at(path_size - 1);
        StreetIdx startstreet_id = map_data.street_Segs_Info[startpoint].streetID;
        StreetIdx endstreet_id = map_data.street_Segs_Info[endpoint].streetID;
        std::string startstreet = getStreetName(startstreet_id);
        std::string endstreet = getStreetName(endstreet_id);
        double Distance = distance(startpoint);
        ss << travel_time << "minutes from " << startstreet << " to "
                << endstreet << std::endl;

        ss << std::endl << "Head " << findDirection(startpoint, endpoint) << " on " << startstreet << endl;
        for (int i = 1; i < path_size; i++) {
            StreetSegmentIdx current = path[i];
            StreetSegmentIdx previous = path[i - 1];
            if (map_data.street_Segs_Info[current].streetID != map_data.street_Segs_Info[previous].streetID) {
                ss << "(" << Distance << "m)" << endl;
                Distance = 0;
                Distance += distance(current);
                /*std::string which_way = direction(previous, current);*/
                ss << std::endl << "turn " << findRightOrLeft(previous, current) << " onto "
                        << getStreetName(map_data.street_Segs_Info[current].streetID) << endl;
            }
            if (i == path_size - 1) {
                ss << "(" << Distance << "m)" << endl;
            }
            path.clear();
        }

        app->create_popup_message("Directions", ss.str().c_str());
    } else {
         ss <<"   No path has been searched   " << endl;
        app->create_popup_message("Directions", ss.str().c_str());
    }
}

double distance(StreetSegmentIdx seg) {
    IntersectionIdx current = map_data.street_Segs_Info[seg].from;
    IntersectionIdx finish = map_data.street_Segs_Info[seg].to;
    ezgl::point2d start = map_data.intersections_Info[current].xy_loc;
    ezgl::point2d goal = map_data.intersections_Info[finish].xy_loc;
    double dx = goal.x - start.x;
    double dy = goal.y - start.y;
    //length formula
    return sqrt(pow(dx, 2.0) + pow(dy, 2.0));
}
