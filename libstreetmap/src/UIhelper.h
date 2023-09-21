/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   UIhelper.h
 * Author: yooncha5
 *
 * Created on April 1, 2023, 12:48 PM
 */

#ifndef UIHELPER_H
#define UIHELPER_H
#include "LatLon.h"
#include "m2.h"
#include "m1.h"
#include "loading.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <sstream>
#include <stdbool.h>
#include <set>
#include <algorithm>

void clear_callback(GtkWidget* /*widget*/, ezgl::application* app);
void help_callback(GtkWidget* /*widget*/, ezgl::application* app);
void colorblind_callback(GtkWidget* /*widget*/, ezgl::application* app);
void directions_callback(GtkWidget* /*widget*/, ezgl::application* app);
double distance(StreetSegmentIdx seg);
#endif /* UIHELPER_H */

