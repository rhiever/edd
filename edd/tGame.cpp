/*
 * tGame.cpp
 *
 * This file is part of the Evolved Digit Detector project.
 *
 * Copyright 2014 Randal S. Olson.
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

#include "tGame.h"
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>

// simulation-specific constants
#define totalStepsInSimulation      2000

tGame::tGame() { }

tGame::~tGame() { }

// runs the simulation for the given agent(s)
string tGame::executeGame(tAgent* eddAgent, FILE *dataFile, bool report, int gridSizeX, int gridSizeY, bool zoomingCamera, bool randomPlacement, bool noise, float noiseAmount)
{
    string reportString = "";
    
    // grid that the digits are placed in
    // first index is for the digit (0-9)
    // following two indeces are the X and Y positions in that digit's grid
    // (center - 2, center - 2) is the bottom-left corner of the digit
    // the digit is always 5x5
    vector<vector<vector<int>>> digitGrid;
    digitGrid.resize(10);
    
    for (int digit = 0; digit < 10; ++digit)
    {
        digitGrid[digit].resize(gridSizeX);
        for (int x = 0; x < gridSizeX; ++x)
        {
            digitGrid[digit][x].resize(gridSizeY);
        }
    }
    
    // if the digits are being randomly placed, place all 10 digits (0-9)
    // in random spots on the grid at the beginning of every simulation
    if (randomPlacement)
    {
        cerr << "random placement not yet implemented" << endl;
        exit(0);
    }
    // otherwise, place all 10 digits (0-9) centered in the grid
    else
    {
        int digitCenterX = (int)(gridSizeX / 2.0), digitCenterY = (int)(gridSizeY / 2.0);
        
        for (int digit = 0; digit < 10; ++digit)
        {
            placeDigit(digitGrid, digit, digitCenterX, digitCenterY);
        }
    }
    
    /* // visualize the digits
    for (int digit = 0; digit < 10; ++digit)
    {
        cout << digit << endl;
        
        for (int y = gridSizeY - 1; y >= 0; --y)
        {
            for (int x = 0; x < gridSizeY; ++x)
            {
                cout << digitGrid[digit][x][y] << " ";
            }
            cout << endl;
        }
        cout << "---" << endl;
    }*/
    
    // LOD data variables
    double eddFitness = 0.0;
    
    // set up brain for EDD agent
    eddAgent->setupPhenotype();
    eddAgent->fitness = 0.0;
    
    /*       BEGINNING OF SIMULATION LOOP       */
    
    for(int step = 0; step < totalStepsInSimulation; ++step)
    {
        
        /*       CREATE THE REPORT STRING FOR THE VIDEO       */
        if(report)
        {
            // report X, Y, angle of EDD agent
            /*char text[1000];
            sprintf(text,"%f,%f,%f,%d,%d,%d=", predX, predY, predA, 255, 0, 0);
            reportString.append(text);
            reportString.append("N");*/
            
        }
        /*       END OF REPORT STRING CREATION       */
        
        
        /*       SAVE DATA FOR THE LOD FILE       */
        if(dataFile != NULL)
        {
            
        }
        /*       END OF DATA GATHERING       */
        
        
        // put sensory values in edd agent's retina
        
        
        // activate the edd agent's brains
        eddAgent->updateStates();
        
        // get edd agent's action
        int action = ((eddAgent->states[(maxNodes - 1)] & 1) << 1) + (eddAgent->states[(maxNodes - 2)] & 1);
                
        switch(action)
        {
                // do nothing
            case 0:
                break;
                
                //
            case 1:
                break;
                
                //
            case 2:
                break;
                
                //
            case 3:
                break;
                
            default:
                break;
        }
    }
    /*       END OF SIMULATION LOOP       */
    
    // compute overall fitness
    eddAgent->fitness = eddFitness;
    
    if(eddAgent->fitness <= 0.0)
    {
        eddAgent->fitness = 1.0;
    }
    
    // output to data file, if provided
    if (dataFile != NULL)
    {
        fprintf(dataFile, "%d,%f\n",
                eddAgent->born,         // update born
                eddAgent->fitness       // edd agent fitness
                );
    }
    
    exit(0);
    return reportString;
}

// place the given digit on the digitGrid at the given point (digitCenterX, digitCenterY)
void tGame::placeDigit(vector<vector<vector<int>>> &digitGrid, int digit, int digitCenterX, int digitCenterY)
{
    switch (digit)
    {
        case 0:
            // ~ 0 ~
            
            // 0 1 1 1 0
            // 0 1 0 1 0
            // 0 1 0 1 0
            // 0 1 0 1 0
            // 0 1 1 1 0
            digitGrid[0][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[0][digitCenterX - 1][digitCenterY - 2] = 1;
            digitGrid[0][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[0][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[0][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[0][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[0][digitCenterX - 1][digitCenterY - 1] = 1;
            digitGrid[0][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[0][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[0][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[0][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[0][digitCenterX - 1][digitCenterY] = 1;
            digitGrid[0][digitCenterX][digitCenterY] = 0;
            digitGrid[0][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[0][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[0][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[0][digitCenterX - 1][digitCenterY + 1] = 1;
            digitGrid[0][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[0][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[0][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[0][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[0][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[0][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[0][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[0][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 1:
            // ~ 1 ~
            
            // 0 0 1 0 0
            // 0 0 1 0 0
            // 0 0 1 0 0
            // 0 0 1 0 0
            // 0 0 1 0 0
            digitGrid[1][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[1][digitCenterX - 1][digitCenterY - 2] = 0;
            digitGrid[1][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[1][digitCenterX + 1][digitCenterY - 2] = 0;
            digitGrid[1][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[1][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[1][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[1][digitCenterX][digitCenterY - 1] = 1;
            digitGrid[1][digitCenterX + 1][digitCenterY - 1] = 0;
            digitGrid[1][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[1][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[1][digitCenterX - 1][digitCenterY] = 0;
            digitGrid[1][digitCenterX][digitCenterY] = 1;
            digitGrid[1][digitCenterX + 1][digitCenterY] = 0;
            digitGrid[1][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[1][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[1][digitCenterX - 1][digitCenterY + 1] = 0;
            digitGrid[1][digitCenterX][digitCenterY + 1] = 1;
            digitGrid[1][digitCenterX + 1][digitCenterY + 1] = 0;
            digitGrid[1][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[1][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[1][digitCenterX - 1][digitCenterY + 2] = 0;
            digitGrid[1][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[1][digitCenterX + 1][digitCenterY + 2] = 0;
            digitGrid[1][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 2:
            // ~ 2 ~
            
            // 0 1 1 1 0
            // 0 0 0 1 0
            // 0 0 1 0 0
            // 0 1 0 0 0
            // 0 1 1 1 0
            digitGrid[2][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[2][digitCenterX - 1][digitCenterY - 2] = 1;
            digitGrid[2][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[2][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[2][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[2][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[2][digitCenterX - 1][digitCenterY - 1] = 1;
            digitGrid[2][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[2][digitCenterX + 1][digitCenterY - 1] = 0;
            digitGrid[2][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[2][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[2][digitCenterX - 1][digitCenterY] = 0;
            digitGrid[2][digitCenterX][digitCenterY] = 1;
            digitGrid[2][digitCenterX + 1][digitCenterY] = 0;
            digitGrid[2][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[2][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[2][digitCenterX - 1][digitCenterY + 1] = 0;
            digitGrid[2][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[2][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[2][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[2][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[2][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[2][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[2][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[2][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 3:
            // ~ 3 ~
            
            // 0 1 1 1 0
            // 0 0 0 1 0
            // 0 0 1 1 0
            // 0 0 0 1 0
            // 0 1 1 1 0
            digitGrid[3][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[3][digitCenterX - 1][digitCenterY - 2] = 1;
            digitGrid[3][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[3][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[3][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[3][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[3][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[3][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[3][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[3][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[3][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[3][digitCenterX - 1][digitCenterY] = 0;
            digitGrid[3][digitCenterX][digitCenterY] = 1;
            digitGrid[3][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[3][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[3][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[3][digitCenterX - 1][digitCenterY + 1] = 0;
            digitGrid[3][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[3][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[3][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[3][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[3][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[3][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[3][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[3][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 4:
            // ~ 4 ~
            
            // 0 1 0 1 0
            // 0 1 0 1 0
            // 0 1 1 1 0
            // 0 0 0 1 0
            // 0 0 0 1 0
            digitGrid[4][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[4][digitCenterX - 1][digitCenterY - 2] = 0;
            digitGrid[4][digitCenterX][digitCenterY - 2] = 0;
            digitGrid[4][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[4][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[4][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[4][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[4][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[4][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[4][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[4][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[4][digitCenterX - 1][digitCenterY] = 1;
            digitGrid[4][digitCenterX][digitCenterY] = 1;
            digitGrid[4][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[4][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[4][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[4][digitCenterX - 1][digitCenterY + 1] = 1;
            digitGrid[4][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[4][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[4][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[4][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[4][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[4][digitCenterX][digitCenterY + 2] = 0;
            digitGrid[4][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[4][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 5:
            // ~ 5 ~
            
            // 0 1 1 1 0
            // 0 1 0 0 0
            // 0 1 1 1 0
            // 0 0 0 1 0
            // 0 1 1 1 0
            digitGrid[5][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[5][digitCenterX - 1][digitCenterY - 2] = 1;
            digitGrid[5][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[5][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[5][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[5][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[5][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[5][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[5][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[5][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[5][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[5][digitCenterX - 1][digitCenterY] = 1;
            digitGrid[5][digitCenterX][digitCenterY] = 1;
            digitGrid[5][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[5][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[5][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[5][digitCenterX - 1][digitCenterY + 1] = 1;
            digitGrid[5][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[5][digitCenterX + 1][digitCenterY + 1] = 0;
            digitGrid[5][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[5][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[5][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[5][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[5][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[5][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 6:
            // ~ 6 ~
            
            // 0 1 0 0 0
            // 0 1 0 0 0
            // 0 1 1 1 0
            // 0 1 0 1 0
            // 0 1 1 1 0
            digitGrid[6][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[6][digitCenterX - 1][digitCenterY - 2] = 1;
            digitGrid[6][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[6][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[6][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[6][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[6][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[6][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[6][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[6][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[6][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[6][digitCenterX - 1][digitCenterY] = 1;
            digitGrid[6][digitCenterX][digitCenterY] = 1;
            digitGrid[6][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[6][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[6][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[6][digitCenterX - 1][digitCenterY + 1] = 1;
            digitGrid[6][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[6][digitCenterX + 1][digitCenterY + 1] = 0;
            digitGrid[6][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[6][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[6][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[6][digitCenterX][digitCenterY + 2] = 0;
            digitGrid[6][digitCenterX + 1][digitCenterY + 2] = 0;
            digitGrid[6][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 7:
            // ~ 7 ~
            
            // 0 1 1 1 0
            // 0 0 0 1 0
            // 0 0 0 1 0
            // 0 0 0 1 0
            // 0 0 0 1 0
            digitGrid[7][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[7][digitCenterX - 1][digitCenterY - 2] = 0;
            digitGrid[7][digitCenterX][digitCenterY - 2] = 0;
            digitGrid[7][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[7][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[7][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[7][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[7][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[7][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[7][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[7][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[7][digitCenterX - 1][digitCenterY] = 0;
            digitGrid[7][digitCenterX][digitCenterY] = 0;
            digitGrid[7][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[7][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[7][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[7][digitCenterX - 1][digitCenterY + 1] = 0;
            digitGrid[7][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[7][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[7][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[7][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[7][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[7][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[7][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[7][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 8:
            // ~ 8 ~
            
            // 0 1 1 1 0
            // 0 1 0 1 0
            // 0 1 1 1 0
            // 0 1 0 1 0
            // 0 1 1 1 0
            digitGrid[8][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[8][digitCenterX - 1][digitCenterY - 2] = 1;
            digitGrid[8][digitCenterX][digitCenterY - 2] = 1;
            digitGrid[8][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[8][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[8][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[8][digitCenterX - 1][digitCenterY - 1] = 1;
            digitGrid[8][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[8][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[8][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[8][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[8][digitCenterX - 1][digitCenterY] = 1;
            digitGrid[8][digitCenterX][digitCenterY] = 1;
            digitGrid[8][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[8][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[8][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[8][digitCenterX - 1][digitCenterY + 1] = 1;
            digitGrid[8][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[8][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[8][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[8][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[8][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[8][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[8][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[8][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        case 9:
            // ~ 9 ~
            
            // 0 1 1 1 0
            // 0 1 0 1 0
            // 0 1 1 1 0
            // 0 0 0 1 0
            // 0 0 0 1 0
            digitGrid[9][digitCenterX - 2][digitCenterY - 2] = 0;
            digitGrid[9][digitCenterX - 1][digitCenterY - 2] = 0;
            digitGrid[9][digitCenterX][digitCenterY - 2] = 0;
            digitGrid[9][digitCenterX + 1][digitCenterY - 2] = 1;
            digitGrid[9][digitCenterX + 2][digitCenterY - 2] = 0;
            
            digitGrid[9][digitCenterX - 2][digitCenterY - 1] = 0;
            digitGrid[9][digitCenterX - 1][digitCenterY - 1] = 0;
            digitGrid[9][digitCenterX][digitCenterY - 1] = 0;
            digitGrid[9][digitCenterX + 1][digitCenterY - 1] = 1;
            digitGrid[9][digitCenterX + 2][digitCenterY - 1] = 0;
            
            digitGrid[9][digitCenterX - 2][digitCenterY] = 0;
            digitGrid[9][digitCenterX - 1][digitCenterY] = 1;
            digitGrid[9][digitCenterX][digitCenterY] = 1;
            digitGrid[9][digitCenterX + 1][digitCenterY] = 1;
            digitGrid[9][digitCenterX + 2][digitCenterY] = 0;
            
            digitGrid[9][digitCenterX - 2][digitCenterY + 1] = 0;
            digitGrid[9][digitCenterX - 1][digitCenterY + 1] = 1;
            digitGrid[9][digitCenterX][digitCenterY + 1] = 0;
            digitGrid[9][digitCenterX + 1][digitCenterY + 1] = 1;
            digitGrid[9][digitCenterX + 2][digitCenterY + 1] = 0;
            
            digitGrid[9][digitCenterX - 2][digitCenterY + 2] = 0;
            digitGrid[9][digitCenterX - 1][digitCenterY + 2] = 1;
            digitGrid[9][digitCenterX][digitCenterY + 2] = 1;
            digitGrid[9][digitCenterX + 1][digitCenterY + 2] = 1;
            digitGrid[9][digitCenterX + 2][digitCenterY + 2] = 0;
            break;
            
        default:
            cerr << "invalid digit to place: " << digit << endl;
            break;
    }
}

// sums a vector of values
double tGame::sum(vector<double> values)
{
    double sum = 0.0;
    
    for (vector<double>::iterator i = values.begin(); i != values.end(); ++i)
    {
        sum += *i;
    }
    
    return sum;
}

// averages a vector of values
double tGame::average(vector<double> values)
{
    return sum(values) / (double)values.size();
}

// computes the variance of a vector of values
double tGame::variance(vector<double> values)
{
    double sumSqDist = 0.0;
    double mean = average(values);
    
    for (vector<double>::iterator i = values.begin(); i != values.end(); ++i)
    {
        sumSqDist += pow( *i - mean, 2.0 );
    }
    
    return sumSqDist /= (double)values.size();
}
