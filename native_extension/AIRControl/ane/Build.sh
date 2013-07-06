#!/bin/sh

#Check for the required number of arguments.
if [ ! $# == 2 ]; then

#Wrong number of arguments, print usage instructions.
echo -e "USAGE:\n\tBuild.sh <PATH_TO_AIR_SDK> <PATH_TO_DIGITAL_CERTIFICATE>"

else

#Get the path to the script and trim to get the directory.
echo "Setting path to current firectory to:"
pathtome=$0
pathtome="${pathtome%/*}"
echo $pathtome

#Check for existing build.
echo "Checking for existing build."
if [ -e "$pathtome/platforms" ]; then
echo "ERROR: $pathtome/platforms already exists, delete it to rebuild."
elif [ -e "$pathtome/AIRControl.ane" ]; then
echo "ERROR: $pathtome/AIRControl.ane already exists, delete it to rebuild."
elif [ -e "$pathtome/AIRControl.swc" ]; then
echo "ERROR: $pathtome/AIRControl.swc already exists, delete it to rebuild."
elif [ -e "$pathtome/library.swf" ]; then
echo "ERROR: $pathtome/library.swf already exists, delete it to rebuild."
else

#Setup the directory.
echo "Making directories."
mkdir "$pathtome/platforms"
mkdir "$pathtome/platforms/win"
mkdir "$pathtome/platforms/mac"

#Copy SWC into place.
echo "Copying SWC into place."
cp "$pathtome/../bin/AIRControl.swc" "$pathtome/"

#Extract contents of SWC.
echo "Extracting files form SWC."
unzip "$pathtome/AIRControl.swc" "library.swf" -d "$pathtome"

#Copy library.swf to folders.
echo "Copying library.swf into place."
cp "$pathtome/library.swf" "$pathtome/platforms/win"
cp "$pathtome/library.swf" "$pathtome/platforms/mac"

#Copy native libraries into place.
echo "Copying native libraries into place."
cp "$pathtome/../../../native_library/native_library_win/AIRControl/bin/Release/AIRControl.dll" "$pathtome/platforms/win"
cp -r "$pathtome/../../../native_library/native_library_mac/AIRControl/build/Release/AIRControl.framework" "$pathtome/platforms/mac"

#Add the AIR SDK bin folder to the PATH.
echo "Setting PATH to:"
export PATH="${1}/bin:${PATH}"
echo $PATH

#Set the path to the keystore.
echo "Settings path to keystore to:"
keystore=$2
echo $keystore

#Run the build command.
echo "Building."
adt -package \
-storetype pkcs12 \
-keystore "$keystore" \
-target ane "$pathtome/AIRControl.ane" "$pathtome/extension.xml" \
-swc "$pathtome/AIRControl.swc" \
-platform Windows-x86 -C "$pathtome/platforms/win" "AIRControl.dll" "library.swf" \
-platform MacOS-x86 -C "$pathtome/platforms/mac" "AIRControl.framework" "library.swf"

fi

fi
