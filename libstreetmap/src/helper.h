/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   helper.h
 * Author: fanshuh2
 *
 * Created on March 10, 2023, 4:52 PM
 */

#ifndef HELPER_H
#define HELPER_H
#include "LatLon.h"
#include "m2.h"
#include "m1.h"
#include "loading.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <sstream>
#include <stdbool.h>
#include <set>
#include <algorithm>


struct global_var{
        //global variables from m1
    std::vector<std::vector<StreetSegmentIdx>> intersectionStreetSegs;
    std::unordered_map<StreetIdx, std::vector<StreetSegmentIdx>> streetMap;
    std::unordered_map<StreetIdx, std::vector<IntersectionIdx>> intersectionMap;
    std::unordered_map<std::string, std::vector<StreetIdx>> nameMap;
    std::unordered_map<std::string, std::vector<IntersectionIdx>> intersectionNameMap;
    std::vector<SegmentInfo> segments;
    std::unordered_map<OSMID,const OSMNode*> osmNodes;
    std::unordered_map<OSMID,const OSMWay*> osmWays;
    //global variables from m2
    std::vector<Intersection_data> more_Streets_Info;
    std::vector<Streets_data> streets_Info;
    std::vector<StreetSegmentInfo> street_Segs_Info;
    std::vector<Intersection_data> intersections_Info;
    std::vector<Feature_data> features_Info;
    std::vector<POI_data> POI_Info;
    std::vector<StreetSegmentIdx> Path = {-1};
    ezgl::point2d startpoint = {0,0};
    ezgl::point2d endpoint = {0,0};
    int pathFindState;
    int inter_id;
    bool blind;
    bool search1clicked;
    bool search2clicked;
    IntersectionIdx search1id;
    IntersectionIdx search2id;  
    double initialArea;
    float latAvg;
    float lonAvg;
    double max_lat;
    double min_lat;
    double max_lon;
    double min_lon;
    float map_max_speed;
    bool load_successful = false;
    double zoomfactor;
    const double num_smaller_segments = 10;
    const float zoom_level_1 = 0.5;
    const float zoom_level_2 = 0.2;
    const float zoom_level_3 = 0.06;
    const float zoom_level_4 = 0.02;
    const float zoom_level_5 = 0.007;
    const float zoom_level_6 = 0.003;
    const float zoom_level_7 = 0.001;
    const float zoom_level_8 = 0.0005;
    const float zoom_level_9 = 0.0002;
    const float zoom_level_10 = 0.00005;
    const float zoom_level_11 = 0.00002;
    std::string city_name = "toronto";
    ezgl::color street_text = ezgl::color(116, 120, 123);
    ezgl::color POI_text = ezgl::color(53, 56, 57);
    ezgl::color highway_yellow = ezgl::color(246, 207, 101);
    ezgl::color path_color = ezgl::color(8, 180, 252);
};


extern global_var map_data;


//functions from m1
void computeStreetSegInfo();
void findSegmentsofIntersections();
void findAllPartialNames();
void findAllPartialNamesIntersections();
double findTrapArea(LatLon x, LatLon y);
// void loadOsmNodes();
bool withinBounds(ezgl::rectangle rec, ezgl::point2d xy_loc);
ezgl::point2d xy_from_latlon(LatLon loc);
std::vector<double> latlonToPoint(LatLon pointA, LatLon pointB);
void draw_road_closure(ezgl::renderer *g);
std::string getOSMWayTagValue (OSMID OSMid, std::string key);


//functions from m2
bool compareArea(const Feature_data &a, const Feature_data &b);
std::vector<StreetIdx> findIntersectionIdsFromPartialStreetName(std::string street_prefix);
void draw_main_canvas(ezgl::renderer *g);
float x_from_latlon(float lon);
float y_from_latlon(float lat);
float latlon_from_x(float x);
float latlon_from_y(float y);
float get_latAvg ();
float get_lonAvg ();
bool outBounds(ezgl::rectangle bounds, ezgl::point2d xy_loc);
std:: string findDirection(StreetSegmentIdx id1,StreetSegmentIdx id2);
std::string one_way_street_name (StreetSegmentIdx id, std::string one_way_street);
std::string findRightOrLeft (StreetSegmentIdx from, StreetSegmentIdx to);
ezgl::point2d center_of_street(StreetSegmentIdx id);
ezgl::point2d center_of_curved_street(StreetSegmentIdx id);
double angle_of_street (StreetSegmentIdx id);
double angle_of_curved_street(StreetSegmentIdx id);
double x_boundary (StreetSegmentIdx id);
std::string type_of_street(OSMID OSMid);
void language_setting(ezgl::renderer *g, std::string city);
int line_width();
#endif /* HELPER_H */

