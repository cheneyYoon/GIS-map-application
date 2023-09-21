

#include <iostream>
#include "helper.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "math.h" //math functions
#include <set>
#include "OSMDatabaseAPI.h"
#include "libcurl.h"
#include "drawing.h"
#include "loading.h"

void loadOsm();
//std::string getOSMNodeTagValue (OSMID OSMid, std::string key);

bool loadMap(std::string map_streets_database_filename) {
     //Indicates whether the map has loaded 
                                  //successfully
    std::cout << "loadMap: " << map_streets_database_filename << std::endl;
    //this string will manipulate the streets database to change it to osm database
    std::string osm_database_filename = map_streets_database_filename;
    //check if the file ends with .bin
    if(map_streets_database_filename.find(".bin", 0 ) < map_streets_database_filename.size()){
       //pop back 11times to delete streets.bin at the end of filename
        for (int i =0; i < 11 ; i++){
        osm_database_filename.pop_back();
        }
        //add osm.bin at the end for osm database
        osm_database_filename.append("osm.bin");
    }
    map_data.load_successful = loadStreetsDatabaseBIN(map_streets_database_filename) && loadOSMDatabaseBIN(osm_database_filename);
    //instantly cut all process when wrong data is fetched to the program
    if(!map_data.load_successful){
        return false;
    }
    //load all maps for better speed of the functions 
    loadOsm();
    computeStreetSegInfo();
    findSegmentsofIntersections();
    findAllPartialNames();
    findAllPartialNamesIntersections();
    preload_streets();
    preload_intersections();
    preload_features();
    preload_POI();
    preload_closure();
    preload_carpark();
    return map_data.load_successful;
}

void closeMap() {   //2
    //clear all the data structures
   map_data.streetMap.clear(); 
   map_data.intersectionMap.clear();
   map_data.intersectionNameMap.clear();
   map_data.nameMap.clear();
   map_data.more_Streets_Info.clear();
   map_data.streets_Info.clear();
   map_data.intersections_Info.clear();
   map_data.features_Info.clear();
   map_data.POI_Info.clear();
   map_data.intersectionStreetSegs.clear();
   map_data.segments.clear();
   parking_Info.clear();
   closure_Info.clear();
   map_data.osmNodes.clear();
   map_data.osmWays.clear();
   map_data.street_Segs_Info.clear();
   map_data.Path.clear();
   
   closeStreetDatabase();
   closeOSMDatabase();
}

//return the value related with given key of the node with given OSMid
std::string getOSMNodeTagValue (OSMID OSMid, std::string key){
    std::string keyfound, value;
    //Access global osm map and get info that u need
    auto e = map_data.osmNodes[OSMid];
   
    //find the value tied with key by iterating with a loop
    for(int i=0; i < getTagCount(e); i++) { 
        std::pair<std::string, std::string> temp;    
        //store key and value in the temp 
        temp = getTagPair(e, i);
            //check if the key given matches the temporary value of key
            if(temp.first == key){
                //return the value assosiated with the key
                return temp.second;
            }  
    }
    //return null if key isn't found
   return NULL;
}
//return the value related with given key of the way with given OSMid
std::string getOSMWayTagValue (OSMID OSMid, std::string key){
    std::string keyfound, value;
    //Access global osm map and get info that u need
    auto e = map_data.osmWays[OSMid];
   
    //find the value tied with key by iterating with a loop
    for(int i=0; i < getTagCount(e); i++) { 
        std::pair<std::string, std::string> temp;    
        //store key and value in the temp 
        temp = getTagPair(e, i);
            //check if the key given matches the temporary value of key
            if(temp.first == key){
                //return the value assosiated with the key
                return temp.second;
            }  
    }
    //return null if key isn't found
   return NULL;
}
void loadOsm(){
    //Iterate
    for(int i=0; i < getNumberOfNodes(); ++i){
        const OSMNode* ptr = getNodeByIndex(i);
        OSMID id = ptr->id();  
        //Store node in map_data.osmNodes, pairing with OSMid
        map_data.osmNodes[id] = ptr; 
    }
    //Iterate
    for(int i=0; i < getNumberOfWays(); ++i){
        const OSMWay* ptr = getWayByIndex(i);
        OSMID id = ptr->id();  
        //Store node in map_data.osmNodes, pairing with OSMid
        map_data.osmWays[id] = ptr; 
    }
}





/* for(int i = 0; i<)





*/