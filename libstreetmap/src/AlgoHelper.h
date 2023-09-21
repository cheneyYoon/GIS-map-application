/* 
 * File:   AlgoHelper.h
 * Author: yooncha5
 *
 * Created on March 30, 2023, 9:38 AM
 */

#ifndef AlgoHelper_H
#define AlgoHelper_H

#include "m3.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "helper.h"
#include <vector>
#include <iostream>
#include "drawing.h"
#include "loading.h"
struct node_Info {
    //distance to goal
    double h_Value = 0;
    //previous cost
    double g_Value = 0;
    //total value
    //double f_Value = 0;
    //location
    IntersectionIdx idx = -1;
    IntersectionIdx parent = -1;
    StreetSegmentIdx parent_segIdx = -1;
    IntersectionIdx cur_dest = -1;
    //bool ignored = false;

    // bool operator<(const node_Info& b) const {
    //     return this->h_Value + (this->g_Value) >= (b.h_Value)+(b.g_Value);
    // }
    bool operator<(const node_Info& b) const {
        return this->g_Value >= b.g_Value;
    }
};
//calculates the heuristic cost with maximum speed limit
float heuristic_cost(IntersectionIdx current,IntersectionIdx finish );


std::vector<std::pair<std::vector<StreetSegmentIdx>,IntersectionIdx>> findPathBetweenIntersectionsMulti_Dest(
        const std::pair<IntersectionIdx, std::vector<IntersectionIdx>> intersect_ids, const double turn_penalty);

//use Pythagorean theorem to calculate the distance in between the two points 
float distance( IntersectionIdx current,IntersectionIdx finish);

std::vector<int> getStreetSegmentFromIdx(IntersectionIdx current, IntersectionIdx finish);

std::vector<StreetSegmentIdx> bfsTraceBack(int destID);
#endif /* AlgoHelper */

