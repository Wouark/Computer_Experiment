#!/bin/zsh

for var in {1..5}
do
    ./compile ../samples1/arith${var}.cpr > ../samples1/arith${var}.pco
    ./run ../samples1/arith${var}.pco > ../samples1/arith${var}_output.text
done
