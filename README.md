This is the Evolved Digit Detector project.

Copyright 2016 Randal S. Olson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Compiling

Enter the command `./build_edd`

If the console gives an error about permissions, enter `chmod 755 build_edd` and enter the above build command again.

# Usage

Type ./edd to run the simulation. The following parameters can be passed to aBeeDa:

* -e [LOD out file name] [genome out file name]: evolve
* -d [genome in file name]: display 
* -dd [directory of genome files]: display all genome files in a given directory
* -s [int]: set random number generator seed
* -g [int]: set generations to evolve for
* -t [int]: save best brain every [int] generations
* -v [int]: make video of best brains at the given interval
* -lv: make video of LOD of best agent brain at the end of run
* -lt [genome in file name] [out file name]: create logic table for given genome
* -df [genome in file name] [dot out file name]: create dot image file for given genome
* -zc: allow the active categorical classifier to move around and scan the image
* -rs: force the active categorical classifier to start at random positions in the image for every image (forces it to learn to generalize)
* -gs [int] [int]: the width and height of the image in pixels, respectively; strongly recommended to keep the two values equal

-e, -d, -dd, or -df must be passed to edd, otherwise it will not do anything by default.

# Output

edd produces a variety of output files, detailed below.

## LOD files

There will be a single entry for each ancestor in the final best swarm agent's LOD.

LOD files will be in csv format with the column headers listed at the top. Column headers are in the following order:

* generation: the generation the ancestor was born
* fitness: the fitness of the ancestor prey

## Markov network files

Generally, we save Markov network files as .genome files.

These files contain integer values which encode the Markov network.

## Logic table files

The logic table files contain the logic table for the most-likely decision made by the Markov network.

They are formatted specifically for the Logic Friday logic optimization program. They should be able to be fed directly into the Logic Friday program without any modification.

## DOT files

DOT files are the picture representations of Markov network structure and connectivity. We recommend using the Graphviz software to view these images.

# Experiment reproducibility

To reproduce the latest results with the active categorical classifier, pass the following parameters to the program.

```
./edd -e LOD.csv run1agent.genome -s 1 -g 1000000 -zc -rs -gs 28 28 -mr 0.0005 -t 5000
```

This command will optimize the active categorical classifiers on the MNIST data set for 1,000,000 generations. It will likely take several weeks before the run finishes, so make sure to run it on dedicated, long-running hardware.
