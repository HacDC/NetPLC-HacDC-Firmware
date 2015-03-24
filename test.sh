#!/bin/bash
. ubiquitous_bash.sh

scriptAbsoluteFolder="$(getScriptAbsoluteFolder)"	#Start in script's own location.

#Improves make readability.
#Depends on colout (http://nojhan.github.io/colout/) and unbuffer (http://expect.sourceforge.net/) .
readableMake() {

	if [[ -e $(which colout) && -e $(which unbuffer) ]]
	then
		unbuffer make -j6 "$@" 2>&1 | colout -t cmake | grep --color=always -E '^|undefined reference'
	else
		make -j6 "$@"
	fi
}

#Improves cmake readability.
#Depends on colout (http://nojhan.github.io/colout/) .
readableCMake() {
	if [[ -e $(which colout) && -e $(which unbuffer) ]]
	then 
		cmake "$@" | colout -t cmake
	else
		cmake "$@"
	fi
}


#Produces out-of-source build in Release folder.
cd "$scriptAbsoluteFolder"
mkdir -p ./build
cd ./build
readableCMake -DCMAKE_BUILD_TYPE=Release ../
readableMake upload

sleep 3

#../cmake/ard-reset-arduino --caterina /dev/ttyACM0

sleep 3

make Blink-serial

echo -e '\E[1;32;46m Compilation finished. \E[0m'