#include "m4.h"
#include "helper.h"
#include <vector>
#include "loading.h"
#include "AlgoHelper.h"
#include <cmath>

std::vector<delivery_info> delivery_status;
int order = 0;
bool delivery_finished(const std::vector<DeliveryInf>& deliveries);
bool delivery_finished_2(const std::vector<DeliveryInf>& deliveries);
IntersectionIdx find_nearest_point(const std::vector<DeliveryInf>& deliveries, 
                                    IntersectionIdx current_location);
IntersectionIdx find_nearest_point_2(const std::vector<DeliveryInf>& deliveries, 
                                    IntersectionIdx current_location);
double total_path_travel_time(std::vector<CourierSubPath>& path, const double turn_penalty);
//std::vector<CourierSubPath> two_opt_swap(const std::vector<DeliveryInf>& deliveries, const std::vector<CourierSubPath>& best_path, int i, int j);
std::vector<CourierSubPath> travelingCourier(
                            const std::vector<DeliveryInf>& deliveries,
                            const std::vector<IntersectionIdx>& depots,
                            const float turn_penalty){
    delivery_status.resize(deliveries.size());
    int starting_idx = 0, starting_idx_2 = 0;
    double best_distance = INFINITY;
    double second_best = INFINITY;
    double current_distance;
    IntersectionIdx first_depot = depots[0], first_pickup = deliveries[0].pickUp, 
                    first_depot_2 = 0, first_pickup_2 = 0;
    std::vector<CourierSubPath> best_path, other_path;
    CourierSubPath segment;
    //selecting initial starting point by picking the shortest starting point
    #pragma omp parallel for
    for(int i = 0; i < depots.size(); i ++) {
    for (int j = 0; j < deliveries.size(); j++) {
        current_distance = distance(depots[i], deliveries[j].pickUp);
        if(current_distance < best_distance) {
            // Update primary variables
            second_best = best_distance;
            best_distance = current_distance;
            first_depot_2 = first_depot;  // Update secondary variables
            first_pickup_2 = first_pickup;
            starting_idx_2 = starting_idx;
            first_depot = depots[i];
            first_pickup = deliveries[j].pickUp;
            starting_idx = j;
        }
        else if(current_distance < second_best) {
            // Update secondary variables only
            second_best = current_distance; 
            first_depot_2 = depots[i];
            first_pickup_2 = deliveries[j].pickUp;
            starting_idx_2 = j;
        }
    }
}
    //first package retrieved
    delivery_status[starting_idx].retrieved = true;
    delivery_status[starting_idx_2].retrieved_2 = true;
    //save it into the optimized path
    segment.start_intersection = first_depot;
    segment.end_intersection = first_pickup;
    best_path.push_back(segment);  
    segment.start_intersection = first_depot_2;
    segment.end_intersection = first_pickup_2;
    other_path.push_back(segment);  
    IntersectionIdx current_location, next_location;
    current_location = first_pickup;
    //continue to all delivery points
    while(!delivery_finished(deliveries)){
        next_location = find_nearest_point(deliveries, current_location);
        segment.start_intersection = current_location;
        segment.end_intersection = next_location;
        best_path.push_back(segment);  
        current_location = next_location;
    } 
    //find the nearest depot to end the journey
    best_distance = INFINITY;
    IntersectionIdx best_finish = 0;
    #pragma omp parallel for
    for(int i = 0; i < depots.size(); i ++){
            current_distance = distance(depots[i], current_location);
            if(current_distance < best_distance){
                best_distance = current_distance;
                best_finish = depots[i];
            }      
    }
    segment.start_intersection = current_location;
    segment.end_intersection = best_finish;
    best_path.push_back(segment);
    
    current_location = first_pickup_2;
    while(!delivery_finished_2(deliveries)){
        next_location = find_nearest_point_2(deliveries, current_location);
        segment.start_intersection = current_location;
        segment.end_intersection = next_location;
        other_path.push_back(segment);  
        current_location = next_location;
    }  
    
    best_distance = INFINITY;
    best_finish = 0;
    #pragma omp parallel for
    for(int i = 0; i < depots.size(); i ++){
            current_distance = distance(depots[i], current_location);
            if(current_distance < best_distance){
                best_distance = current_distance;
                best_finish = depots[i];
            }      
    }
    segment.start_intersection = current_location;
    segment.end_intersection = best_finish;
    other_path.push_back(segment);
    
    
    //calculate all path using a-star for the best path
    #pragma omp parallel for
    for(int i = 0; i < best_path.size(); i++){
       
        std::pair<IntersectionIdx, IntersectionIdx> intersection_ids = 
                    std::make_pair(best_path[i].start_intersection, best_path[i].end_intersection);
        best_path[i].subpath = findPathBetweenIntersections(intersection_ids, turn_penalty);
    }
    #pragma omp parallel for
    for(int i = 0; i < other_path.size(); i++){
       
        std::pair<IntersectionIdx, IntersectionIdx> intersection_ids = 
                    std::make_pair(other_path[i].start_intersection, other_path[i].end_intersection);
        other_path[i].subpath = findPathBetweenIntersections(intersection_ids, turn_penalty);
    }
    delivery_status.clear();
    double best_path_time = total_path_travel_time(best_path, turn_penalty);
    double other_path_time = total_path_travel_time(other_path, turn_penalty);
    
    if(other_path_time < best_path_time){
        best_path = other_path;
    }
    
    return best_path;
}

//check if all packages have been delivered
bool delivery_finished(const std::vector<DeliveryInf>& deliveries){
    for(int i = 0; i < deliveries.size(); i++){
        if(!delivery_status[i].delivered){
            return false;
        }
    }
    return true;
}
bool delivery_finished_2(const std::vector<DeliveryInf>& deliveries){
    for(int i = 0; i < deliveries.size(); i++){
        if(!delivery_status[i].delivered_2){
            return false;
        }
    }
    return true;
}

//finds the nearest legal pickup or drop-off point
IntersectionIdx find_nearest_point(const std::vector<DeliveryInf>& deliveries, 
                                    IntersectionIdx current_location){
    int index = 0;
    bool pick_up = true;
    double current_distance;
    double best_distance = INFINITY;
    IntersectionIdx nearest = 0;
    //cant have multithreading in this loop
    //#pragma omp parallel for
    for (int i = 0; i < deliveries.size(); i++){
            
            current_distance = distance(current_location, deliveries[i].pickUp);
            if(current_distance < best_distance && !delivery_status[i].retrieved){
                best_distance = current_distance;
                nearest = deliveries[i].pickUp;
                index = i;
                pick_up = true;
            }
            current_distance = distance(current_location, deliveries[i].dropOff);
            if(current_distance < best_distance && delivery_status[i].retrieved 
               && !delivery_status[i].delivered){
                best_distance = current_distance;
                nearest = deliveries[i].dropOff;
                index = i;
                pick_up = false;
            }
    }
    if(pick_up){
        delivery_status[index].retrieved = true;
    }
    else{
        delivery_status[index].delivered = true;  
    }
    return nearest;
}
//finds the nearest legal pickup or drop-off point
IntersectionIdx find_nearest_point_2(const std::vector<DeliveryInf>& deliveries, 
                                    IntersectionIdx current_location){
    int index = 0;
    bool pick_up = true;
    double current_distance;
    double best_distance = INFINITY;
    IntersectionIdx nearest = 0;
    //cant have multithreading in this loop
    //#pragma omp parallel for
    for (int i = 0; i < deliveries.size(); i++){
            
            current_distance = distance(current_location, deliveries[i].pickUp);
            if(current_distance < best_distance && !delivery_status[i].retrieved_2){
                best_distance = current_distance;
                nearest = deliveries[i].pickUp;
                index = i;
                pick_up = true;
            }
            current_distance = distance(current_location, deliveries[i].dropOff);
            if(current_distance < best_distance && delivery_status[i].retrieved_2 
               && !delivery_status[i].delivered_2){
                best_distance = current_distance;
                nearest = deliveries[i].dropOff;
                index = i;
                pick_up = false;
            }
    }
    if(pick_up){
        delivery_status[index].retrieved_2 = true;
    }
    else{
        delivery_status[index].delivered_2 = true;  
    }
    return nearest;
}

//std::vector<CourierSubPath> two_opt_swap(const std::vector<DeliveryInf>& deliveries, const std::vector<CourierSubPath>& best_path, int i, int j) {
//    std::vector<CourierSubPath> new_path = best_path;
//    
//    
//    // Reverse the order of the nodes between i and j
//    std::reverse(new_path.begin() + i, new_path.begin() + j);
//    
//    //check legality
//    for (int k = 0; k < deliveries.size(); k++){
//        //1=2
//        if( (new_path[i].end_intersection == deliveries[k].dropOff) 
//                && (new_path[i+1].end_intersection == deliveries[k].pickUp) ){
//            std::swap(new_path[i], new_path[i+1]);
//        }
//        //1=3
//        if( (new_path[i].end_intersection == deliveries[k].dropOff) && 
//                new_path[j].end_intersection == deliveries[k].pickUp){
//            return best_path;
//        }
//        //2=3
//        if(new_path[i+1].end_intersection == deliveries[k].dropOff 
//                && new_path[j].end_intersection == deliveries[k].pickUp){
//            std::swap(new_path[i+1], new_path[j]);
//        }
//    }
//    
//    return new_path;
//}
//
double total_path_travel_time(std::vector<CourierSubPath>& path, const double turn_penalty){
    double total_time = 0;
    for(int i = 0; i < path.size(); i++){
        total_time += computePathTravelTime(path[i].subpath,turn_penalty);
    }
    return total_time;
}
