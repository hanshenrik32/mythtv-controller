#!/bin/bash
if [[ ! -f build-number.txt ]]
 then 
	 touch build-number.txt
 fi
make compile

