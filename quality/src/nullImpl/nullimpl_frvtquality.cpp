/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <cstring>
#include <cstdlib>
#include <random>

#include "nullimpl_frvtquality.h"

using namespace std;
using namespace FRVT;
using namespace FRVT_QUALITY;

NullImplFRVTQuality::NullImplFRVTQuality() {}

NullImplFRVTQuality::~NullImplFRVTQuality() {}

ReturnStatus
NullImplFRVTQuality::initialize(const std::string &configDir)
{
    // Read stuff (models, etc.) in from configDir
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTQuality::scalarQuality(
        const FRVT::Image &face,
        double &quality)
{
    quality = (rand() % 100) + 1; 
    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
NullImplFRVTQuality::vectorQuality(
    const FRVT::Image &face,
    std::vector<FRVT::QualityElementValues> &qualityVector,
    std::vector<FRVT::EyePair> &eyeCoordinates)
{
    std::uniform_real_distribution<double> dist(-90, 90); 
    std::mt19937 rng; 
    rng.seed(std::random_device{}()); 
    
    for (int i = 0; i < rand() % 3; i++) {
        QualityElementValues qualityMap;
        qualityMap[QualityElement::SubjectPoseYaw] = dist(rng);
        qualityMap[QualityElement::SubjectPosePitch] = dist(rng);
        qualityMap[QualityElement::SubjectPoseRoll] = dist(rng);    
        qualityVector.push_back(qualityMap);
        eyeCoordinates.push_back(EyePair(true, true, 50+i, 25+i, 20+i, 25+i));
    }
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<NullImplFRVTQuality>();
}

