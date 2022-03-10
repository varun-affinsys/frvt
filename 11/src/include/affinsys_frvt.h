//
// Created by varunsakunia on 09/03/22.
//

#ifndef CHECK_AFFINSYS_FRVT_H
#define CHECK_AFFINSYS_FRVT_H

#include <list>
#include <iterator>
#include <string>

#include <dlib/opencv.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

int * eye_coords(const std::string& input_str);
std::vector<float> face_vect(const std::string& input_str);
double vectors_distance(const std::vector<float>& a, const std::vector<float>& b);

#endif //CHECK_AFFINSYS_FRVT_H
