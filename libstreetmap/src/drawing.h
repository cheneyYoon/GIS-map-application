#ifndef DRAWING_H
#define DRAWING_H
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

void draw_streets(ezgl::renderer *g);
void draw_street_name(ezgl::renderer *g);
void draw_intersections(ezgl::renderer *g);
void draw_features(ezgl::renderer *g);
void draw_POI(ezgl::renderer *g);
void draw_POI_name(ezgl::renderer *g);
void draw_parking(ezgl::renderer* g);
void draw_for_street_type (ezgl::renderer *g, std::string street_type, ezgl::color street_color, int width);
void fill_functions(Feature_data feature, ezgl::renderer *g, int /*index*/);
void drawScale(ezgl::renderer* g);
void draw_placeholder(ezgl::renderer* g);
void draw_path(ezgl::renderer* g, std::vector<StreetSegmentIdx> path);
#endif