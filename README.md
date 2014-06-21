This is Evolution of Swarming project.

Copyright 2012 Randal S. Olson, Arend Hintze.

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

Compiling
====================

Enter the command ./build_abeeda

If the console gives an error about permissions, enter:

chmod 755 build_abeeda

then enter the above build command again.

Usage
====================

Type ./abeeda to run the simulation. The following parameters can be passed to aBeeDa:

* -e [LOD out file name] [prey genome out file name] [predator genome out file name]: evolve
* -d [prey genome in file name] [predator genome in file name]: display 
* -dd [directory of prey and predator genome files]: display all genome files in a given directory
* -s [int]: set random number generator seed
* -g [int]: set generations to evolve for
* -t [int]: save best brain every [int] generations
* -v [int]: make video of best brains at the given interval
* -lv: make video of LOD of best agent brain at the end of run
* -lt [genome in file name] [out file name]: create logic table for given genome
* -dfs [genome in file name] [dot out file name]: create dot image file for given swarm genome
* -dfp [genome in file name] [dot out file name]: create dot image file for given predator genome
* -sd [int]: set swarm safety distance (default: 30)
* -pva [int]: set predator vision angle (default: 180)
* -kd [int]: set predator kill attempt delay (default: 10)

-e, -d, -dd, -dfs, or -dfp must be passed to abeeda, otherwise it will not do anything by default.

Output
====================

aBeeDa produces a variety of output files, detailed below.

LOD files
---------------------

There will be a single entry for each ancestor in the final best swarm agent's LOD.

LOD files will be in csv format with the column headers listed at the top. Column headers are in the following order:

* generation: the generation the ancestor was born
* prey_fitness: the fitness of the ancestor prey
* predator_fitness: the fitness of the ancestor predator
* num_alive_end: the number of surviving prey at the end of the fitness evaluation
* avg_bb_size: the average bounding box size of the swarm during the simulation
* var_bb_size: the variance in the bounding box size of the swarm during the simulation
* avg_shortest_dist: the average distance from every prey agent to the prey agent closest to it
* swarm_density_count: the average number of prey agents within *safety distance* units of each other
* prey_neurons_connected_prey_retina: the number of conspecific sensor neurons that the prey Markov network brain is connected to
* prey_neurons_connected_predator_retina: the number of predator sensor neurons that the prey Markov network brain is connected to
* predator_neurons_connected_prey_retina: the number of prey sensor neurons that the predator Markov network brain is connected to
* mutual_info: the average amount of mutual information between all of the prey's navigation angles and the predator's navigation angle

Markov network brain files
---------------------

Generally, we save Markov network brain files as .genome files.

These files contain integer values which encode the Markov network brain.

Logic table files
---------------------

The logic table files contain the logic table for the most-likely decision made by the Markov network brain.

They are formatted specifically for the Logic Friday logic optimization program. They should be able to be fed directly into the Logic Friday program without any modification.

DOT files
---------------------

DOT files are the picture representations of Markov network brain structure and connectivity. We recommend using the Graphviz software to view these images.