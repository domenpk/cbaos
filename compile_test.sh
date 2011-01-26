#!/bin/bash

all=0
failed=0

echo "trying to compile all configurations from configs/"
for i in configs/*; do
	let all++
	./use_config.py $i; make &>/dev/null
	if [ $? -ne 0 ]; then
		echo -e "\t$i failed"
		let failed++
	fi
	make clean &>/dev/null
done

echo "testing results: $failed/$all failed"
