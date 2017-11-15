#!/bin/bash

all=0
failed=0

echo "trying to compile all configurations from configs/"
for i in configs/*; do
	let all++
	./scripts/use_config.py "$i"

	prefix=""
	if [ -z "$V" ]; then
		echo -en "\t$i ..."
		make &>/dev/null
	else
		prefix="\t$i"
		make
	fi
	if [ $? -ne 0 ]; then
		echo -e "$prefix fail"
		let failed++
	else
		echo -e "$prefix OK"
	fi
	make clean &>/dev/null
done

echo "testing results: $failed/$all failed"
