/* 
 * File:   m3.cpp
 * Author: team026
 * Created on March 23, 2023, 10:50 AM
 */

#include "m3.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "helper.h"
#include <vector>
#include <iostream>
#include "drawing.h"
#include "loading.h"
#include <set>
#include "AlgoHelper.h"
#include <bits/stdc++.h>
#include <queue>


//struct node_Info {
//    //distance to goal
//    double h_Value = 0;
//    //previous cost
//    double g_Value = 0;
//    //total value
//    //double f_Value = 0;
//    //location
//    IntersectionIdx idx = -1;
//    IntersectionIdx parent = -1;
//    StreetSegmentIdx parent_segIdx = -1;
//    IntersectionIdx cur_dest = -1;
//    //bool ignored = false;
//
//    // bool operator<(const node_Info& b) const {
//    //     return this->h_Value + (this->g_Value) >= (b.h_Value)+(b.g_Value);
//    // }
//   
//};

std::vector<std::pair<std::vector<StreetSegmentIdx>,IntersectionIdx>> findPathBetweenIntersectionsMulti_Dest(
        const std::pair<IntersectionIdx, std::vector<IntersectionIdx>> intersect_ids, const double turn_penalty);   



double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, const double turn_penalty) {
    double total_travel_time = 0.0;

    //iterate through the path and add up the time to travel each segment
    for (int id = 0; id < path.size(); id++) {
        total_travel_time += findStreetSegmentTravelTime(path[id]);

        //unless the path has reached the end, if the next segment contains different street id,
        //that means there exists a turn, therefore add up the time consumed to turn.
        if ((id + 1) != path.size())
            if (map_data.street_Segs_Info[path[id]].streetID != map_data.street_Segs_Info[path[id + 1]].streetID) {
                total_travel_time += turn_penalty;
            }
    }
    return total_travel_time;
}

std::vector<StreetSegmentIdx> findPathBetweenIntersections(
        const std::pair<IntersectionIdx, IntersectionIdx> intersect_ids, const double turn_penalty) {

    //aStarPth(intersect_ids.first,intersect_idx.second);
    std::vector<StreetSegmentIdx> path;
  
    std::priority_queue<node_Info> openQ;
    
    std::unordered_map<int, node_Info> closelist;
    std::unordered_map<int, node_Info> openMap;
    int startP = intersect_ids.first;
    int endP = intersect_ids.second;
    node_Info startNode = {heuristic_cost(startP, endP), 0, startP, -1, -1};

    openQ.push(startNode);
    openMap.insert(std::make_pair(startP, startNode));
    while (!openQ.empty()) {
        node_Info currentNode;

        currentNode = openQ.top();
        openQ.pop();
        if (closelist.find(currentNode.idx) != closelist.end()) {
            continue;
        }

        closelist[currentNode.idx] = currentNode;
        if (currentNode.idx == endP) {
            while (currentNode.parent_segIdx != -1) {
                path.insert(path.begin(), currentNode.parent_segIdx);
                currentNode = openMap[currentNode.parent];
            }
//            std::cout << "Done Return" << std::endl;
            return path;
        }
        for (auto adj_idx : findAdjacentIntersections(currentNode.idx)) {
            if (closelist.find(adj_idx) != closelist.end()) {
                continue;
            }
            std::vector<int> segment_vec;
            segment_vec = getStreetSegmentFromIdx(currentNode.idx, adj_idx);
            if (segment_vec.size() == 0) {
                continue;
            }
            for (auto segment_idx : segment_vec) {
                double g_cost = currentNode.g_Value + findStreetSegmentTravelTime(segment_idx);
                double h_cost = heuristic_cost(adj_idx, endP);
                if (currentNode.parent != -1 &&
                        (map_data.street_Segs_Info[currentNode.parent_segIdx].streetID !=
                        map_data.street_Segs_Info[segment_idx].streetID)) {
                    g_cost += turn_penalty;
                }
                auto it = openMap.find(adj_idx);
                if (it == openMap.end()) {
                    node_Info adjNode = {h_cost, g_cost, adj_idx, currentNode.idx, segment_idx};
                    openQ.push(adjNode);
                    openMap[adj_idx] = adjNode;
                } else if (g_cost < it->second.g_Value) {
                    it->second.parent = currentNode.idx;
                    it->second.g_Value = g_cost;
                    it->second.parent_segIdx = segment_idx;
                    openQ.push(it->second);
                }
            }
        }
    }
    return {};
}



//multi-destination dijkstra
//returns a vector of a pair of intersections(destination) and a vector path
std::vector<std::pair<std::vector<StreetSegmentIdx>,IntersectionIdx>> findPathBetweenIntersectionsMulti_Dest(
        const std::pair<IntersectionIdx, std::vector<IntersectionIdx>> intersect_ids, const double turn_penalty) {

    std::vector<StreetSegmentIdx> path;
    std::vector<std::pair<std::vector<StreetSegmentIdx>,IntersectionIdx>> multidest_path;
    std::set<IntersectionIdx> destinations;
    std::unordered_set<IntersectionIdx> unvisited_destinations(intersect_ids.second.begin(), intersect_ids.second.end());
    std::priority_queue<node_Info> openQ;
    
    std::unordered_map<int, node_Info> closelist;
    //std::set<int, node_Info> visitedNode;
    std::unordered_map<int, node_Info> openMap;
    int startP = intersect_ids.first;
    

    IntersectionIdx dest = -1;
    //node_Info startNode = {heuristic_cost(startP, endP), 0, startP, -1, -1};
    node_Info startNode = {0, 0, startP, -1, -1,-1};

    openQ.push(startNode);
    openMap.insert(std::make_pair(startP, startNode));
    //for(auto dest : destination){
        int endP = intersect_ids.second[dest];
        while (!openQ.empty()&& !unvisited_destinations.empty()) {
            node_Info currentNode;
            currentNode = openQ.top();
            openQ.pop();
            if (closelist.find(currentNode.idx) != closelist.end()) {
                continue;
            }
            closelist[currentNode.idx] = currentNode;
            //end condition becomes when all destination nodes are visited
            if (unvisited_destinations.find(currentNode.idx) != unvisited_destinations.end()) {
                unvisited_destinations.erase(currentNode.idx);
                path.clear();
                currentNode.cur_dest = currentNode.idx;
                while (currentNode.parent_segIdx != -1) {
                    path.insert(path.begin(), currentNode.parent_segIdx);
                    currentNode = openMap[currentNode.parent];
                }
                //paths function??
            }
            for (auto adj_idx : findAdjacentIntersections(currentNode.idx)) {
                if (closelist.find(adj_idx) != closelist.end()) {
                    continue;
                }
                std::vector<int> segment_vec;
                segment_vec = getStreetSegmentFromIdx(currentNode.idx, adj_idx);
                if (segment_vec.size() == 0) {
                    continue;
                }
                for (auto segment_idx : segment_vec) {
                    double g_cost = currentNode.g_Value + findStreetSegmentTravelTime(segment_idx);
                    //double h_cost = heuristic_cost(adj_idx, endP);
                    if (currentNode.parent != -1 &&
                            (map_data.street_Segs_Info[currentNode.parent_segIdx].streetID !=
                            map_data.street_Segs_Info[segment_idx].streetID)) {
                        g_cost += turn_penalty;
                    }
                    auto it = openMap.find(adj_idx);
                    if (it == openMap.end()) {
                        node_Info adjNode = {0, g_cost, adj_idx, currentNode.idx, segment_idx,currentNode.cur_dest};
                        openQ.push(adjNode);
                        openMap[adj_idx] = adjNode;
                    } else if (g_cost < it->second.g_Value) {
                        it->second.parent = currentNode.idx;
                        it->second.g_Value = g_cost;
                        it->second.parent_segIdx = segment_idx;
                        it->second.cur_dest = currentNode.cur_dest;
                        openQ.push(it->second);
                    }
                }
            }
        }

    //}
     return multidest_path;
}



