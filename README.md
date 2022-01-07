# Face Recognition Vendor Test (FRVT) Validation Packages for Ubuntu
Starting February 14th 2022, we will be switching the FRVT evaluation operating system to Ubuntu 20.04.3 LTS (Focal Fossa).  This repository contains Ubuntu-compatible validation packages for all [Ongoing FRVT evaluation](https://www.nist.gov/programs-projects/face-recognition-vendor-test-frvt-ongoing) tracks.    Developers can download the Ubuntu 20.04.3 LTS ISO file from [here](https://nigos.nist.gov/evaluations/ubuntu-20.04.3-live-server-amd64.iso).

# API updates
Additionally, this development branch includes API updates for the 1:1 and 1:N tracks that will be rolled out on February 14th, 2022.  A second version of the createTemplate() function has been added to the APIs to support the potential existence of multiple people in an image.

Draft versions of the [1:1 API document version 5.0](https://pages.nist.gov/frvt/api/FRVT_ongoing_11_api_5.0_draft.pdf) and [1:N API document version 2.0](https://pages.nist.gov/frvt/api/FRVT_ongoing_1N_api_v2.0_draft.pdf) are available for comment.  Please direct any questions or comments to frvt@nist.gov.

# Cloning the repository
We recommend developers clone the entire repository and run validation from within
the folder that corresponds to the evaluation of interest.  The ./common directory
contains files that are shared across all validation packages.

