#!/bin/bash

all=0
failed=0

echo "trying to compile all configurations from configs/"
for i in configs/*; do
	let all++
	./scripts/use_config.py "$i"

	if [ -z "$V" ]; then
		make &>/dev/null
	else
		make
	fi
	if [ $? -ne 0 ]; then
		echo -e "\t$i failed"
		let failed++
	fi
	make clean &>/dev/null
done

echo "testing results: $failed/$all failed"
