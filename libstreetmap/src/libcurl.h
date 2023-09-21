/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   libcurl.h
 * Author: fanshuh2
 *
 * Created on March 11, 2023, 5:44 PM
 */

#ifndef LIBCURL_H
#define LIBCURL_H
#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <ctime>
#include <chrono>
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;

//using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::read_json;



struct park_data{
    ezgl::point2d xy_loc;
};


struct roadClosure_data{
    std::string id;
    std::string name;
    ezgl::point2d xy_loc;
    std::string description;
};

struct weather_data{
    float temp_c ;
    std::string description;
    float wind_mph;
    float humidity;
    float feels_like_c;
    float feels_like_f;
    float id;
};


typedef struct MyCustomStruct {
    char *url = nullptr;
    unsigned int size = 0;
    char *response = nullptr;
} MyCustomStruct;

extern std::vector<roadClosure_data> closure_Info;
extern std::vector<park_data> parking_Info;
//static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);




void weatherInfo(ptree &ptRoot);
void RoadClosureInfo(ptree &ptRoot);
void preload_closure();
void weather_callback(GtkWidget* /*widget*/, ezgl::application *app);
void parkingInfo(ptree &ptRoot);
void preload_carpark();

#endif /* LIBCURL_H */

