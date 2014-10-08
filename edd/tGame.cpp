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
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

// simulation-specific constants
#define totalStepsInSimulation      20
#define MAX_CAM_SIZE                3

// each sensor's (x, y) offset from the center of the camera
map< int, vector<int> > sensorOffsetMap;

map< string, vector< vector<int> > > symbols;

tGame::tGame()
{
    // pre-compute the sensor offsets
    // to maintain the same order of inputs, start counting sensors from the inside.
    // e.g. for 7x7:
    
    // 43 42 41 40 39 38 37
    // 44 21 20 19 18 17 36
    // 45 22 7  6  5  16 35
    // 46 23 8  0  4  15 34
    // 47 24 1  2  3  14 33
    // 48 9  10 11 12 13 32
    // 25 26 27 28 29 30 31
    
    int offsetX = 0, offsetY = 0;
    int offsetAmount = 0;
    
    for (int sensor = 0; sensor < 111 * 111; ++sensor)
    {
        int root = sqrt(sensor);
        if (root % 2 == 1 && root * root == sensor)
        {
            ++offsetAmount;
            offsetX = -offsetAmount;
            offsetY = -offsetAmount;
        }
        else if (offsetX != offsetAmount && offsetY == -offsetAmount)
        {
            ++offsetX;
        }
        else if (offsetX == offsetAmount && offsetY != offsetAmount)
        {
            ++offsetY;
        }
        else if (offsetX != -offsetAmount && offsetY == offsetAmount)
        {
            --offsetX;
        }
        else if (offsetX == -offsetAmount && offsetY != -offsetAmount)
        {
            --offsetY;
        }
        
        vector<int> offsets;
        offsets.push_back(offsetX);
        offsets.push_back(offsetY);
        sensorOffsetMap[sensor] = offsets;
    }
    
    ifstream symbolFile ("symbols.txt");
    string line;
    string key = "";
    vector< vector<int> > symbol;
    int symbolRow = 0;

    while ( getline (symbolFile, line) )
      {
	if (line.find("-") != string::npos)
	  {
	    key = line;
	    symbol.clear();
	    symbolRow = 0;
	  }
	
	else if (line.find("0") != string::npos || line.find("1") != string::npos)
	  {
	    symbol.resize(symbol.size() + 1);
	    
	    for (int i = 0; i < line.length(); ++i)
	      {
		if (line.substr(i, 1) != " ")
		  {
		    symbol[symbolRow].push_back(atoi(line.substr(i, 1).c_str()));
		  }
	      }
	    
	    ++symbolRow;
	  }

	else
	  {
	    symbols[key] = symbol;
	  }
      }

    symbolFile.close();
}

tGame::~tGame() { }

// runs the simulation for the given agent(s)
string tGame::executeGame(tAgent* eddAgent, FILE *dataFile, bool report, int gridSizeX, int gridSizeY, bool zoomingCamera, bool randomPlacement, bool randomStart, bool noise, float noiseAmount, bool digitRotation)
{
    stringstream reportString;
    
    // grid that the digits are placed in
    // first index is for the digit (0-9)
    // following two indeces are the X and Y positions in that digit's grid
    // (center - 2, center - 2) is the bottom-left corner of the digit
    // the digit is always 5x5
    vector< vector< vector<int> > > digitGrid;
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
    // otherwise, place all 10 digits (0-9) centered in the grid
    int digitCentersX[10], digitCentersY[10];
    int digitRotated[10];
    
    for (int digit = 0; digit < 10; ++digit)
    {
        int digitCenterX = (int)(gridSizeX / 2.0), digitCenterY = (int)(gridSizeY / 2.0);
        
        if (randomPlacement)
        {
            bool validPlacement = false;
            
            while (!validPlacement)
            {
                digitCenterX = randDouble * gridSizeX;
                digitCenterY = randDouble * gridSizeY;
                
                if (digitCenterX - 2 >= 0 && digitCenterX + 2 < gridSizeX &&
                    digitCenterY - 2 >= 0 && digitCenterY + 2 < gridSizeY)
                {
                    validPlacement = true;
                }
            }

        }

	digitRotated[digit] = 0;

	if (digitRotation)
	  {
	    do
	      {
		digitRotated[digit] = (int)(randDouble * 4.0);
	      } while (digitRotated[digit] == 4);
	  }
	
        placeDigit(digitGrid, digit, digitRotated[digit], digitCenterX, digitCenterY);
        
        digitCentersX[digit] = digitCenterX;
        digitCentersY[digit] = digitCenterY;
    }

    // visualize the digits
    /*for (int digit = 0; digit < 10; ++digit)
      {
	cout << digit << endl;
	
	for (int x = 0; x < gridSizeX; ++x)
	  {
	    for (int y = 0; y < gridSizeY; ++y)
	      {
		cout << digitGrid[digit][x][y] << " ";
	      }
	    cout << endl;
	  }
	cout << "---" << endl;
      }*/

    // set up brain for EDD agent
    eddAgent->setupPhenotype();
    eddAgent->classificationFitness = 0.0;
    eddAgent->fitness = 0.0;
    
    // edd agent camera variables
    int cameraX = gridSizeX / 2.0, cameraY = gridSizeY / 2.0;
    int cameraSize = 3;
    
    for (int digit = 0; digit < 10; ++digit)
    {
        eddAgent->truePositives[digit] = 0;
        eddAgent->falsePositives[digit] = 0;
        eddAgent->trueNegatives[digit] = 0;
        eddAgent->falseNegatives[digit] = 0;
    }
    
    
    /*       BEGINNING OF SIMULATION LOOP       */
    
    // test the edd agent on all 10 digits (0-9)
    vector<int> digits;
    for (int digit = 0; digit < 10; ++digit)
    {
        digits.push_back(digit);
    }
    random_shuffle(digits.begin(), digits.end());
    
    for (int counter = 0; counter < digits.size(); ++counter)
    {
        int digit = digits[counter];
        
        eddAgent->resetBrain();
        cameraX = gridSizeX / 2.0;
        cameraY = gridSizeY / 2.0;
        cameraSize = 3;

	if (randomStart)
	  {
	    do
	      {
		cameraX = (int)(randDouble * gridSizeX);
		cameraY = (int)(randDouble * gridSizeY);
	      } while (cameraX == gridSizeX || cameraY == gridSizeY);
	  }
        
        if (report)
        {
	  reportString << digit << "-" << digitRotated[digit] << "," << digitCentersX[digit] << "," << digitCentersY[digit] << "," << gridSizeX << "," << gridSizeY << "\n";
        }
        
        for (int step = 0; step < totalStepsInSimulation; ++step)
        {
            
            /*       CREATE THE REPORT STRING FOR THE VIDEO       */
            if (report)
            {
	      reportString << cameraX << "," << cameraY << "," << cameraSize;
            }
            /*       END OF REPORT STRING CREATION       */
            
            
            /*       SAVE DATA FOR THE LOD FILE       */
            if (dataFile != NULL)
            {
                
            }
            /*       END OF DATA GATHERING       */
            
            // clear all sensors
            for (int sensor = 0; sensor < (MAX_CAM_SIZE * MAX_CAM_SIZE) + 4; ++sensor)
            {
                eddAgent->states[sensor] = 0;
            }
            
            // put sensory values in edd agent's retina
            // by default, edd agent has 3x3 retina:
            
            // x x x
            // x x x
            // x x x
            
            // can zoom out to 5x5, 7x7, etc.
            
            // to maintain same order of inputs, start counting sensors from the inside.
            // e.g. for 7x7:
            
            // 43 42 41 40 39 38 37
            // 44 21 20 19 18 17 36
            // 45 22 7  6  5  16 35
            // 46 23 8  0  4  15 34
            // 47 24 1  2  3  14 33
            // 48 9  10 11 12 13 32
            // 25 26 27 28 29 30 31
            
            for (int sensor = 0; sensor < cameraSize * cameraSize; ++sensor)
            {
                int sensorX = cameraX + sensorOffsetMap[sensor][0];
                int sensorY = cameraY + sensorOffsetMap[sensor][1];
                
                if (sensorX >= 0 && sensorX < gridSizeX && sensorY >= 0 && sensorY < gridSizeY)
                {
                    if (digitGrid[digit][sensorX][sensorY] == 1)
                    {
                        eddAgent->states[sensor] = 1;
                    }
                }
            }

	    // "periphery" sensors
            // these are 4 sensors that tell the agent its relative position to the digit

            // "up" sensor
            if (digitCentersY[digit] - cameraY > 0)
              {
		eddAgent->states[9] = 1;
              }

            // "down" sensor
            else if (digitCentersY[digit] - cameraY < 0)
              {
                eddAgent->states[10] = 1;
              }

            // "left" sensor
            if (digitCentersX[digit] - cameraX < 0)
              {
                eddAgent->states[11] = 1;
              }

	    // "right" sensor
            else if(digitCentersX[digit] - cameraX > 0)
              {
                eddAgent->states[12] = 1;
              }
            
            // activate the edd agent's brain
            for (int updateCounter = 0; updateCounter < 1; ++updateCounter)
            {
                eddAgent->updateStates();
            }
            
            // get edd agent's action
            // possible actions:
            //      move up/down: 2
            //      move left/right: 2
            //      zoom in: 1
            //      zoom out: 1
            //      classify (0-9): 10
            //      veto bits (0-9): 10
            //      TODO: "I'm ready" bit: 1
            
            int moveUp = eddAgent->states[(maxNodes - 1)] & 1;
            int moveDown = eddAgent->states[(maxNodes - 2)] & 1;
            int moveLeft = eddAgent->states[(maxNodes - 3)] & 1;
            int moveRight = eddAgent->states[(maxNodes - 4)] & 1;
            int zoomIn = eddAgent->states[(maxNodes - 5)] & 1;
            int zoomOut = eddAgent->states[(maxNodes - 6)] & 1;
            
            // edd agent can move the camera
            // possible for up/down and left/right actuators to cancel each other out
            if (zoomingCamera && moveUp) cameraY += 1;
            if (zoomingCamera && moveDown) cameraY -= 1;
            if (zoomingCamera && moveRight) cameraX += 1;
            if (zoomingCamera && moveLeft) cameraX -= 1;

	    if (report)
	      {
		// parse edd agent classifications
		int classifyDigit[10];
		for (int i = 0; i < 10; ++i)
		  {
		    classifyDigit[i] = eddAgent->states[(maxNodes - 7 - i)] & 1;
		  }

		int vetoBits[10];
		for (int i = 0; i < 10; ++i)
		  {
		    vetoBits[i] = eddAgent->states[(maxNodes - 17 - i)] & 1;
		  }
		for (int i = 0; i < 10; ++i)
                  {
                    if (classifyDigit[i] == 1 && vetoBits[i] == 0)
                      {
                        reportString << "," << i;
                      }
                  }
		reportString << "\n";
	      }
	}
        
        if (report)
        {
            reportString << "X\n";
        }
        
        // parse edd agent classifications
        int classifyDigit[10];
        for (int i = 0; i < 10; ++i)
        {
            classifyDigit[i] = eddAgent->states[(maxNodes - 7 - i)] & 1;
        }
        
        int vetoBits[10];
        for (int i = 0; i < 10; ++i)
        {
            vetoBits[i] = eddAgent->states[(maxNodes - 17 - i)] & 1;
        }
        
        // check accuracy of edd agent classifications
        float score = 0.0;
        float numDigitsGuessed = 0.0;
        
        for (int i = 0; i < 10; ++i)
        {
            bool guessedThisDigit = (classifyDigit[i] == 1 && vetoBits[i] == 0);
            
            if (guessedThisDigit)
            {
                numDigitsGuessed += 1.0;
            }
            
            if (guessedThisDigit && i == digit)
            {
                // true positive
                eddAgent->truePositives[i] += 1;
                score = 1.0;
            }
            
            else if (guessedThisDigit && i != digit)
            {
                // false positive
                eddAgent->falsePositives[i] += 1;
            }
            
            else if (!guessedThisDigit && i == digit)
            {
                // false negative
                eddAgent->falseNegatives[i] += 1;
            }
            
            else if (!guessedThisDigit && i != digit)
            {
                // true negative
                eddAgent->trueNegatives[i] += 1;
            }
        }
        
        if (numDigitsGuessed > 0.0)
        {
            eddAgent->classificationFitness += score / numDigitsGuessed;
        }
    }
    
    /*       END OF SIMULATION LOOP       */
    
    // compute TPR and TNR
    for (int digit = 0; digit < 10; ++digit)
    {
        eddAgent->truePositiveRate[digit] = eddAgent->truePositives[digit] / (eddAgent->truePositives[digit] + eddAgent->falseNegatives[digit]);
        
        eddAgent->trueNegativeRate[digit] = eddAgent->trueNegatives[digit] / (eddAgent->trueNegatives[digit] + eddAgent->falsePositives[digit]);
    }
    
    // compute overall fitness
    eddAgent->fitness = eddAgent->classificationFitness / 10.0;
    eddAgent->classificationFitness = eddAgent->classificationFitness / 10.0;
    
    // don't allow fitness to be 0 nor negative
    if (eddAgent->fitness <= 0.0)
    {
        eddAgent->fitness = 0.000001;
    }
    
    // output to data file, if provided
    if (dataFile != NULL)
    {
        fprintf(dataFile, "%d,%f\n",
                eddAgent->born,         // update born
                eddAgent->fitness       // edd agent fitness
                );
    }

    //cout << eddAgent->fitness << endl;

    return reportString.str();
}

// place the given digit on the digitGrid at the given point (digitCenterX, digitCenterY)
void tGame::placeDigit(vector< vector< vector<int> > > &digitGrid, int digit, int rotation, int digitCenterX, int digitCenterY)
{
    for (int i = 0; i < digitGrid[digit].size(); ++i)
    {
        for (int j = 0; j < digitGrid[digit][i].size(); ++j)
        {
            digitGrid[digit][i][j] = 0;
        }
    }

    stringstream key_ss;
    key_ss << digit << "-" << rotation;
    string key = key_ss.str();

    for (int i = 0; i < symbols[key].size(); ++i)
      {
	for (int j = 0; j < symbols[key][i].size(); ++j)
	  {
	    digitGrid[digit][digitCenterX - 2 + i][digitCenterY - 2 + j] = symbols[key][i][j];
	  }
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
