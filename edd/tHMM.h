/*
 * tHMM.h
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

#ifndef _tHMM_h_included_
#define _tHMM_h_included_

#include <vector>
#include <deque>
#include <iostream>
#include "globalConst.h"

using namespace std;

class tHMMU{
public:
	vector<vector<unsigned char> > hmm;
	vector<unsigned int> sums;
	vector<int> ins,outs;
	unsigned char posFBNode,negFBNode;
	unsigned char nrPos,nrNeg;
	vector<int> posLevelOfFB,negLevelOfFB;
	deque<unsigned char> chosenInPos,chosenInNeg,chosenOutPos,chosenOutNeg;
	
	unsigned char _xDim,_yDim;
	tHMMU();
	~tHMMU();
	void setup(vector<unsigned char> &genome, int start);
	void setupDeterministic(vector<unsigned char> &genome, int start);
	void update(unsigned char *states,unsigned char *newStates);
	void show(void);
	
};

#endif