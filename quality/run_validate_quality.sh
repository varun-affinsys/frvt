#!/bin/bash
success=0
failure=1

bold=$(tput bold)
normal=$(tput sgr0)

# Check version of OS
reqOS="CentOS Linux release 8.2.2004 (Core) "
currentOS=$(cat /etc/centos-release)
if [ "$reqOS" != "$currentOS" ]; then
	echo "${bold}[ERROR] You are not running the correct version of the operating system, which should be $reqOS.  Please install the correct operating system and re-run this validation package.${normal}"
	exit $failure
fi
 
# Install the necessary packages to run validation
echo -n "Checking installation of required packages "
for package in coreutils gawk gcc gcc-c++ grep cmake make sed libarchive
do
	yum -q list installed $package &> /dev/null
	retcode=$?
	if [[ $retcode != 0 ]]; then
		sudo yum install -y $package
	fi
    # Need a later version to support cmake for now
    if [ "$package" == "libarchive" ]; then
        sudo yum update -y $package
    fi	
done
echo "[SUCCESS]"


# Compile and build implementation library against
# validation test driver
scripts/compile_and_link.sh
retcode=$?
if [[ $retcode != 0 ]]; then
	exit $failure
fi

# Set dynamic library path to the folder location of the developer's submission library
export LD_LIBRARY_PATH=$(pwd)/lib

# Run testdriver against linked library
# and validation images
scripts/run_testdriver.sh
retcode=$?
if [[ $retcode != 0 ]]; then
	exit $failure
fi

outputDir="validation"
# Do some sanity checks against the output logs
echo -n "Sanity checking validation output "
numInputLines=$(cat input/quality.txt | wc -l)
numLogs=0
for action in scalarQ vectorQ 
do
	# Make sure all images in input file have been processed
	if [ -e "$outputDir/$action.log" ]; then
		numLogs=$((numLogs+1))
		numLogLines=$(sed '1d' $outputDir/$action.log | sort -k1n,1 -u | wc -l)
		if [ "$numInputLines" != "$numLogLines" ]; then
			echo "[ERROR] The $outputDir/$action.log file does not include results for all of the input images.  Please re-run the validation test."
			exit $failure
		fi

		# Check return codes
		numFail=$(sed '1d' $outputDir/$action.log | awk '{ if($3!=0) print }' | wc -l)
		if [ "$numFail" != "0" ]; then
			echo -e "\n${bold}[WARNING] The following entries in $action.log generated non-successful return codes:${normal}"
			sed '1d' $outputDir/$action.log | awk '{ if($3!=0) print }'
		fi
	fi
done

if [ $numLogs == 0 ]; then
	echo "[ERROR] There are no output logs in the validation folder.  Please make sure you have implemented at least one of the quality functions."
	exit $failure
fi

echo "[SUCCESS]"

# Create submission archive
echo -n "Creating submission package "
libstring=$(basename `ls ./lib/libfrvt_quality_*_???.so`)
libstring=${libstring%.so}

for directory in config lib validation doc
	do
	if [ ! -d "$directory" ]; then
		echo "[ERROR] Could not create submission package.  The $directory directory is missing."
		exit $failure	
	fi
done

# write OS to text file
cat /etc/redhat-release > validation/os.txt

tar -zcf $libstring.tar.gz ./config ./lib ./validation ./doc
echo "[SUCCESS]"
echo "
#################################################################################################################
A submission package has been generated named $libstring.tar.gz.  

This archive must be properly encrypted and signed before transmission to NIST.
This must be done according to these instructions - https://www.nist.gov/sites/default/files/nist_encryption.pdf
using the LATEST FRVT Ongoing public key linked from -
https://www.nist.gov/itl/iad/image-group/products-and-services/encrypting-softwaredata-transmission-nist.

For example:
      gpg --default-key <ParticipantEmail> --output <filename>.gpg \\\\
      --encrypt --recipient frvt@nist.gov --sign \\\\
      libfrvt_quality_<organization>_<three-digit submission sequence>.tar.gz

Send the encrypted file and your public key to NIST.  You can
      a) Email the files to frvt@nist.gov if your package is less than 20MB OR
      b) Provide a download link from a generic http webserver (NIST will NOT register or establish any kind of
         membership on the provided website) OR
      c) Mail a CD/DVD to NIST at the address provided in the participation agreement
##################################################################################################################
"
