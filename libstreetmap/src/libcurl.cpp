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
#include <iostream>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS  
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <ctime>
#include <curl/curl.h>
#include "libcurl.h"
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;

//using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::read_json;
std::vector<roadClosure_data> closure_Info;
std::vector<park_data> parking_Info;
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
static size_t write_data(void *buffer, size_t /*size*/, size_t nmemb, void *userp) {
    if (buffer && nmemb && userp) {
        MyCustomStruct *pMyStruct = (MyCustomStruct *)userp;
        // Writes to struct passed in from main
        if (pMyStruct->response == nullptr) {
            // Case when first time write_data() is invoked
            pMyStruct->response = new char[nmemb + 1];
            strncpy(pMyStruct->response, (char *)buffer, nmemb);
        }
        else {
            // Case when second or subsequent time write_data() is invoked
            char *oldResp = pMyStruct->response;

            pMyStruct->response = new char[pMyStruct->size + nmemb + 1];

            // Copy old data
            strncpy(pMyStruct->response, oldResp, pMyStruct->size);

            // Append new data
            strncpy(pMyStruct->response + pMyStruct->size, (char *)buffer, nmemb);

            delete []oldResp;
        }
        pMyStruct->size += nmemb;
        pMyStruct->response[pMyStruct->size] = '\0';
    }
    return nmemb;
}


void RoadClosureInfo(ptree &ptRoot) {
    std::string roadName;
    std::string id;
    std::string closure_descrip;
    double longitude = 0, latitude = 0;

    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("Closure")) {
    // "features" maps to a JSON array, so each child should have no name
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";
        longitude = featVal.second.get<double>("longitude");
        latitude = featVal.second.get<double>("latitude");
        roadName = featVal.second.get<std::string>("name");
        id = featVal.second.get<std::string>("id");
        closure_descrip = featVal.second.get<std::string>("description");
        roadClosure_data info;
        info.name = roadName;
        LatLon position(latitude,longitude);
        info.xy_loc =  xy_from_latlon(position);
        info.description = closure_descrip;
        info.id = id;
        // Print road closure info
        closure_Info.push_back(info);
        //cout << "Affected Road: " << roadName << endl;
        //cout << "\t Description: " << closureName << endl;
        //cout << "\t Time: " << std::put_time(std::localtime(&startTime), "%F %T") << " - " << std::put_time(std::localtime(&endTime), "%F %T") << endl;
        //cout << "\t Coordinates: " << longitude << ", " << latitude << endl;
    }
    return;
}

void preload_closure(){
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        std::cout << "ERROR: Unable to initialize libcurl" << std::endl;
        std::cout << curl_easy_strerror(res) << std::endl;
        return;
    }

    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        std::cout << "ERROR: Unable to get easy handle" << std::endl;
        return;
    } else {
        char errbuf[CURL_ERROR_SIZE] = {0};
        MyCustomStruct myStruct;
        char targetURL[] = "https://secure.toronto.ca/opendata/cart/road_restrictions/v3?format=json";

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);
        myStruct.url = targetURL;
        if (res != CURLE_OK) {
            std::cout << "ERROR: Unable to set libcurl option" << std::endl;
            std::cout << curl_easy_strerror(res) << std::endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }
        //cout << endl << endl;
        if (res == CURLE_OK) {
            // Create an empty proper tree
            ptree ptRoot;
            /* Store JSON data into a Property Tree
             *
             * read_json() expects the first parameter to be an istream object,
             * or derived from istream (e.g. ifstream, istringstream, etc.).
             * The second parameter is an empty property tree.
             *
             * If your JSON data is in C-string or C++ string object, you can
             * pass it to the constructor of an istringstream object.
             */
            std::istringstream issJsonData(myStruct.response);
            read_json(issJsonData, ptRoot);
            // Parsing and printing the data
//            cout << "Current road closures are as follows:" << endl;
//            cout << "====================" << endl << endl;
            try {
                RoadClosureInfo(ptRoot);
            } catch (const char *errMsg) {
                std::cout << "ERROR: Unable to fully parse the road closures JSON data" << std::endl;
                std::cout << "Thrown message: " << errMsg << std::endl;
            }
        } 
        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
    curl_global_cleanup();
    //return;
}


void weather_callback(GtkWidget* /*widget*/, ezgl::application *app){
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        std::cout << "ERROR: Unable to initialize libcurl" << std::endl;
        std::cout << curl_easy_strerror(res) << std::endl;
        return;
    }

    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        std::cout << "ERROR: Unable to get easy handle" << std::endl;
        return;
    } else {
        char errbuf[CURL_ERROR_SIZE] = {0};
        MyCustomStruct myStruct;
       
        std::string URL = "https://api.weatherapi.com/v1/current.json?key=41b84a072593436f8cc212208231103&q=" + map_data.city_name;
        int size = URL.size()+1;
        char *targetURL = new char[size];
        //char targetURL[size];
        //char targetURL[URL.size()+1];
        //use cstr to convert a string to constant character array
        std::strcpy(targetURL,URL.c_str());
        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);
        myStruct.url = targetURL;
        delete[] targetURL;
        if (res != CURLE_OK) {
            std::cout << "ERROR: Unable to set libcurl option" << std::endl;
            std::cout << curl_easy_strerror(res) << std::endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }
        if (res == CURLE_OK) {
            // Create an empty proper tree
            ptree ptRoot;
            std::istringstream issJsonData(myStruct.response);
            read_json(issJsonData, ptRoot);
            std::stringstream ss;
            std::string local_time = ptRoot.get<std::string>("location.localtime");
            std::string condition = ptRoot.get<std::string>("current.condition.text");
            std::string name = ptRoot.get<std::string>("location.name");
            std::string region = ptRoot.get<std::string>("location.region");
            std::string country = ptRoot.get<std::string>("location.country");
            double feelslike_c = ptRoot.get<double>("current.feelslike_c");
            double temp = ptRoot.get<double>("current.temp_c");
            ss << "In "+name+",  "+region+",  "+country <<std::endl;
            ss << "Local Time: " +local_time <<std::endl;
            ss << "The temperature is "<<temp <<"\u2103"<<std::endl;
            ss << "Feels like: "<<feelslike_c<< "\u2103"<<std::endl;
            ss << "Weather Condition: " << condition <<std::endl;
            
            app->create_popup_message("Weather update",ss.str().c_str());
        } 
        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
    curl_global_cleanup();
    
    return;
}




void preload_carpark(){
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        std::cout << "ERROR: Unable to initialize libcurl" << std::endl;
        std::cout << curl_easy_strerror(res) << std::endl;
        return;
    }

    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        std::cout << "ERROR: Unable to get easy handle" << std::endl;
        return;
    } else {
        char errbuf[CURL_ERROR_SIZE] = {0};
        MyCustomStruct myStruct;
        char targetURL[] = "https://ckan0.cf.opendata.inter.prod-toronto.ca/dataset/b66466c3-69c8-4825-9c8b-04b270069193/resource/8549d588-30b0-482e-b872-b21beefdda22/download/green-p-parking-2019.json";

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);
        myStruct.url = targetURL;
        if (res != CURLE_OK) {
            std::cout << "ERROR: Unable to set libcurl option" << std::endl;
            std::cout << curl_easy_strerror(res) << std::endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }
        //cout << endl << endl;
        if (res == CURLE_OK) {
            // Create an empty proper tree
            ptree ptRoot;
            /* Store JSON data into a Property Tree
             *
             * read_json() expects the first parameter to be an istream object,
             * or derived from istream (e.g. ifstream, istringstream, etc.).
             * The second parameter is an empty property tree.
             *
             * If your JSON data is in C-string or C++ string object, you can
             * pass it to the constructor of an istringstream object.
             */
            std::istringstream issJsonData(myStruct.response);
            read_json(issJsonData, ptRoot);
            // Parsing and printing the data
//            cout << "Current road closures are as follows:" << endl;
//            cout << "====================" << endl << endl;
            try {
                parkingInfo(ptRoot);
            } catch (const char *errMsg) {
                std::cout << "ERROR: Unable to fully parse the road closures JSON data" << std::endl;
                std::cout << "Thrown message: " << errMsg << std::endl;
            }
        } 
        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
    curl_global_cleanup();
    //return;
}

void parkingInfo(ptree &ptRoot) {
    ezgl::point2d xy_loc;
    std::string id;
    std::string closure_descrip;
    //double longitude = 0, latitude = 0;

    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("carparks")) {
    // "features" maps to a JSON array, so each child should have no name
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";
        float lat = featVal.second.get<float>("lat");
        float longi = featVal.second.get<float>("lng");
       
        park_data info;

        LatLon position(lat,longi);
        info.xy_loc =  xy_from_latlon(position);
        
        parking_Info.push_back(info);
        
    }
    return;
}