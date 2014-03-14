#include "elem.hh"
#include "interface.hh"
#include "balancing.hh"
#include <algorithm>
#include <stdio.h>
#include <vector>

using namespace std;

XElem* ElemManager::findElem(vector<XElem*> v, int delta)
{
	for(vector<XElem*>::iterator i = v.begin(); i != v.end(); ++i)
	{
		if((*i)->delta == delta)
			return *i; // Found it		
	}
	return NULL;	// Didn't exist
}

YElem* ElemManager::findElem(vector<YElem*> v, int delta)
{
	for(vector<YElem*>::iterator i = v.begin(); i != v.end(); ++i)
	{
		if((*i)->delta == delta)
			return *i; // Found it		
	}
	return NULL;	// Didn't exist
}

void XElem::applyActualResult(int d3, unsigned int totalFetches)
{
	if (d3 == 0)//ignore prefetch
		return;
	else if (candidate == d3)
	{
		if (score < totalFetches + START_SCORE)
			score = totalFetches + START_SCORE;
		else
			score += HIT_SCORE_BOOST;
		
	}
	else
	{
		//change candidate:
		if (score <= KICK_THRESHOLD + totalFetches)
		{
			candidate = d3;
			score = START_SCORE + totalFetches;
		}
		else
			score -= MISS_SCORE_PUNISHMENT;
	}		
}


ElemManager::ElemManager()
{
	mFetches = 0;
}

void ElemManager::previousActualCandidate(int d3)
{
	static int d_odd = 0, d_even = 0;
	XElem* xE = NULL;
	YElem* yE = NULL;
	if (mFetches % 2)
	{
		yE = findElem(elements, d_odd);
		if (yE != NULL) 
			xE = findElem(yE->elements, d_even);
			
		if ((yE == NULL || xE == NULL))
		{
			if (d_odd != 0 && d_even != 0)
				xE = addCombination(d_odd, d_even);
		}
		
		d_odd = d3;
	}
	else
	{
		yE = findElem(elements, d_even);
		if (yE != NULL) 
			xE = findElem(yE->elements, d_odd);
			
		if ((yE == NULL || xE == NULL))
		{
			if (d_odd != 0 && d_even != 0)
				xE = addCombination(d_even, d_odd);
		}
		
		d_even = d3;
	}


	//check if we hit the candidate:
	if (xE != NULL)
		xE->applyActualResult(d3, mFetches);

	mFetches++;
}

int ElemManager::getDelta(int d1, int d2)
{
	XElem* xE = NULL;
	YElem* yE = findElem(elements, d1);
	
	if (yE != NULL)
		xE = findElem(yE->elements, d2);
	
	if (xE != NULL)
		return xE->getNext();
	else
		return 0;
}

unsigned int ElemManager::findNextFetch(unsigned int address)
{
	static int previous = 0;
	static int delta = 0;

	int nextFetchDelta = 0;

	if (previous != 0)
	{
		previousActualCandidate(address - previous);
		nextFetchDelta = getDelta(delta, address - previous);	
	}
	if (nextFetchDelta)
		cout << "attempted fetch " << (address + nextFetchDelta) << ", delta: " << nextFetchDelta << endl;
	if (mFetches % 1000)
		cout << "------------------------------------------" << mFetches << "------------------------------------" << endl;
	delta = address - previous;
	previous = address;
	return address + nextFetchDelta;
}

XElem* ElemManager::addCombination(int d1, int d2)
{
	if (elemCount >= MAX_ELEMS)
	{
		YElem* yE = NULL;
		XElem* xE = NULL;
		int minScore = 100000;
		int saved_j = 0;
		for (int i = 0; i < elements.size(); i++)
		{
			bool foundNewCandidate = false;
			for (int j = 0; j < elements[i]->elements.size(); j++)
			{
				if (elements[i]->elements[j]->getScore() < minScore)
				{
					xE = elements[i]->elements[j];
					minScore = xE;
					foundNewCandidate = true;
					saved_j = j;
				}
			}
			if (foundNewCandidate)
				yE = elements[i];
		}	
		//remove element if its score is under the threshold:
		if (xE->getScore() < KICK_ELEM_THRESHOLD + mFetches)
		{
			yE->elements.erase(saved_j);
			delete xE;
			if (yE->elements.size() == 0)
				delete yE;		
			elemCount--;
		}
	}
	//need to recheck the elem count
	if (elemCount < MAX_ELEMS)
	{
		//we already know that at least one of the elements are non-existing.
		YElem* yE = findElem(elements, d1);
		XElem* xE = new XElem(d2);

		if (yE == NULL)
		{
			yE = new YElem(d1);

			elements.push_back(yE);//TODO: dont push back, insert at correct position.
		}

		yE->elements.push_back(xE);//TODO: again with the push back.

		elemCount++;

		return xE;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

ElemManager* manager;

void prefetch_access(AccessStat stat)
{
	int nextFetch =  manager->findNextFetch((int)stat.mem_addr);
	if(!in_cache((uint64_t)nextFetch))
	{

	  	issue_prefetch((uint64_t)nextFetch);
	} 
}

void prefetch_complete(Addr addr)
{
}

void prefetch_init(void)
{
	manager = new ElemManager();
}
