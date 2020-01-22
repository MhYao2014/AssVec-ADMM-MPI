# AssVec-ADMM-MPI
The admm version of AssVec model. Implemented based on openmpi and openmp. 

Conceptually, openmpi will open a bunch of processes to handle different groups of training corpus. Each process will open another bunch of threads to apply the Hogwild! optimizer to each training corpus in this group.
The processes don't share the memory with each other, while threads within the same process do.

In my case, I have access to USTC's supercomputer clusters, which have enough cores for me.
I used 18 processes with 28 threads each process, which sums up to 504 cores totally, to train the word vectors for the top 100000 frequent words. I sincerely hope that I can have access to 100000 cores.

How to determine the number of processes and threads? The principle is to make sure that each training corpus groups are roughly the same size so that no cores will wait too long for other cores. The groupData.cc can help you figure out how to decide these numbers. How to use it? This file will explain for itself if you know how to compile it into an executable and use the -h flag.
## Pipeline
There are three steps to train the AssVec model from the scratch.
+ Step 1. Collect a cleaned training corpus from the web. You don't have to remove the punctuation or make one sentence per line in this single training corpus file. Then, you run the dictionary executable to build the vocabulary. If the size of this vocabulary is 100000, then this executable will also split the original training corpus file into 100000 files, where each file corresponds to a word and is named after with this word's index in the vocabulary. Each file records the unigram concurrent statistics in the original training corpus (with words' vocab index recorded). The graph bellow illustrates the general idea in this step.

![image](https://github.com/MhYao2014/AssVec-ADMM-MPI/blob/master/images/stempOne.png)
+ Step 2. Group your files based on your machine's total core numbers and architecture. Because the word frequence obeys Zipf's law, therefore the files will also have a very unbalanced size distribution. For example, 1.1G for the largest file, while 5M for the smallest one. Each file is a subproblem that should be solved parallelly by different processes. However, due to the limit amounts of cores we can access, we have to gather the files into roughly same size groups so that each group is solved parallelly. In contrast, each file is handled serially within each group. You can use the groupData.cc script to help you determine a suitable number of groups.
+ Step 3. Based on your groups'total number (which is the total processes number) and the available cores, decide how many threads each process should have. Then, run the AssVec executable on your machine using these two numbers.
## How to build this project
I decide to adopt makefile to compile and link my scripts.
+ If you do not know what is makefile, you may want to learn this tool firstly.
+ Try to figure out the concepts of "assembly code", "objective file", "machine code", "preprocess", "compile/compiler" and "link/linker", and then you will understand what's going on in the makefile.
The header and source files are in the "src" folder.
To build a project form these scripts, simply use the command,
```
make
```
, in the command line. 

Then, the makefile will put the executable of AssVec and a bunch of other objective files into the build folder. The AssVec executable is all you need. You can decide where to put building results by modifying the makefile. How? Google it.
