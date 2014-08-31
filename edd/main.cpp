/*
 * main.cpp
 *
 * This file is part of the Evolved Digit Detector project.
 *
 * Copyright 2014 Randal S. Olson, Arend Hintze.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <vector>
#include <map>
#include <math.h>
#include <time.h>
#include <iostream>
#include <dirent.h>

#include "globalConst.h"
#include "tHMM.h"
#include "tAgent.h"
#include "tGame.h"

string  findBestRun(tAgent *eddAgent);

using namespace std;

double  perSiteMutationRate         = 0.005;
int     populationSize              = 100;
int     totalGenerations            = 252;
tGame   *game                       = NULL;

bool    make_interval_video         = false;
int     make_video_frequency        = 25;
bool    make_LOD_video              = false;
bool    track_best_brains           = false;
int     track_best_brains_frequency = 25;
bool    display_only                = false;
bool    display_directory           = false;
bool    make_logic_table            = false;
bool    make_dot_edd                = false;
int     gridSizeX                   = 5;
int     gridSizeY                   = 5;
bool    zoomingCamera               = false;
bool    randomPlacement             = false;
bool    noise                       = false;
float   noiseAmount                 = 0.05;

int main(int argc, char *argv[])
{
	vector<tAgent*> eddAgents, EANextGen;
	tAgent *eddAgent = NULL, *bestEddAgent = NULL;
	double eddMaxFitness = 0.0;
    string LODFileName = "", eddGenomeFileName = "", inputGenomeFileName = "";
    string eddDotFileName = "", logicTableFileName = "";
    int displayDirectoryArgvIndex = 0;
    
    // initial object setup
    eddAgents.resize(populationSize);
	eddAgent = new tAgent;
    
    // time-based seed by default. can change with command-line parameter.
    srand((unsigned int)time(NULL));
    
    for (int i = 1; i < argc; ++i)
    {
        // -d [in file name]: display
        if (strcmp(argv[i], "-d") == 0 && (i + 1) < argc)
        {
            ++i;
            eddAgent->loadAgent(argv[i]);
            
            display_only = true;
        }
        
        // -dd [directory]: display all genome files in a given directory
        if (strcmp(argv[i], "-dd") == 0 && (i + 1) < argc)
        {
            ++i;
            displayDirectoryArgvIndex = i;
            
            display_directory = true;
        }
        
        // -e [out file name] [out file name]: evolve
        else if (strcmp(argv[i], "-e") == 0 && (i + 2) < argc)
        {
            ++i;
            stringstream lodfn;
            lodfn << argv[i];
            LODFileName = lodfn.str();
            
            ++i;
            stringstream sgfn;
            sgfn << argv[i];
            eddGenomeFileName = sgfn.str();
        }
        
        // -s [int]: set seed
        else if (strcmp(argv[i], "-s") == 0 && (i + 1) < argc)
        {
            ++i;
            srand(atoi(argv[i]));
            
            cout << "random seed set to " << atoi(argv[i]) << endl;
        }
        
        // -g [int]: set generations
        else if (strcmp(argv[i], "-g") == 0 && (i + 1) < argc)
        {
            ++i;
            totalGenerations = atoi(argv[i]);
            
            if (totalGenerations < 5)
            {
                cerr << "minimum number of generations permitted is 5." << endl;
                exit(0);
            }
            
            cout << "generations set to " << totalGenerations << endl;
        }
        
        // -t [int]: track best brains
        else if (strcmp(argv[i], "-t") == 0 && (i + 1) < argc)
        {
            track_best_brains = true;
            ++i;
            track_best_brains_frequency = atoi(argv[i]);
            
            if (track_best_brains_frequency < 1)
            {
                cerr << "minimum brain tracking frequency is 1." << endl;
                exit(0);
            }
        }
        
        // -v [int]: make video of best brains at an interval
        else if (strcmp(argv[i], "-v") == 0 && (i + 1) < argc)
        {
            make_interval_video = true;
            ++i;
            make_video_frequency = atoi(argv[i]);
            
            if (make_video_frequency < 1)
            {
                cerr << "minimum video creation frequency is 1." << endl;
                exit(0);
            }
        }
        
        // -lv: make video of LOD of best agent brain at the end
        else if (strcmp(argv[i], "-lv") == 0)
        {
            make_LOD_video = true;
        }
        
        // -lt [in file name] [out file name]: create logic table for given genome
        else if (strcmp(argv[i], "-lt") == 0 && (i + 2) < argc)
        {
            ++i;
            eddAgent->loadAgent(argv[i]);
            eddAgent->setupPhenotype();
            ++i;
            stringstream ltfn;
            ltfn << argv[i];
            logicTableFileName = ltfn.str();
            make_logic_table = true;
        }
        
        // -df [in file name] [out file name]: create dot image file for given genome
        else if (strcmp(argv[i], "-df") == 0 && (i + 2) < argc)
        {
            ++i;
            eddAgent->loadAgent(argv[i]);
            eddAgent->setupPhenotype();
            ++i;
            stringstream dfn;
            dfn << argv[i];
            eddDotFileName = dfn.str();
            make_dot_edd = true;
        }
        
        // -gs [int] [int]: set the digit grid size
        else if (strcmp(argv[i], "-gs") == 0 && (i + 2) < argc)
        {
            ++i;
            gridSizeX = atoi(argv[i]);
            ++i;
            gridSizeY = atoi(argv[i]);
            
            if (gridSizeX < 5 || gridSizeY < 5)
            {
                cerr << "minimum grid size dimension is 5." << endl;
                exit(0);
            }
            
            cout << "grid size set to: (" << gridSizeX << ", " << gridSizeY << ")" << endl;
        }
        
        // -zc: allow the edd agent to use a zooming camera
        else if (strcmp(argv[i], "-zc") == 0)
        {
            cout << "zooming camera enabled" << endl;
            zoomingCamera = true;
        }
        
        // -rp: randomly place the digits within the grid. if randomPlacement = false,
        // the digits are always centered
        else if (strcmp(argv[i], "-rp") == 0)
        {
            cout << "random placement of digits enabled" << endl;
            randomPlacement = true;
        }
        
        // -noise [float]: add noise to the edd agent's camera; each input bit is flipped
        // with the probability given (0.0 = never, 1.0 = always flipped)
        else if (strcmp(argv[i], "-noise") == 0 && (i + 1) < argc)
        {
            noise = true;
            ++i;
            noiseAmount = atof(argv[i]);
            cout << "noise enabled with probability: " << noiseAmount << endl;
        }
    }
    
    // set up the game
    game = new tGame;
    
    if (display_only || display_directory || make_interval_video || make_LOD_video)
    {
        // start monitor first, then edd
        //setupBroadcast();
    }
    
    if (display_only)
    {
        string bestString = findBestRun(eddAgent);
        bestString.append("X");
        //doBroadcast(bestString);
        exit(0);
    }
    
    if (display_directory)
    {
        DIR *dir;
        struct dirent *ent;
        dir = opendir(argv[displayDirectoryArgvIndex]);
        
        // map: run # -> [swarm file name, predator file name]
        map< int, vector<string> > fileNameMap;
        
        if (dir != NULL)
        {
            cout << "reading in files" << endl;
            
            // read all of the files in the directory
            while ((ent = readdir(dir)) != NULL)
            {
                string dirFile = string(ent->d_name);
                
                if (dirFile.find(".genome") != string::npos)
                {
                    // find the first character in the file name that is a digit
                    int i = 0;
                    
                    for ( ; i < dirFile.length(); ++i)
                    {
                        if (isdigit(dirFile[i]))
                        {
                            break;
                        }
                    }
                    
                    // get the run number from the file name
                    int runNumber = atoi(dirFile.substr(i).c_str());
                    
                    if (fileNameMap[runNumber].size() == 0)
                    {
                        fileNameMap[runNumber].resize(2);
                    }
                    
                    // map the file name into the appropriate location
                    if (dirFile.find("swarm") != string::npos)
                    {
                        fileNameMap[runNumber][0] = argv[displayDirectoryArgvIndex] + dirFile;
                    }
                    else if (dirFile.find("predator") != string::npos)
                    {
                        fileNameMap[runNumber][1] = argv[displayDirectoryArgvIndex] + dirFile;
                    }
                }
            }
            
            closedir(dir);
        }
        else
        {
            cerr << "invalid directory: " << argv[displayDirectoryArgvIndex] << endl;
            exit(0);
        }
        
        // display every set of swarm/predator files
        for (map< int, vector<string> >::iterator it = fileNameMap.begin(), end = fileNameMap.end(); it != end; )
        {
            if (it->second[0] != "" && it->second[1] != "")
            {
                cout << "building video for run " << it->first << endl;
                
                eddAgent->loadAgent((char *)it->second[0].c_str());
                
                string bestString = findBestRun(eddAgent);
                
                cout << "displaying video for run " << it->first << endl;
                
                if ( (++it) == end )
                {
                    bestString.append("X");
                }
                
                //doBroadcast(bestString);
            }
            else
            {
                cerr << "unmatched file: " << it->second[0] << " " << it->second[1] << endl;
            }
        }
        
        exit(0);
    }
    
    if (make_logic_table)
    {
        eddAgent->saveLogicTable(logicTableFileName.c_str());
        exit(0);
    }
    
    if (make_dot_edd)
    {
        eddAgent->saveToDot(eddDotFileName.c_str());
        exit(0);
    }
    
    // seed the agents
    delete eddAgent;
    eddAgent = new tAgent;
    eddAgent->setupRandomAgent(10000);
    //eddAgent->loadAgent("startAgent.genome");
    
    // make mutated copies of the start genome to fill up the initial population
	for(int i = 0; i < populationSize; ++i)
    {
		eddAgents[i] = new tAgent;
		eddAgents[i]->inherit(eddAgent, 0.01, 1, false);
    }
    
	EANextGen.resize(populationSize);
    
	eddAgent->nrPointingAtMe--;
    
	cout << "setup complete" << endl;
    cout << "starting evolution" << endl;
    
    // main loop
	for (int update = 1; update <= totalGenerations; ++update)
    {
        // reset fitnesses
		for(int i = 0; i < populationSize; ++i)
        {
			eddAgents[i]->fitness = 0.0;
			//eddAgents[i]->fitnesses.clear();
		}
        
        // determine fitness of population
		eddMaxFitness = 0.0;
        double eddAvgFitness = 0.0;
        int eddMaxIndex = 0;
        
		for (int i = 0; i < populationSize; ++i)
        {
            game->executeGame(eddAgents[i], NULL, false, gridSizeX, gridSizeY, zoomingCamera, randomPlacement, noise, noiseAmount);
            
            eddAvgFitness += eddAgents[i]->classificationFitness;
            
            //eddAgents[i]->fitnesses.push_back(eddAgents[i]->fitness);
            
            if(eddAgents[i]->classificationFitness > eddMaxFitness)
            {
                eddMaxFitness = eddAgents[i]->classificationFitness;
                eddMaxIndex = i;
            }
		}
        
        eddAvgFitness /= (double)populationSize;
        
        // make a copy of the best agent
        if (bestEddAgent != NULL)
        {
            delete bestEddAgent;
        }
        bestEddAgent = new tAgent;
        bestEddAgent->inherit(eddAgents[eddMaxIndex], 0.0, update, false);
		
        if (update % 100 == 0)
        {
            cout << "gen " << update << ": edd [" << eddAvgFitness << " : " << eddMaxFitness << "] [genome: " << bestEddAgent->genome.size() << "] [gates: " << bestEddAgent->hmmus.size() << "]" << endl;
        }
        
        // display video of simulation
        if (make_interval_video)
        {
            bool finalGeneration = (update == totalGenerations);
            
            if (update % make_video_frequency == 0 || finalGeneration)
            {
                string bestString = game->executeGame(bestEddAgent, NULL, true, gridSizeX, gridSizeY, zoomingCamera, randomPlacement, noise, noiseAmount);
                
                if (finalGeneration)
                {
                    bestString.append("X");
                }
                
                //doBroadcast(bestString);
            }
        }
        
        // randomly shuffle the agents
        random_shuffle(eddAgents.begin(), eddAgents.end());
        
		for(int i = 0; i < populationSize; i += 2)
		{
            // construct swarm agent population for the next generation
			tAgent *offspring1 = new tAgent;
            tAgent *offspring2 = new tAgent;
            
            if (eddAgents[i]->fitness > eddAgents[i + 1]->fitness)
            {
                offspring1->inherit(eddAgents[i], 0.0, update, false);
                offspring2->inherit(eddAgents[i], perSiteMutationRate, update, false);
            }
            else
            {
                offspring1->inherit(eddAgents[i + 1], 0.0, update, false);
                offspring2->inherit(eddAgents[i + 1], perSiteMutationRate, update, false);
            }
			
			EANextGen[i] = offspring1;
            EANextGen[i + 1] = offspring2;
		}
        
        // shuffle the populations so there is a minimal chance of the same predator/prey combo in the next generation
        random_shuffle(EANextGen.begin(), EANextGen.end());
        
		for(int i = 0; i < populationSize; ++i)
        {
            // replace the edd agents from the previous generation
			eddAgents[i]->nrPointingAtMe--;
			if(eddAgents[i]->nrPointingAtMe == 0)
            {
				delete eddAgents[i];
            }
			eddAgents[i] = EANextGen[i];
		}
        
		eddAgents = EANextGen;
        
        if (track_best_brains && update % track_best_brains_frequency == 0)
        {
            stringstream ess;
            
            ess << eddGenomeFileName << "-gen" << update;
            
            bestEddAgent->saveGenome(ess.str().c_str());
        }
	}
	
    // save the genome file of the best agent
	bestEddAgent->saveGenome(eddGenomeFileName.c_str());
    
    // save video and quantitative stats on the best swarm agent's LOD
    vector<tAgent*> saveLOD;
    
    cout << "building ancestor list" << endl;
    
    // use 2 ancestors down from current population because that ancestor is highly likely to have high fitness
    tAgent* curAncestor = bestEddAgent;
    
    while (curAncestor != NULL)
    {
        // don't add the base ancestor
        if (curAncestor->ancestor != NULL)
        {
            saveLOD.insert(saveLOD.begin(), curAncestor);
        }
        
        curAncestor = curAncestor->ancestor;
    }
    
    FILE *LOD = fopen(LODFileName.c_str(), "w");

    fprintf(LOD, "generation,fitness\n");
    
    cout << "analyzing ancestor list" << endl;
    
    for (vector<tAgent*>::iterator it = saveLOD.begin(); it != saveLOD.end(); ++it)
    {
        // collect quantitative stats
        game->executeGame(*it, LOD, false, gridSizeX, gridSizeY, zoomingCamera, randomPlacement, noise, noiseAmount);
        
        // make video
        if (make_LOD_video)
        {
            string bestString = findBestRun(eddAgent);
            
            if ( (it + 1) == saveLOD.end() )
            {
                bestString.append("X");
            }
            
            //doBroadcast(bestString);
        }
    }
    
    fclose(LOD);
    
    return 0;
}

string findBestRun(tAgent *eddAgent)
{
    string reportString = "", bestString = "";
    double bestFitness = 0.0;
    
    for (int rep = 0; rep < 100; ++rep)
    {
        reportString = game->executeGame(eddAgent, NULL, true, gridSizeX, gridSizeY, zoomingCamera, randomPlacement, noise, noiseAmount);
        
        if (eddAgent->fitness > bestFitness)
        {
            bestString = reportString;
            bestFitness = eddAgent->fitness;
        }
    }
    
    return bestString;
}