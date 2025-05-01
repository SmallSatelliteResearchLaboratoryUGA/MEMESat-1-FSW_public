#!/bin/bash 

source zephyr-venv/bin/activate
cd ../../libs/zephyr
west update
west zephyr-export
pip install -r zephyr/scripts/requirements.txt
source zephyr/zephyr-env.sh
cd ../../fsw/eps