#include "LatLon.h"
#include "m2.h"
#include "m1.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <sstream>
#include "helper.h"
#include <unordered_map>
#include <stdbool.h>
#include "libcurl.h"
#include <algorithm>
#include "loading.h"
#include "drawing.h"



using namespace std;

void draw_features(ezgl::renderer *g) {
    // Loop through all features
    for (int i = 0; i < map_data.features_Info.size(); i++) {
        // If the feature has more than one point
        if (map_data.features_Info[i].featurePoints.size() > 1) {
            // Set color based on the feature type
            if (map_data.features_Info[i].type == PARK || map_data.features_Info[i].type == GREENSPACE || map_data.features_Info[i].type == GOLFCOURSE || map_data.features_Info[i].type == ISLAND) {
                // Set the fill color for parks, greenspaces, golf courses, and islands
                g->set_color(206, 234, 214, 255);
                // If the map is in blind mode, change the fill color
                if (map_data.blind) {
                    g->set_color(0, 155, 125);
                }
                // If the map is zoomed in and not in blind mode, change the fill color
                if (map_data.zoomfactor > .03 && !map_data.blind) {
                    g->set_color(172, 220, 180, 255);
                }
                // Call the fill_functions method to draw the feature
                fill_functions(map_data.features_Info[i], g, i);
            } else if (map_data.features_Info[i].type == LAKE || map_data.features_Info[i].type == RIVER) {
                // Set the fill color for lakes and rivers
                g->set_color(156, 192, 249);
                // If the map is in blind mode, change the fill color
                if (map_data.blind) {
                    g->set_color(213, 142, 214);
                }
                // Call the fill_functions method to draw the feature
                fill_functions(map_data.features_Info[i], g, i);
            } else if (map_data.features_Info[i].type == STREAM && map_data.zoomfactor < .05) {
                // Set the fill color for streams if the map is zoomed out
                g->set_color(156, 192, 249);
                // If the map is in blind mode, change the fill color
                if (map_data.blind) {
                    g->set_color(213, 142, 214);
                }
                // Call the fill_functions method to draw the feature
                fill_functions(map_data.features_Info[i], g, i);
            } else if (map_data.features_Info[i].type == BEACH) {
                // Set the fill color for beaches
                g->set_color(254, 239, 195, 255);
                // If the map is in blind mode, change the fill color
                if (map_data.blind) {
                    g->set_color(255, 200, 87);
                }
                // Call the fill_functions method to draw the feature
                fill_functions(map_data.features_Info[i], g, i);
            } else if (map_data.features_Info[i].type == BUILDING && g->get_visible_world().area() < 2000000) {
                // Set the fill color for buildings if the visible world is small
                g->set_color(241, 243, 244, 255);
                // If the map is in blind mode, change the fill color
                if (map_data.blind) {
                    g->set_color(ezgl::BLACK);
                }
                // Call the fill_functions method to draw the feature
                fill_functions(map_data.features_Info[i], g, i);
            } else if (map_data.features_Info[i].type == GLACIER) {
                // Set the fill color for glaciers
                g->set_color(204, 255, 255, 255);
                if (map_data.blind)
                    g->set_color(ezgl::BLUE);
                fill_functions(map_data.features_Info[i], g, i);
                //default
            } else if (map_data.features_Info[i].type == UNKNOWN) {
                g->set_color(0, 0, 0);
                fill_functions(map_data.features_Info[i], g, i);
            }
        }
    }
}
// This function is responsible for rendering a given feature on the map.
// It takes a Feature_data object, a renderer object, and an index as input.

void fill_functions(Feature_data feature, ezgl::renderer *g, int /*index*/) {

    // If the feature is closed, it is a polygon and we can fill it
    if (feature.closed) {
        g-> fill_poly(feature.featurePoints);
    }        // If the feature is not closed, it is a polyline and we need to draw each segment
    else {
        // Set the line width to 0 to draw a thin line
        g->set_line_width(0);
        for (int j = 0; j < feature.featurePoints.size() - 1; j++) {
            // Draw a line segment between each pair of consecutive points
            g->draw_line(feature.featurePoints[j], feature.featurePoints[j + 1]);
        }
    }
}


// This function is responsible for drawing all points of interest (POIs) on the map.
// It takes a renderer object as input.

void draw_POI(ezgl::renderer *g) {

    // Loop through all POIs and draw a rectangle at their xy location
    for (int i = 0; i < getNumPointsOfInterest(); i++) {
        // Set the color to purple
        g->set_color(ezgl::PURPLE);
        // Define the width and height of the rectangle
        float width = 10;
        float height = width;
        // Draw a filled rectangle at the POI's xy location
        g->fill_rectangle(map_data.POI_Info[i].xy_loc, width, height);
    }
}

void draw_road_closure(ezgl::renderer* g) {
    // Get the current visible rectangle in the map
    ezgl::rectangle rec = g->get_visible_world();
    // Load the image for the road closure
    ezgl::surface *closure = ezgl::renderer::load_png("libstreetmap/resources/closure.png");
    // Check if the map is zoomed in enough to show the road closures
    if (map_data.zoomfactor < .02) {
        // Loop through all the road closure info objects
        for (int cur = 0; cur < closure_Info.size(); cur++) {
            // Check if the current road closure is within the visible rectangle
            if (withinBounds(rec, closure_Info[cur].xy_loc)) {
                // Draw the road closure image at the location of the closure info object
                g->draw_surface(closure, closure_Info[cur].xy_loc, 0.02);
            }
        }
    }
}

void draw_placeholder(ezgl::renderer* g) {
    ezgl::point2d basic {0,0};
    ezgl::surface *placeholder = ezgl::renderer::load_png("libstreetmap/resources/placeholder.png");
    if (map_data.zoomfactor < map_data.zoom_level_8) {
        g->draw_surface(placeholder,  map_data.startpoint, 0.6);
    } else if ( map_data.zoomfactor < map_data.zoom_level_4) {
        g->draw_surface(placeholder, map_data.startpoint, 0.4);
    } else if (map_data.startpoint != basic ) {
        g->draw_surface(placeholder, map_data.startpoint, 0.3);
    }
    if (map_data.zoomfactor < map_data.zoom_level_8) {
        g->draw_surface(placeholder,  map_data.endpoint, 0.6);
    } else if ( map_data.zoomfactor < map_data.zoom_level_4) {
        g->draw_surface(placeholder, map_data.endpoint, 0.4);
    } else if (map_data.endpoint != basic ) {
        g->draw_surface(placeholder, map_data.endpoint, 0.3);
    }
}

void draw_parking(ezgl::renderer* g) {
    //cout<<closure_Info.size();
    ezgl::rectangle rec = g-> get_visible_world();
    //cout<<"im here";
    ezgl::surface *parking = ezgl::renderer::load_png("libstreetmap/resources/parking.png");
    if (map_data.zoomfactor < .03) {
        for (int cur = 0; cur < parking_Info.size(); cur++) {
            //cout<<"monkey";
            if (withinBounds(rec, parking_Info[cur].xy_loc)) {
                //            cout<<"imhere";
                //            cout<<closure_Info[cur].name;
                //            LatLon LLposition(map_data.min_lat, map_data.min_lon);
                g->draw_surface(parking, parking_Info[cur].xy_loc, 0.03);
            }
        }
    }
}

void drawScale(ezgl::renderer* g) {
    // Get the visible world rectangle
    ezgl::rectangle recta(g->get_visible_world());
    // Calculate the coordinates for the scale bar
    ezgl::point2d bLeft = recta.bottom_left();
    ezgl::point2d bRight = recta.bottom_right();
    ezgl::point2d tLeft = recta.top_left();
    //Calculates the coordinates for the scale bar
    double startX = bLeft.x + ((bRight.x - bLeft.x) / 10);
    double startY = bLeft.y + ((tLeft.y - bLeft.y) / 10);
    double endX = bLeft.x + (2 * ((bRight.x - bLeft.x) / 10));
    double endY = bLeft.y + ((tLeft.y - bLeft.y) / 10);
    double dY = startY - ((startY - bLeft.y) / 10);
    // Draw the scale bar line
    g->set_line_width(2);
    g->set_color(ezgl::BLACK);
    g->draw_line(ezgl::point2d(startX, startY), ezgl::point2d(endX, endY));
    // Convert the distance to a string for display on the map
    std::string distance = std::to_string(int(endX - startX));
    std::string Z = std::to_string(0);
    // Draw the distance labels for the scale bar
    g->draw_text(ezgl::point2d(startX, dY - ((startY - bLeft.y) / 10)), Z, 7 * (startY - dY), 7 * (startY - dY));
    g->draw_text(ezgl::point2d(endX, dY - ((startY - bLeft.y) / 10)), distance, 7 * (startY - dY), 7 * (startY - dY));

}

//draws all streets

void draw_streets(ezgl::renderer *g) {
    map_data.latAvg = get_latAvg();
    //       load streets based on zoom level
    if (map_data.zoomfactor < map_data.zoom_level_10) {
        draw_for_street_type(g, "minor", ezgl::WHITE, 8);
        draw_for_street_type(g, "major", ezgl::WHITE, 12);
        draw_for_street_type(g, "highway", map_data.highway_yellow, 12);
    } else if (map_data.zoomfactor < map_data.zoom_level_8) {
        draw_for_street_type(g, "minor", ezgl::WHITE, 5);
        draw_for_street_type(g, "major", ezgl::WHITE, 10);
        draw_for_street_type(g, "highway", map_data.highway_yellow, 10);
    } else if (map_data.zoomfactor < map_data.zoom_level_6) {
        draw_for_street_type(g, "minor", ezgl::WHITE, 2);
        draw_for_street_type(g, "major", ezgl::WHITE, 4);
        draw_for_street_type(g, "highway", map_data.highway_yellow, 6);
    }        //minor streets loaded from this level
    else if (map_data.zoomfactor < map_data.zoom_level_4) {
        draw_for_street_type(g, "minor", ezgl::WHITE, 1);
        draw_for_street_type(g, "major", ezgl::WHITE, 2);
        draw_for_street_type(g, "highway", map_data.highway_yellow, 3);
    } else if (map_data.zoomfactor < map_data.zoom_level_3) {
        draw_for_street_type(g, "major", ezgl::WHITE, 1);
        draw_for_street_type(g, "highway", map_data.highway_yellow, 2);
    } else if (map_data.zoomfactor < map_data.zoom_level_1) {
        draw_for_street_type(g, "major", ezgl::WHITE, 0);
        draw_for_street_type(g, "highway", map_data.highway_yellow, 1);
    }        //only load highway at the highest level
    else {
        draw_for_street_type(g, "highway", map_data.highway_yellow, 0);
    }


}

void draw_for_street_type(ezgl::renderer *g, std::string street_type, ezgl::color street_color, int width) {
    for (int id = 0; id < getNumStreetSegments(); id++) {
        //render only if in range
        ezgl::rectangle r = g->get_visible_world();
        double x1, x2, y1, y2;
        x1 = r.m_first.x;
        y1 = r.m_first.y;
        x2 = r.m_second.x;
        y2 = r.m_second.y;
        bool in_view = false;
        ezgl::point2d start = map_data.intersections_Info[map_data.street_Segs_Info[id].from].xy_loc;
        ezgl::point2d finish = map_data.intersections_Info[map_data.street_Segs_Info[id].to].xy_loc;
        //when zoomed in, divide up the segment into smaller pieces to check if these pieces are
        //in the range of visible map. This ensures long streets to be not cut off.
        if (map_data.zoomfactor < map_data.zoom_level_8) {
            ezgl::point2d increment = ezgl::point2d(((finish.x - start.x) / map_data.num_smaller_segments),
                    ((finish.y - start.y) / map_data.num_smaller_segments));
            for (int i = 0; i < map_data.num_smaller_segments; i++) {
                ezgl::point2d mid_point = ezgl::point2d((start.x + increment.x * i),
                        (start.y + increment.y * i));
                if (mid_point.x > x1 && mid_point.x < x2 && mid_point.y > y1 && mid_point.y < y2) {
                    in_view = true;
                    break;
                }
            }
        }            //if map is zoomed out, simply check if either starting and ending point in in the map
        else {
            if ((start.x > x1 && start.x < x2 && start.y > y1 && start.y < y2) ||
                    (finish.x > x1 && finish.x < x2 && finish.y > y1 && finish.y < y2)) {
                in_view = true;
            }
        }
        //if the street segment is within the range of visible part, continue
        if (in_view) {
            //store all needed information about the current street
            int num_curve_points = map_data.street_Segs_Info[id].numCurvePoints;
            OSMID street_osmid = map_data.street_Segs_Info[id].wayOSMID;
            //determine line width and color based on street type
            if (type_of_street(street_osmid) == street_type) {
                g->set_color(street_color);
                g->set_line_width(width);
                g->set_line_cap(ezgl::line_cap(1));
                if (num_curve_points == 0) {
                    g->draw_line(start, finish);
                }                    //for a street segment without a curve
                else {
                    // If the street segment is a curve, draw it using multiple line segments
                    auto prev_point = start;
                    for (int i = 0; i < num_curve_points; i++) {
                        const auto& finish_latlon = getStreetSegmentCurvePoint(id, i);
                        const auto& temp_finish = ezgl::point2d(x_from_latlon(finish_latlon.longitude())
                                , y_from_latlon(finish_latlon.latitude()));
                        g->draw_line(prev_point, temp_finish);
                        prev_point = temp_finish;
                    }
                    g->draw_line(prev_point, finish);
                }
            }
        }

    }
}

//prints the street name in the center of the street

void draw_street_name(ezgl::renderer *g) {
    for (int id = 0; id < getNumStreetSegments(); id++) {
        float street_ID = map_data.street_Segs_Info[id].streetID;
        std::string streetName = map_data.streets_Info[street_ID].name;
        //render only if in range
        ezgl::rectangle r = g->get_visible_world();
        double x1, x2, y1, y2;
        x1 = r.m_first.x;
        y1 = r.m_first.y;
        x2 = r.m_second.x;
        y2 = r.m_second.y;
        ezgl::point2d sPos = center_of_street(id);
        double x = sPos.x;
        double y = sPos.y;
        if (x > x1 && x < x2 && y > y1 && y < y2) {
            //print only every other segment
            if (id % 2 == 0) {
                //do not print if street name is unknown
                if (streetName != "<unknown>") {
                    g->set_color(map_data.street_text);
                    //add arrow for one way streets
                    if (map_data.street_Segs_Info[id].oneWay) {
                        streetName = one_way_street_name(id, streetName);
                    }
                    //printing street name for straight streets
                    if (map_data.street_Segs_Info[id].numCurvePoints == 0) {
                        g->set_text_rotation(angle_of_street(id));
                        g->draw_text(center_of_street(id), streetName, x_boundary(id), 30);
                    }                        //printing street name for curves
                    else {
                        g->set_text_rotation(angle_of_street(id));
                        g->draw_text(center_of_curved_street(id), streetName, x_boundary(id), 30);
                    }
                }
            }
        }
    }
}
//prints the POI name at the point that POI exist

void draw_POI_name(ezgl::renderer *g) {
    for (int id = 0; id < getNumPointsOfInterest(); id++) {
        std::string POIName = map_data.POI_Info[id].name;
        //render only if in range
        ezgl::rectangle r = g->get_visible_world();
        double x1, x2, y1, y2;
        x1 = r.m_first.x;
        y1 = r.m_first.y;
        x2 = r.m_second.x;
        y2 = r.m_second.y;
        ezgl::point2d POI_position = map_data.POI_Info[id].xy_loc;
        double x = POI_position.x;
        double y = POI_position.y;
        if (x > x1 && x < x2 && y > y1 && y < y2) {
            if (POIName != "<unknown>") {
                g->set_text_rotation(0);
                g->set_color(map_data.POI_text);
                //10 is width and height for all POIs
                g->draw_text(POI_position, POIName, 10, 10);
            }

        }
    }
}

//functions that draws all the intersections

void draw_intersections(ezgl::renderer *g) {
    for (int id = 0; id < map_data.intersections_Info.size(); ++id) {
        if (map_data.zoomfactor < .01) {
            if (map_data.intersections_Info[id].highlight) {
                g->set_color(ezgl::RED);
                if (map_data.intersections_Info[id].found)
                    g->set_color(ezgl::PINK);
                double width = 4;
                double height = width;
                ezgl::point2d inter_loc = map_data.intersections_Info[id].xy_loc - ezgl::point2d{width / 2, height / 2};
                g->fill_rectangle(inter_loc, width, height);
            }
        }
    }
}

void draw_path(ezgl::renderer* g, std::vector<StreetSegmentIdx> path) {
    int path_size = path.size();
    for (int i = 0; i < path_size; i++) {
        StreetSegmentIdx id = path.at(i);
        ezgl::point2d start = map_data.intersections_Info[map_data.street_Segs_Info[id].from].xy_loc;
        ezgl::point2d finish = map_data.intersections_Info[map_data.street_Segs_Info[id].to].xy_loc;
        int num_curve_points = map_data.street_Segs_Info[id].numCurvePoints;
        g->set_color(map_data.path_color);
        g->set_line_width(line_width());
        g->set_line_cap(ezgl::line_cap(1));
        if (num_curve_points == 0) {
            g->draw_line(start, finish);
        }            //for a street segment without a curve
        else {
            // If the street segment is a curve, draw it using multiple line segments
            auto prev_point = start;
            for (int curve_idx = 0; curve_idx < num_curve_points; curve_idx++) {
                const auto& finish_latlon = getStreetSegmentCurvePoint(id, curve_idx);
                const auto& temp_finish = ezgl::point2d(x_from_latlon(finish_latlon.longitude())
                        , y_from_latlon(finish_latlon.latitude()));
                g->draw_line(prev_point, temp_finish);
                prev_point = temp_finish;
            }
            g->draw_line(prev_point, finish);
        }

    }

}