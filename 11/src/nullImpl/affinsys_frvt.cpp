//
// Created by varunsakunia on 09/03/22.
//

#include <string>
#include <list>
#include <iterator>
#include <vector>
#include <functional>
#include <cmath>
#include <numeric>

#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/opencv.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include "../common/src/include/frvt_structs.h"


using namespace dlib;
using namespace std;

// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*
    This is an example illustrating the use of the deep learning tools from the dlib C++
    Library.  In it, we will show how to do face recognition.  This example uses the
    pretrained dlib_face_recognition_resnet_model_v1 model which is freely available from
    the dlib web site.  This model has a 99.38% accuracy on the standard LFW face
    recognition benchmark, which is comparable to other state-of-the-art methods for face
    recognition as of February 2017.

    In this example, we will use dlib to do face clustering.  Included in the examples
    folder is an image, bald_guys.jpg, which contains a bunch of photos of action movie
    stars Vin Diesel, The Rock, Jason Statham, and Bruce Willis.   We will use dlib to
    automatically find their faces in the image and then to automatically determine how
    many people there are (4 in this case) as well as which faces belong to each person.

    Finally, this example uses a network with the loss_metric loss.  Therefore, if you want
    to learn how to train your own models, or to get a general introduction to this loss
    layer, you should read the dnn_metric_learning_ex.cpp and
    dnn_metric_learning_on_images_ex.cpp examples.
*/

// ----------------------------------------------------------------------------------------

// The next bit of code defines a ResNet network.  It's basically copied
// and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
// layer with loss_metric and made the network somewhat smaller.  Go read the introductory
// dlib DNN examples to learn what all this stuff means.
//
// Also, the dnn_metric_learning_on_images_ex.cpp example shows how to train this network.
// The dlib_face_recognition_resnet_model_v1 model used by this example was trained using
// essentially the code shown in dnn_metric_learning_on_images_ex.cpp except the
// mini-batches were made larger (35x15 instead of 5x5), the iterations without progress
// was set to 10000, and the training dataset consisted of about 3 million images instead of
// 55.  Also, the input layer was locked to images of size 150.
template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
        alevel0<
                alevel1<
                        alevel2<
                                alevel3<
                                        alevel4<
                                                max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                                                        input_rgb_image_sized<150>
                                                >>>>>>>>>>>>;

// ----------------------------------------------------------------------------------------

std::vector<matrix<rgb_pixel>> jitter_image(
        const matrix<rgb_pixel>& img
);

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

std::vector<matrix<rgb_pixel>> jitter_image(
        const matrix<rgb_pixel>& img
)
{
    // All this function does is make 100 copies of img, all slightly jittered by being
    // zoomed, rotated, and translated a little bit differently. They are also randomly
    // mirrored left to right.
    thread_local dlib::rand rnd;

    std::vector<matrix<rgb_pixel>> crops;
    for (int i = 0; i < 100; ++i)
        crops.push_back(jitter_image(img,rnd));

    return crops;
}

// ----------------------------------------------------------------------------------------

int * eye_coords(const std::string& input_str){

    // Read image using opencv
    cv::Mat Image = cv::imread(input_str);

    //Convert loaded opencv image to dlib image format
    dlib::matrix<dlib::rgb_pixel> dlibImage;
    dlib::load_image(dlibImage, input_str);

    // assigning variables and array for eye centres
    int* arr = new int[4];
    int eye_centre_x_left, eye_centre_y_left,eye_centre_x_right, eye_centre_y_right;

    // Load the dlib face detector
    frontal_face_detector faceDetector = get_frontal_face_detector();

    // Load the dlib face landmark detector
    std::string sp_path = "/home/varunsakunia/dlib_test/shape_predictor_5_face_landmarks.dat";
    dlib::shape_predictor sp;
    dlib::deserialize(sp_path) >> sp;

    // Detect faces in the image and print the number of faces detected
    std::vector<dlib::rectangle> faces = faceDetector(dlibImage);

    // Now we will go ask the shape_predictor to tell us the pose of each face we detected.
    std::vector<dlib::full_object_detection> shapes;
    for (auto & face : faces)
    {
        full_object_detection shape = sp(dlibImage, face);
        eye_centre_x_left = ((shape.part(2) + shape.part(3))/2).x();
        eye_centre_y_left = ((shape.part(2) + shape.part(3))/2).y();
        eye_centre_x_right = ((shape.part(0) + shape.part(1))/2).x();
        eye_centre_y_right = ((shape.part(0) + shape.part(1))/2).y();

        // You get the idea, you can get all the face part locations if
        // you want them.  Here we just store them in shapes so we can
        // put them on the screen.
        shapes.push_back(shape);
    }

    // Allocating to an array
    arr[0] = eye_centre_x_left;
    arr[1] = eye_centre_y_left;
    arr[2] = eye_centre_x_right;
    arr[3] = eye_centre_y_right;

    return arr;
}

std::vector<float> face_vect(const std::string& input_str)
{
    // filepath
    std::string img_path = input_str;

    // The first thing we are going to do is load all our models.  First, since we need to
    // find faces in the image we will need a face detector:
    frontal_face_detector detector = get_frontal_face_detector();
    // We will also use a face landmarking model to align faces to a standard pose:  (see face_landmark_detection_ex.cpp for an introduction)
    shape_predictor sp;
    deserialize("/home/varunsakunia/dlib_test/shape_predictor_5_face_landmarks.dat") >> sp;
    // And finally we load the DNN responsible for face recognition.
    anet_type net;
    deserialize("/home/varunsakunia/dlib_test/dlib_face_recognition_resnet_model_v1.dat") >> net;

    matrix<rgb_pixel> img;
    load_image(img, img_path);

    // Run the face detector on the image of our action heroes, and for each face extract a
    // copy that has been normalized to 150x150 pixels in size and appropriately rotated
    // and centered.
    std::vector<matrix<rgb_pixel>> faces;
    for (auto face : detector(img))
    {
        auto shape = sp(img, face);
        matrix<rgb_pixel> face_chip;
        extract_image_chip(img, get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(move(face_chip));
    }

    // This call asks the DNN to convert each face image in faces into a 128D vector.
    // In this 128D vector space, images from the same person will be close to each other
    // but vectors from different people will be far apart.  So we can use these vectors to
    // identify if a pair of images are from the same person or from different people.
    std::vector<matrix<float,0,1>> face_descriptors = net(faces);

    // In particular, one simple thing we can do is face clustering.  This next bit of code
    // creates a graph of connected faces and then uses the Chinese whispers graph clustering
    // algorithm to identify how many people there are and which faces belong to whom.
    std::vector<sample_pair> edges;
    for (size_t i = 0; i < face_descriptors.size(); ++i)
    {
        for (size_t j = i; j < face_descriptors.size(); ++j)
        {
            // Faces are connected in the graph if they are close enough.  Here we check if
            // the distance between two face descriptors is less than 0.6, which is the
            // decision threshold the network was trained to use.  Although you can
            // certainly use any other threshold you find useful.
            if (length(face_descriptors[i]-face_descriptors[j]) < 0.6)
                edges.push_back(sample_pair(i,j));
        }
    }
    std::vector<unsigned long> labels;
    const auto num_clusters = chinese_whispers(edges, labels);
    // This will correctly indicate that there are 4 people in the image.

    // Now let's display the face clustering results on the screen.  You will see that it
    // correctly grouped all the faces.
    std::vector<image_window> win_clusters(num_clusters);
    for (size_t cluster_id = 0; cluster_id < num_clusters; ++cluster_id)
    {
        std::vector<matrix<rgb_pixel>> temp;
        for (size_t j = 0; j < labels.size(); ++j)
        {
            if (cluster_id == labels[j])
                temp.push_back(faces[j]);
        }

    }

    // It should also be noted that face recognition accuracy can be improved if jittering
    // is used when creating face descriptors.  In particular, to get 99.38% on the LFW
    // benchmark you need to use the jitter_image() routine to compute the descriptors,
    // like so:
    matrix<float,0,1> face_descriptor = mean(mat(net(jitter_image(faces[0]))));

// Normalisation to min-0 and max-256
//    int GetRangedValue(float value, double min, double max)
//    {
//        int outputLimit = 256;
//        double range = (max - min) - double.Epsilon; // Here we shorten the range slightly
//        // Then we build a range such that value >= 0 and value < 1
//        double rangedValue = (value - min) / range;
//        return min + (int)(outputLimit * rangedValue);
//    }

    std::vector<float> g1;
    float min = *min_element(face_descriptor.begin(), face_descriptor.end());
    float max = *max_element(face_descriptor.begin(), face_descriptor.end());

    for (long r = 0; r < face_descriptor.nr(); ++r)
    {
        // loop over all the columns
        for (long c = 0; c < face_descriptor.nc(); ++c)
        {
            float old_val = face_descriptor(r,c);
            auto new_val = (float) ((old_val - min)*(256/(max-min)));
            g1.push_back(new_val);
        }
    }

    //shrink_to_fit() – Reduces the capacity of the container to fit its size and destroys all elements beyond the capacity.
    g1.shrink_to_fit();
    // If you use the model without jittering, as we did when clustering the bald guys, it
    // gets an accuracy of 99.13% on the LFW benchmark.  So jittering makes the whole
    // procedure a little more accurate but makes face descriptor calculation slower.

    return g1;
}

// Computes the distance between two std::vectors
double vectors_distance(const std::vector<float>& a, const std::vector<float>& b)
{
    std::vector<float> auxiliary;
    std::transform (a.begin(), a.end(), b.begin(), std::back_inserter(auxiliary),//
                    [](float element1, float element2) {return pow((element1-element2),2);});
    return  std::sqrt(std::accumulate(auxiliary.begin(), auxiliary.end(), 0.0));
} // end template vectors_distance




