#!/bin/bash

TIMEFORMAT='It took %R seconds.'
time {
for i in $(seq 1 10)
do
    ./concurrent_client &
done
}