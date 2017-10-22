#!/bin/sh

for i in configs/*; do ./use_config.py $i && make clean && make || break; done
