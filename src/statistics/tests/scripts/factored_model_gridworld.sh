#! /bin/sh

outdir=~/experiments/bvmdp/gridworld/maze1/prior_01
mkdir -p $outdir
T=100000
iter=100
states=0
echo "factored_model $iter $T" >${outdir}/run.params

for depth in 0 1 2 3 4 5 6 7 8 
do
    for model in FMC BFMC BVMM CTW
    do
        echo $model $depth
        time ./bin/factored_model $iter $T $states $depth $model >${outdir}/${states}s_${depth}d_${model}.out
    done
done
