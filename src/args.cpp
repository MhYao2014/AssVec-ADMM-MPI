//
// Created by mhyao on 20-1-10.
//

#include "args.h"

Args::Args() {
    lr = 0.05;
    lrTree = 0.025;
    hyperparam = 1;
    dim = 100;
    ws = 5;
    epoch = 5;
    epochTree = 2;
    minCount = 5;
    minCountLabel = 0;
    neg = 5;
    negTree = 5;
    wordNgrams = 1;
    loss = loss_name::ns;
    model = model_name::sg;
    bucket = 2000000;
    minn = 3;
    maxn = 6;
    thread = 12;
    lrUpdateRate = 100;
    t = 1e-4;
    label = "__label__";
    verbose = 2;
    pretrainedVectors = "";
    saveOutput = false;
    IfNeedTree = false;
    IfNeedRegular = false;
    minibatch = 1;

    qout = false;
    retrain = false;
    qnorm = false;
    cutoff = 0;
    dsub = 2;
}
