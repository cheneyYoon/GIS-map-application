#ifndef LOADING_H
#define LOADING_H
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


using namespace std;


//struct from m1
struct SegmentInfo{
    StreetIdx street;
    double travel_time;
    double length;
    StreetSegmentIdx streetsegmentIdx;
};

//struct from m2
struct Streets_data{
    ezgl::point2d xy_loc = {0,0};
    std::string name;
    float max_speed; 
    int index;
};
struct Intersection_data{
    ezgl::point2d xy_loc = {0,0};
    std::string name;
    bool highlight = false;
    bool found = false;
    bool selected = false;
    int index;
};
struct Feature_data{
    std::string name;
    FeatureType type;
    std::vector <ezgl::point2d> featurePoints;
    int numPoints;
    bool closed = false;
    double area;
};
struct POI_data{
    std::string name;
    std::string type;
    ezgl::point2d xy_loc;
};

struct delivery_info{
    bool delivered = false;
    bool retrieved = false;
    bool delivered_2 = false;
    bool retrieved_2 = false;
};

//void loadOsmNodes();
void preload_streets();
void preload_intersections();
void preload_features();
void preload_POI();
#endif