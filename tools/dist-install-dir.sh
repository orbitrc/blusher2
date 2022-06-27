#!/bin/bash

source /usr/lib/os-release

if [ $ID == "ubuntu" ]; then
	echo "/usr/lib/x86_64-linux-gnu/"
else
	echo "/usr/lib/"
fi

