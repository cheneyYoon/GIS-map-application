/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */
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
global_var map_data;

//functions from m2

using namespace std;


float x_from_latlon(float lon){
    //map_data.latAvg = (map_data.+map_data.min_lat)/2;
    return lon*(kDegreeToRadian*kEarthRadiusInMeters*cos(map_data.latAvg));
}
float y_from_latlon(float lat){
    return lat*(kDegreeToRadian*kEarthRadiusInMeters);
}
float latlon_from_x(float x){
    return x/(kDegreeToRadian*kEarthRadiusInMeters*cos(map_data.latAvg));
}
float latlon_from_y(float y){
    return y/(kDegreeToRadian*kEarthRadiusInMeters);
}
float get_latAvg (){
    map_data.max_lat = getIntersectionPosition(0).latitude();
    map_data.min_lat = map_data.max_lat;
    map_data.max_lon = getIntersectionPosition(0).longitude();
    map_data.min_lon = map_data.max_lon;
        for(int num = 0; num < getNumIntersections(); num++){
            LatLon position_LatLon = getIntersectionPosition(num);
            map_data.max_lat = std::max(map_data.max_lat, position_LatLon.latitude());
            map_data.min_lat = std::min(map_data.min_lat, position_LatLon.latitude());
            map_data.max_lon = std::max(map_data.max_lon, position_LatLon.longitude());
            map_data.min_lon = std::min(map_data.min_lon, position_LatLon.longitude());
        
    }
   
    return (map_data.max_lat+map_data.min_lat)/2 * kDegreeToRadian;
}


//determines street type based on the speed limit
std::string type_of_street(OSMID OSMid){
    std::string street_type;
    street_type = getOSMWayTagValue(OSMid, "highway");
    //determine major and minor roads based on the OSM data
        //highway
        if (street_type == "motorway" || street_type == "motorway_link" ||
                street_type == "trunk" || street_type == "trunk_link" ){   
            return "highway";
        }
        //major roads
        else if( street_type == "primary" || street_type == "primary_link" ||
                street_type == "secondary" || street_type == "secondary_link"){
                return "major";
        }
        //minor roads
    
        else {
               return "minor";
        }
}

//add direction to a one way street
std::string one_way_street_name (StreetSegmentIdx id, std::string one_way_street){
    ezgl::point2d start = map_data.intersections_Info[map_data.street_Segs_Info[id].from].xy_loc;
    ezgl::point2d finish = map_data.intersections_Info[map_data.street_Segs_Info[id].to].xy_loc;
    double dx = finish.x - start.x;
    double dy = finish.y - start.y;
    std::string direction;

    // Add the appropriate arrow based on the direction of the street
    if(dx <= 0 && dy > 0){
        direction = "  <<";
    }
    else if(dx >= 0 && dy < 0){
        direction = "  >>";
    }
    else if(dx <= 0 && dy <= 0){
        direction = "  <<";
    }
    else{
        direction = "  >>";
    }
    return one_way_street + direction;
}


//calculate the angle of the street
double angle_of_street (StreetSegmentIdx id){
    ezgl::point2d start = map_data.intersections_Info[map_data.street_Segs_Info[id].from].xy_loc;
    ezgl::point2d finish = map_data.intersections_Info[map_data.street_Segs_Info[id].to].xy_loc;
    double dx = finish.x - start.x;
    double dy = finish.y - start.y;
    double abs_dx = std::abs(dx);
    double abs_dy = std::abs(dy);
    double angle;
    //for vertical line
    if (dx == 0){
        if(dy >= 0) {
           return 90; 
        }
        else{
            return -90;
        }
    }
        //angle formula
        angle = std::abs(std::atan(abs_dy/abs_dx)*180.0/M_PI); 
    //adjust angle
    if (( dx <=0 && dy > 0) || ( dx >= 0 && dy <0)){
        angle = -angle;
    }   
    return angle;
}

std:: string findDirection(StreetSegmentIdx id1, StreetSegmentIdx id2){
    double distance1 = findDistanceBetweenTwoPoints(getIntersectionPosition(map_data.street_Segs_Info[id1].from), getIntersectionPosition(map_data.street_Segs_Info[id2].to));
    double distance2 = findDistanceBetweenTwoPoints(getIntersectionPosition(map_data.street_Segs_Info[id1].to), getIntersectionPosition(map_data.street_Segs_Info[id2].to));
    int from = map_data.street_Segs_Info[id1].from;
    int to = map_data.street_Segs_Info[id1].to;
    if( distance1 < distance2){
        map_data.street_Segs_Info[id1].from = to;
        map_data.street_Segs_Info[id1].to = from;
    }
    std::string str;
    int angle = angle_of_street(id1);
    if((angle > -23 && angle < 0) || (angle >=0 && angle < 22) ){
        str = "East";
    }
    if(angle >21 && angle < 66){
        str = "Northeast";
    }
    if(angle >65 && angle < 110){
        str = "North";
    }
    if(angle >109 && angle < 155){
        str = "Northwest";
    }
    if((angle >154 && angle < 180) || (angle >= -180 && angle < -155) ) {
        str = "West";
    }
    if(angle  > -154 && angle < -110){
        str = "SouthWest";
    }
    if(angle  > -111 && angle < -65 ){
        str = "South";
    }
    if(angle > -66 && angle < -23 ){
        str = "SouthWest";
    }
        map_data.street_Segs_Info[id1].from = from;
        map_data.street_Segs_Info[id1].to = to;   
        return str; 
}

//Finds left or right based on angle
std::string findRightOrLeft (StreetSegmentIdx from, StreetSegmentIdx to){
 if (angle_of_street(to) > angle_of_street(from)){
    return "right";
 }
    else{
        return "left";
    }
}

//calculate the angle for a curve
double angle_of_curved_street(StreetSegmentIdx id){
    int num_curve_points = map_data.street_Segs_Info[id].numCurvePoints;
    int i, j;
    //use the normal angle calculation if there is one curve point
    if (num_curve_points == 1){
        return angle_of_street(id);
    }
    //find angle of the line tangent to the middle point for odd number of curves
    if (num_curve_points % 2 == 1){
        i = num_curve_points/2-1;
        j = i+2;
    }
    //find angle of the line tangent to the middle point for even number of curves
    else{
        i = num_curve_points/2-1;
        j = i+1;
    }
    //2d location of starting point
    const auto& start_latlon = getStreetSegmentCurvePoint(id, i);
    const auto& start = ezgl::point2d(x_from_latlon(start_latlon.longitude())
                                        , y_from_latlon(start_latlon.latitude()));
    //2d location of starting point
    const auto& finish_latlon = getStreetSegmentCurvePoint(id, j);
    const auto& finish = ezgl::point2d(x_from_latlon(finish_latlon.longitude())
                                        , y_from_latlon(finish_latlon.latitude()));
    double dx = finish.x - start.x;
    double dy = finish.y - start.y;
    double abs_dx = std::abs(dx);
    double abs_dy = std::abs(dy);
    double angle;
    //for vertical line
    if (dx == 0){
        if(dy >= 0) {
           return 90; 
        }
        else{
            return -90;
        }
    }
        //angle formula
        angle = std::abs(std::atan(abs_dy/abs_dx)*180.0/M_PI);   
    //adjust angle
    if (( dx <=0 && dy > 0) || ( dx >= 0 && dy <0)){
        angle = -angle;
    }
        return angle;
}
//calculate the centerpoint for the street
ezgl::point2d center_of_street(StreetSegmentIdx id){
    ezgl::point2d start = map_data.intersections_Info[map_data.street_Segs_Info[id].from].xy_loc;
    ezgl::point2d finish = map_data.intersections_Info[map_data.street_Segs_Info[id].to].xy_loc;
    double xCenter = (finish.x + start.x)/2;
    double yCenter = (finish.y + start.y)/2;
    return ezgl::point2d(xCenter, yCenter);
}
//calculate the centerpoint for the curve
ezgl::point2d center_of_curved_street(StreetSegmentIdx id){
    int num_curve_points = map_data.street_Segs_Info[id].numCurvePoints;
    int i, j;
    //calculate the centerpoint like a non-curved street if there is only one curve point
    if (num_curve_points == 1){
        return center_of_street(id);
    }
    //for even number of curve points, find angle 
    //approximately tangent to middle point for odd number of curves
    if (num_curve_points % 2 == 1){
        i = num_curve_points/2-1;
        j = i+2;
    }
    //for even number of curve points, find angle
    //approximately tangent to middle point for odd number of curves
    else{
        i = num_curve_points/2-1;
        j = i+1;
    }
    //2d location of starting point
    const auto& start_latlon = getStreetSegmentCurvePoint(id, i);
    const auto& start = ezgl::point2d(x_from_latlon(start_latlon.longitude())
                                        , y_from_latlon(start_latlon.latitude()));
    //2d location of finishing point
    const auto& finish_latlon = getStreetSegmentCurvePoint(id, j);
    const auto& finish = ezgl::point2d(x_from_latlon(finish_latlon.longitude())
                                        , y_from_latlon(finish_latlon.latitude()));
    double xCenter = (finish.x + start.x)/2;
    double yCenter = (finish.y + start.y)/2;
    //find the center of the two points, start and finish
    return ezgl::point2d(xCenter, yCenter);
}

//calculate the length of the street segment
double x_boundary (StreetSegmentIdx id){
    ezgl::point2d start = map_data.intersections_Info[map_data.street_Segs_Info[id].from].xy_loc;
    ezgl::point2d finish = map_data.intersections_Info[map_data.street_Segs_Info[id].to].xy_loc;
    double dx = finish.x - start.x;
    double dy = finish.y - start.y;
    //length formula
    return sqrt(pow(dx,2.0)+pow(dy,2.0));
}



bool compareArea(const Feature_data &a, const Feature_data &b){
    return b.area < a.area; 
}

/*void fit_zoom(int a, int b){
    ezgl::point2d from = xy_from_latlon(getIntersectionPosition(a));
    ezgl::point2d to = xy_from_latlon(getIntersectionPosition(b));
    
}*/

//return a point2d from given latlon
ezgl::point2d xy_from_latlon(LatLon loc){
    //make a point2d in x,y from lat and long conversion
    float x_loc = loc.longitude()*kDegreeToRadian*kEarthRadiusInMeters*(cos(map_data.latAvg));
    float y_loc = loc.latitude()*kDegreeToRadian*kEarthRadiusInMeters;
    return ezgl::point2d(x_loc, y_loc);
}


//m1 helper



void findSegmentsofIntersections(){
    //Populates a vector of all the intersections on the map with a vector of all it's associated street segments
    for(int i = 0; i <getNumIntersections(); i++){
            std::vector<StreetSegmentIdx> ss_ids;
        for(int j = 0; j < getNumIntersectionStreetSegment(i); ++j) {
            int ss_id = getIntersectionStreetSegment(i, j);
            ss_ids.push_back(ss_id);
        }
        map_data.intersectionStreetSegs.push_back(ss_ids);
    }
}


// This function finds all partial names of streets in the map and stores them in a hash table
void findAllPartialNames(){
    // Loop through all street segments in the map
    for(int i = 0; i < getNumStreetSegments(); i++){
        // Get the street ID of the current street segment
        StreetIdx s = getStreetSegmentInfo(i).streetID;
        // If the street is not already in the streetMap hash table, add it
        if(!map_data.streetMap.count(s)){
            map_data.streetMap.insert({s,std::vector<StreetIdx>()});
            // Get the name of the street and remove whitespace
            std::string name = getStreetName(s);
            name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
            // Convert the street name to lowercase
            std::transform(name.begin(), name.end(), name.begin(),[](unsigned char c){ return std::tolower(c); });
            // Loop through all possible prefixes of the street name and store them in the nameMap hash table
            for(int m = 0; m < name.length()+1; m ++){
                std::string p = name.substr(0,m);
                if(!map_data.nameMap.count(p)){
                    map_data.nameMap.insert({p, std::vector<StreetIdx>()});
                }
                map_data.nameMap.at(p).push_back(s);
            }
        }
        // Add the street segment to the streetMap hash table
        map_data.streetMap.at(s).push_back(i);
    }
} 

void findAllPartialNamesIntersections(){
    // Loop through all street segments in the map
    for(int i = 0; i < getNumIntersections(); i++){
        // If the street is not already in the streetMap hash table, add it
            map_data.streetMap.insert({i,std::vector<IntersectionIdx>()});
            // Get the name of the street and remove whitespace
            std::string name = getIntersectionName(i);
            name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
            // Convert the street name to lowercase
            std::transform(name.begin(), name.end(), name.begin(),[](unsigned char c){ return std::tolower(c); });
            // Loop through all possible prefixes of the street name and store them in the nameMap hash table
            for(int m = 0; m < name.length()+1; m ++){
                std::string p = name.substr(0,m);
                if(!map_data.intersectionNameMap.count(p)){
                    map_data.intersectionNameMap.insert({p, std::vector<StreetIdx>()});
                }
                map_data.intersectionNameMap.at(p).push_back(i);
            }
    }
}  



// This function computes and stores travel time for each street segment in the map
void computeStreetSegInfo(){
    // Loop through all street segments in the map
    for(int k = 0; k< getNumStreetSegments(); k++){
        // Get the length and speed limit of the current street segment
        StreetSegmentInfo segmentInfo = getStreetSegmentInfo(k);
        double time = findStreetSegmentLength(k)/segmentInfo.speedLimit;
        // Store the travel time for the street segment in a SegmentInfo struct and add it to the segments vector in the map_data struct
        SegmentInfo seg;
        seg.travel_time = time;
        map_data.segments.push_back(seg);
    }
} 


//map for map_data.osmNodes



double findDistanceBetweenTwoPoints(LatLon point_1, LatLon point_2){
    // Convert latitude and longitude values to radians
    double latAvg = (kDegreeToRadian * point_1.latitude() + kDegreeToRadian * point_2.latitude()) / 2;
    // Calculate the x and y coordinates of the two points
    double x1 = point_1.longitude() * kDegreeToRadian * kEarthRadiusInMeters * (cos(latAvg));
    double x2 = point_2.longitude() * kDegreeToRadian * kEarthRadiusInMeters * (cos(latAvg));
    double y1 = point_1.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
    double y2 = point_2.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
    // Calculate the distance between the two points using the Pythagorean theorem
    double distanceBetweenTwoPoints = sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
    // Return the distance between the two points
    return distanceBetweenTwoPoints;
}




//outputs the length of the given street segment

double findStreetSegmentLength(StreetSegmentIdx street_segment_id){ //4
//get Street Segments & coordinates of the starting and ending points
    StreetSegmentInfo segmentInfo = getStreetSegmentInfo(street_segment_id);
    LatLon startPos = getIntersectionPosition(segmentInfo.from);
    LatLon endPos = getIntersectionPosition(segmentInfo.to);
    int nCurvePoints = segmentInfo.numCurvePoints;
    double totalDistance = 0;
    double segmentDistance = 0;
    LatLon SegStartPoint = startPos;
    //when theres no curve points, just do distance between 2 points
    if(nCurvePoints == 0){
        return findDistanceBetweenTwoPoints(startPos, endPos);
    }
    //when theres one or more curve points, do a loop and add each of the segments up to get the total distance
    //this might exceeds the speed limit
    else{
        for(int i = 0; i< nCurvePoints; i++){
            LatLon SegEndPoint = getStreetSegmentCurvePoint(street_segment_id, i);
            segmentDistance = findDistanceBetweenTwoPoints(SegStartPoint, SegEndPoint);
            totalDistance += segmentDistance;
            SegStartPoint = SegEndPoint;
        }
    }

    //added distance between last segment point to end point
    totalDistance += findDistanceBetweenTwoPoints(SegStartPoint, endPos);
    return totalDistance;
}


//high speed requirement
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id){//5
     return map_data.segments[street_segment_id].travel_time;
}



std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id){
    std::vector<StreetSegmentIdx> street_s = findStreetSegmentsOfIntersection(intersection_id);
    std::set<IntersectionIdx> answer;
        for(int i = 0; i < street_s.size(); i++){
            StreetSegmentInfo segment = getStreetSegmentInfo(street_s[i]);
            if(segment.from == intersection_id){
            answer.insert(segment.to);
            } else if (!segment.oneWay){
            answer.insert(segment.from);
            }
        }
    return std::vector<IntersectionIdx> (answer.begin(), answer.end());
}



IntersectionIdx findClosestIntersection(LatLon my_position){
    IntersectionIdx closestintloc = 0;
    double minD, curD;
    bool flagI = true; //checks for first time iterating in the loop to initialize values
    int TotalNumOfIntersections = getNumIntersections();
    LatLon IntersectionPosition;
    //loop for all intersection ids
    for (IntersectionIdx i = 1; i < TotalNumOfIntersections; i++){
            //for initializing the minimum distance
            if(flagI == true) {
                flagI = false;
                IntersectionPosition = getIntersectionPosition(i);
                minD = findDistanceBetweenTwoPoints(my_position, IntersectionPosition); 
            }
            //update the closest intersection if current distance is less than the minimum
            IntersectionPosition = getIntersectionPosition(i);
            curD = findDistanceBetweenTwoPoints(my_position, IntersectionPosition);
            if (curD < minD){ 
                minD = curD;
                closestintloc = i;
            }
        }
        return closestintloc;
    }

    //implemented by Sean Narandeniya
    std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id){
    return map_data.intersectionStreetSegs[intersection_id];
}
    
    //implemented by Sean Narandeniya
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    if(map_data.intersectionMap.count(street_id)){
        return map_data.intersectionMap.at(street_id);
    }
    std::vector<StreetSegmentIdx> segments1 = map_data.streetMap.at(street_id);
    std::set<IntersectionIdx> answer;
        for(int i = 0; i < segments1.size(); i++){
            StreetSegmentInfo segment = getStreetSegmentInfo(segments1[i]);
            answer.insert(segment.to);
            answer.insert(segment.from);
        }
    std::vector<IntersectionIdx> map3(answer.begin(), answer.end());
    map_data.intersectionMap.insert({street_id, map3});
    return map3;
}

std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(StreetIdx street_id1, StreetIdx street_id2){
    std::vector<IntersectionIdx> street1 = findIntersectionsOfStreet(street_id1);
    std::vector<IntersectionIdx> street2 = findIntersectionsOfStreet(street_id2);
    std::set<IntersectionIdx> s(street1.begin(), street1.end());
    std::vector<IntersectionIdx> answer;
    for(int i = 0; i < street2.size(); i++){
        IntersectionIdx s1 = street2.at(i);
        if(s.count(s1)){
            answer.push_back(s1);
        }
    }
    return answer;
}


std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){
    street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), ::isspace), street_prefix.end());
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(),[](unsigned char c){ return std::tolower(c); });
    if(!map_data.nameMap.count(street_prefix)) 
        return std::vector<StreetIdx>();
    return map_data.nameMap.at(street_prefix);
}

std::vector<StreetIdx> findIntersectionIdsFromPartialStreetName(std::string street_prefix){
    street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), ::isspace), street_prefix.end());
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(),[](unsigned char c){ return std::tolower(c); });
    if(!map_data.intersectionNameMap.count(street_prefix)) 
        return std::vector<StreetIdx>();
    return map_data.intersectionNameMap.at(street_prefix);
}

//implemented by Shuhao Fan

//do trapizoid for each point on the right and then subtract the area of the trapizoid on the left
// not sure how to get the points 
//high speed requirements

double findFeatureArea(FeatureIdx feature_id){
    int nFeaturePoints = getNumFeaturePoints(feature_id);
    //starts at point 0 not 1 lmao -30min
    LatLon startP = getFeaturePoint(feature_id,0);
    LatLon endP = getFeaturePoint(feature_id, nFeaturePoints-1);
    double area = 0;
    //if tartP = endP, than the feature is a 0 area polygon or a polyline?
    //has to compare lat and lon instead of just say startP == endP
    std::vector<double> vect(4);
    double y1,y2 = 0;
    if((startP.longitude() != endP.longitude()) && (startP.latitude() != endP.longitude())){
        return 0;
    }
    else{ 
        for (int i = 0; i< nFeaturePoints-1; i++){
        LatLon nextP = getFeaturePoint(feature_id,i+1);//i-1
            vect = latlonToPoint(startP,nextP);
            y1 = vect[1];
            y2 = vect[3];
        if(y1 >= y2){
            area += findTrapArea(startP, nextP);
        }
            else
            {
                area -= findTrapArea(nextP, startP);
            }
        startP = nextP;
        }
    }
    return abs(area);
}


// find Street length by calling get streetsegmentlength
//double findStreetLength(StreetIdx street_id){
//    double StreetLength = 0;
//    for(int i = 0; i < map_data.streetMap[street_id].size(); i++){
//    StreetLength += findStreetSegmentLength(map_data.streetMap[street_id][i]);
//    }
//    return StreetLength;
//}




//finds the nearest point of interest to the given position
POIIdx findClosestPOI(LatLon my_position, std::string POItype){
    POIIdx closestPOIlocation = 0;
    int TotalNumOfPOI = getNumPointsOfInterest();
    double minD, curD;
    bool flagD = true; //checks if its entering the loop for the first time to initialize values
    LatLon POIposition;
    //Loop iterates through all POIs 
    for (POIIdx poiIdx = 1; poiIdx <= TotalNumOfPOI-1; poiIdx++){
        //for the first POI in the list that matches the type
        if (getPOIType(poiIdx) == POItype && flagD == true){
            flagD = false;
            POIposition = getPOIPosition(poiIdx);
            minD = findDistanceBetweenTwoPoints(my_position, POIposition);  //store the distance between
            closestPOIlocation = poiIdx; //store the first POI of match as closestPOI
            }
        //if not first, set current POI location as the new location to compare
        if (getPOIType(poiIdx) == POItype && flagD == false){
            POIposition = getPOIPosition(poiIdx); 
            curD = findDistanceBetweenTwoPoints(my_position, POIposition);
            //update closestPOI if the current POI is closer
            if (curD < minD){
                minD = curD;
                closestPOIlocation = poiIdx;
            }
        }
    }
    return closestPOIlocation;
}


double findTrapArea(LatLon pointA, LatLon pointB){
    //app->get_object("ID you put in glade");
    std::vector<double> positionOf2Points(4);
    positionOf2Points = latlonToPoint(pointA, pointB);
    double x1,y1,x2,y2 = 0;
    x1 = positionOf2Points[0];
    y1 = positionOf2Points[1];
    x2 = positionOf2Points[2];
    y2 = positionOf2Points[3];
    return ((abs(x1)+abs(x2))*(abs(y1)-abs(y2)))/2;
}


std::vector<double> latlonToPoint(LatLon pointA, LatLon pointB){
    std::vector<double> vect(4);
    double x1,x2,y1,y2;
    double latAvg = (kDegreeToRadian*pointA.latitude()+kDegreeToRadian*pointB.latitude())/2;
    x1 = pointA.longitude()*kDegreeToRadian*kEarthRadiusInMeters*(cos(latAvg));
    x2 = pointB.longitude()*kDegreeToRadian*kEarthRadiusInMeters*(cos(latAvg));
    y1 = pointA.latitude()*kDegreeToRadian*kEarthRadiusInMeters;
    y2 = pointB.latitude()*kDegreeToRadian*kEarthRadiusInMeters;
    vect[0] = x1;
    vect[1] = y1;
    vect[2] = x2;
    vect[3] = y2;
    return vect;
}

bool withinBounds(ezgl::rectangle rec, ezgl::point2d xy_loc){
    // Extract the x and y coordinates from the given point
    double x = xy_loc.x;
    double y = xy_loc.y;
    // Extract the x and y coordinates of the corners of the given rectangle
    double x_1 = rec.m_first.x;
    double y_1 = rec.m_first.y;
    double x_2 = rec.m_second.x;
    double y_2 = rec.m_second.y;
    // Check if the given point is within the given rectangle
    if(x < x_1 || y < y_1 || x > x_2 || y > y_2){
        // If the point is outside the rectangle, return false
        return false;
    }
    // If the point is inside the rectangle, return true
    return true;
}

//calculate lonAvg
float get_lonAvg(){
    return (map_data.max_lon+map_data.min_lon)/2 * kDegreeToRadian;
}
    

int line_width(){
    map_data.latAvg = get_latAvg();
      
    if (map_data.zoomfactor < map_data.zoom_level_10) {
        return 14;
    } else if (map_data.zoomfactor < map_data.zoom_level_8) {
        return 12;
    } else if (map_data.zoomfactor < map_data.zoom_level_6) {
        return 8;
    }        
    else if (map_data.zoomfactor < map_data.zoom_level_4) {
        return 4;
    } else if (map_data.zoomfactor < map_data.zoom_level_3) {
       return 3;
    } else if (map_data.zoomfactor < map_data.zoom_level_1) {
       return 2;
    }     
    else {
       return 1;
    }
}