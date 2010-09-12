#! /bin/bash

exe=./bin/conditional_density_estimation

T=1000
D=16
D_c=8
a=1
b=2

for T in 10 100 1000 10000 100000
do
	for D in 16 #4 8 12 16 32 64 128
	do
		for D_c in 16 #4 8 12 16 32 64 128
		do
			echo $T $D $D_c
			${exe} $T $D $D_c $a $b >out
			grep P_XY out > results/P_XY_${T}_${D}_${D_c}
			grep P_Y_X out > results/P_Y_X_${T}_${D}_${D_c}
		done
	done
done