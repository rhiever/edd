/*
 * tHMM.cpp
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

#include <stdlib.h>
#include "tHMM.h"
//#define feedbackON

tHMMU::tHMMU(){
}

tHMMU::~tHMMU(){
	hmm.clear();
	sums.clear();
	ins.clear();
	outs.clear();
	posLevelOfFB.clear();
	negLevelOfFB.clear();
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
}

// set up stochastic gate
void tHMMU::setup(vector<unsigned char> &genome, int start){
	int i,j,k;
	ins.clear();
	outs.clear();
	k=(start+2)%(int)genome.size();

	_xDim=1+(genome[(k++)%genome.size()]&3);
	_yDim=1+(genome[(k++)%genome.size()]&3);
	posFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	negFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	nrPos=genome[(k++)%genome.size()]&3;
	nrNeg=genome[(k++)%genome.size()]&3;
	//cout<<"setup "<<(int)genome[start+2]<<" "<<(int)xDim<<" "<<(int)yDim<<endl;
	ins.resize(_yDim);
	outs.resize(_xDim);
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=(int)(1+genome[(k+8+i)%genome.size()]);
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=(int)(1+genome[(k+12+i)%genome.size()]);
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
	
	k=k+16;
	hmm.resize(1<<_yDim);
	sums.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		hmm[i].resize(1<<_xDim);
		for(j=0;j<(1<<_xDim);j++){
//			hmm[i][j]=(genome[(k+j+((1<<yDim)*i))%genome.size()]&1)*255;
			hmm[i][j]=genome[(k+j+((1<<_xDim)*i))%genome.size()];
			if(hmm[i][j]==0) hmm[i][j]=1;
			sums[i]+=hmm[i][j];
		}
	}
}

// set up deterministic gate
void tHMMU::setupDeterministic(vector<unsigned char> &genome, int start){
	int i,j,k;
	ins.clear();
	outs.clear();
	k=(start+2)%(int)genome.size();
	
	_xDim=1+(genome[(k++)%genome.size()]&3);
	_yDim=1+(genome[(k++)%genome.size()]&3);
	posFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	negFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	nrPos=genome[(k++)%genome.size()]&3;
	nrNeg=genome[(k++)%genome.size()]&3;
	//cout<<"setup "<<(int)genome[start+2]<<" "<<(int)xDim<<" "<<(int)yDim<<endl;
	ins.resize(_yDim);
	outs.resize(_xDim);
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=(int)(1+genome[(k+8+i)%genome.size()]);
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=(int)(1+genome[(k+12+i)%genome.size()]);
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
	
	k=k+16;
	hmm.resize(1<<_yDim);
	sums.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++)
    {
		hmm[i].resize(1<<_xDim);
        int largestValueInRow = 0, largestValueInRowIndex = 0;
        
		for(j=0;j<(1<<_xDim);j++)
        {
			hmm[i][j]=0;
            
            if (genome[(k + j + ((1 << _xDim) * i)) % genome.size()] > largestValueInRow)
            {
                largestValueInRow = genome[(k+j+((1<<_xDim)*i))%genome.size()];
                largestValueInRowIndex = j;
            }
        }
        
		hmm[i][largestValueInRowIndex] = 255;
		sums[i] = 255;
	}
	
}

void tHMMU::update(unsigned char *states, unsigned char *newStates)
{
	int I=0;
	int i,j,r;
#ifdef feedbackON
    unsigned char mod;
    
	if((nrPos!=0)&&(states[posFBNode]==1))
    {
		for(i=0;i<chosenInPos.size();i++)
        {
			mod=(unsigned char)(rand()%(int)posLevelOfFB[i]);
			if((hmm[chosenInPos[i]][chosenOutPos[i]]+mod)<255)
            {
				hmm[chosenInPos[i]][chosenOutPos[i]]+=mod;
				sums[chosenInPos[i]]+=mod;
			}
		}
	}
	if((nrNeg!=0)&&(states[negFBNode]==1))
    {
		for(i=0;i<chosenInNeg.size();i++)
        {
			mod=(unsigned char)(rand()%(int)negLevelOfFB[i]);
			if((hmm[chosenInNeg[i]][chosenOutNeg[i]]-mod)>0)
            {
				hmm[chosenInNeg[i]][chosenOutNeg[i]]-=mod;
				sums[chosenInNeg[i]]-=mod;
			}
		}
	}
#endif
    
	for(vector<int>::iterator it = ins.begin(), end = ins.end(); it != end; ++it)
    {
		I=(I<<1)+((states[*it])&1);
    }
    
	r=1+(rand()%(sums[I]-1));
	j=0;
    //	cout<<I<<" "<<(int)hmm.size()<<" "<<(int)hmm[0].size()<<endl;
	while(r > hmm[I][j])
    {
		r -= hmm[I][j];
		++j;
	}
    
	for(i = 0; i < outs.size(); ++i)
    {
		newStates[outs[i]] |= (j >> i) & 1;
    }
#ifdef feedbackON
	chosenInPos.push_back(I);
	chosenInNeg.push_back(I);
	chosenOutPos.push_back(j);
	chosenOutNeg.push_back(j);
	while(chosenInPos.size()>nrPos)
    {
        chosenInPos.pop_front();
    }
	while(chosenOutPos.size()>nrPos)
    {
        chosenOutPos.pop_front();
    }
	while(chosenInNeg.size()>nrNeg)
    {
        chosenInNeg.pop_front();
    }
	while(chosenOutNeg.size()>nrNeg)
    {
        chosenOutNeg.pop_front();
    }
#endif
}

void tHMMU::show(void){
	int i,j;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)ins[i]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)outs[i]<<" ";
	cout<<endl;
	for(i=0;i<hmm.size();i++){
		for(j=0;j<hmm[i].size();j++)
			cout<<" "<<(double)hmm[i][j]/sums[i];
		cout<<endl;
	}
	cout<<endl;
	cout<<"posFB: "<<(int)posFBNode<<" negFB: "<<(int)negFBNode<<endl;
	cout<<"posQue:"<<endl;
	for(i=0;i<posLevelOfFB.size();i++)
		cout<<(int)posLevelOfFB[i]<<" ";
	cout<<endl;
	cout<<"negQue:"<<endl;
	for(i=0;i<negLevelOfFB.size();i++)
		cout<<(int)negLevelOfFB[i]<<" ";
	cout<<endl;
/*
	for(i=0;i<hmm.size();i++){
		for(j=0;j<hmm[i].size();j++)
			cout<<(int)hmm[i][j]<<" ";
		cout<<endl;
	}
	*/
//	cout<<"------"<<endl;
}
