/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRVT_PAD_H_
#define FRVT_PAD_H_

#include <cstdint>
#include <string>

#include <frvt_structs.h>

namespace FRVT_PAD {

/**
 * @brief
 * The interface to FRVT PAD implementation
 *
 * @details
 * The submission software under test will implement this interface by
 * sub-classing this class and implementing each method therein.
 */
class Interface {
public:
    virtual ~Interface() {}

    /**
     * @brief This function initializes the implementation under test.  It will
     * be called by the NIST application before any call to createTemplate() or
     * matchTemplates().  The implementation under test should set all parameters.
     * This function will be called N=1 times by the NIST application, prior to
     * parallelizing M >= 1 calls to createTemplate() via fork().
     *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
     * parameters or run-time data files.  The name of this directory is
     * assigned by NIST, not hardwired by the provider.  The names of the
     * files in this directory are hardwired in the implementation and are
     * unrestricted.
     */
    virtual FRVT::ReturnStatus
    initialize(const std::string &configDir) = 0;

    /**
     * @brief This function takes an input piece of media (image or sequential video frames) and outputs
     * 1. a binary decision on whether the media contains a presentation attack (PA)
     * 2. a "padiness" score on [0, 100] indicating how confident the algorithm
     * thinks the media is a PA, with 0 meaning confidence that
     * the media is not a PA and 100 representing absolute confidence that it is a PA
     *
     * param[in] suspectedPA
     * Input media (image or sequential video frames)
     * param[out] isPA
     * True if media contains a presentation attack; False otherwise
     * param[out] score
     * A score on [0, 100] representing how confident the algorithm is that the media
     * contains a PA.  0 means certainty that the media does not contain a PA, and 100
     * represents certainty that the media contains a PA
     */
    virtual FRVT::ReturnStatus
    detectPA(
        const FRVT::Media &suspectedPA,
        bool &isPA,
        double &score) = 0;

    /**
     * @brief
     * Factory method to return a managed pointer to the Interface object.
     * @details
     * This function is implemented by the submitted library and must return
     * a managed pointer to the Interface object.
     *
     * This function MUST be implemented.
     *
     * @note
     * A possible implementation might be:
     * return (std::make_shared<Implementation>());
     */
    static std::shared_ptr<Interface>
    getImplementation();
};

/*
 * API versioning
 *
 * NIST code will extern the version number symbols.
 * Participant shall compile them into their core library.
 */
#ifdef NIST_EXTERN_API_VERSION
/** API major version number. */
extern uint16_t API_MAJOR_VERSION;
/** API minor version number. */
extern uint16_t API_MINOR_VERSION;
#else /* NIST_EXTERN_API_VERSION */
/** API major version number. */
uint16_t API_MAJOR_VERSION{0};
/** API minor version number. */
uint16_t API_MINOR_VERSION{1};
#endif /* NIST_EXTERN_API_VERSION */
}

#endif /* FRVT_PAD_H_ */
