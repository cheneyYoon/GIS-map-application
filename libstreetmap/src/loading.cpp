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


//functions from m2Search (Ctrl+I)

using namespace std;

//iterate through the intersections and define maximum latitude and maximum longitude
//Also precompute coordinates into xy and names
void preload_intersections(){
   map_data.intersections_Info.resize(getNumIntersections());
   #pragma omp parallel for
    for(int num = 0; num < getNumIntersections(); num++){
         map_data.intersections_Info.at(num).name = getIntersectionName(num);
         map_data.intersections_Info[num].index = num;
    }
    map_data.latAvg = get_latAvg();
    #pragma omp parallel for
    for(int id = 0; id< getNumIntersections(); id++){
        LatLon position_LatLon = getIntersectionPosition(id);
        map_data.intersections_Info[id].xy_loc = xy_from_latlon(position_LatLon);
    }
}  


void preload_features(){
    //std::cout << "1";
   map_data.features_Info.resize(getNumFeatures());
   #pragma omp parallel for
    for(int i = 0; i < getNumFeatures(); i++){
        map_data.features_Info[i].name = getFeatureName(i);
        map_data.features_Info[i].type = getFeatureType(i);
        map_data.features_Info[i].numPoints = getNumFeaturePoints(i);
         map_data.features_Info[i].area = findFeatureArea(i);
        for(int j = 0; j < getNumFeaturePoints(i); j++){
            
            ezgl::point2d fpPositions = xy_from_latlon(getFeaturePoint(i,j));
            map_data.features_Info[i].featurePoints.push_back(fpPositions);
        }
         ezgl::point2d startP = map_data.features_Info[i].featurePoints[0];
         ezgl::point2d endP = map_data.features_Info[i].featurePoints[map_data.features_Info[i].featurePoints.size()-1];
         if (startP == endP)
             map_data.features_Info[i].closed = true;
    }
   std::sort(map_data.features_Info.begin(),map_data.features_Info.end(),compareArea);
}  



// This function preloads the data for all points of interest (POIs) in the map.
void preload_POI(){
    // Resize the POI_Info vector to the number of POIs in the map
    map_data.POI_Info.resize(getNumPointsOfInterest());
    // Loop through all POIs and store their name, type, and xy location
    #pragma omp parallel for
    for(int id=0; id<getNumPointsOfInterest(); id++){
        // Store the name of the POI
        map_data.POI_Info[id].name = getPOIName(id);
        // Store the type of the POI
        map_data.POI_Info[id].type = getPOIType(id);
        // Convert the latitude and longitude of the POI to xy coordinates and store them
        map_data.POI_Info[id].xy_loc = xy_from_latlon(getPOIPosition(id));  
    }
} 

//iterate through the streets and load information about street in vector
void preload_streets(){
   map_data.map_max_speed = 0; 
   map_data.street_Segs_Info.resize(getNumStreetSegments());
   #pragma omp parallel for
    for(int id = 0; id < getNumStreetSegments(); id++){
        map_data.street_Segs_Info[id] = getStreetSegmentInfo(id);
        if (map_data.street_Segs_Info[id].speedLimit > map_data.map_max_speed){
            map_data.map_max_speed = map_data.street_Segs_Info[id].speedLimit;
        }
    }
   map_data.latAvg = get_latAvg();
    map_data.intersections_Info.resize(getNumIntersections());
    #pragma omp parallel for
    for(int id = 0; id< getNumIntersections(); id++){
        map_data.intersections_Info[id].xy_loc = xy_from_latlon(getIntersectionPosition(id));
    }
    map_data.streets_Info.resize(getNumStreets());
    #pragma omp parallel for
    for(int id = 0; id < getNumStreets(); id++){
     map_data.streets_Info[id].name = getStreetName(id);
     map_data.streets_Info[id].index = id;
    }
    
}

