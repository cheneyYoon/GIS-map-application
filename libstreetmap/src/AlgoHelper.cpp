/* 
 * File:   AlgoHelper.cpp
 * Author: yooncha5
 *
 * Created on March 30, 2023, 9:35 AM
 */


#include "AlgoHelper.h"
#include "m3.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "helper.h"
#include <vector>
#include <iostream>
#include "drawing.h"
#include "loading.h"
#include "StreetsDatabaseAPI.h"

//calculates the heuristic cost with maximum speed limit

float heuristic_cost(IntersectionIdx current, IntersectionIdx finish) {
    return (distance(current, finish) / map_data.map_max_speed);
    //return findDistanceBetweenTwoPoints(getIntersectionPosition(current),getIntersectionPosition(finish))/(map_data.map_max_speed);
}

//use Pythagorean theorem to calculate the distance in between the two points 

float distance(IntersectionIdx current, IntersectionIdx finish) {
    ezgl::point2d start = map_data.intersections_Info[current].xy_loc;
    ezgl::point2d goal = map_data.intersections_Info[finish].xy_loc;
    double dx = goal.x - start.x;
    double dy = goal.y - start.y;
    //length formula
    return sqrt(pow(dx, 2.0) + pow(dy, 2.0));
}



//if (current_segments[segment_id] == street_Segs_Info[segment_id].from && finish == to_id) {
std::vector<int> getStreetSegmentFromIdx(IntersectionIdx current, IntersectionIdx finish) {
    std::vector<int> returnVec;
    std::vector<StreetSegmentIdx> current_segments = findStreetSegmentsOfIntersection(current);
    for (auto segment_id : current_segments)
    {
        // get info
        if (current == map_data.street_Segs_Info[segment_id].from && finish == map_data.street_Segs_Info[segment_id].to) {
            returnVec.push_back(segment_id);// record
        } else if (map_data.street_Segs_Info[segment_id].oneWay == false && current == map_data.street_Segs_Info[segment_id].to 
                && finish == map_data.street_Segs_Info[segment_id].from) {
            // record
            returnVec.push_back(segment_id);
            //return segment_id;
        }
       
    }
    
    return returnVec;
}
