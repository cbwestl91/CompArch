#include "elem.h"
#include "interface.hh"
#include "balancing.h"

#include <algorithm>
#include <vector>

using namespace std;

Elem* findElem(vector<Elem*> v, long delta)
{
	//typedef Intcontainer::iterator IntIterator;

	//IntContainer i = find(vector.begin(), vector.end(), delta); // Try to find the wanted element

	for(vector<Elem*>::iterator i = v.begin(); i != v.end(); ++i)
	{
		if(i->delta == delta)
		{
			return *i; // Found it
		}
		
	}
		return NULL;	// Didn't exist

/*	if(i != vector.end())
	{
		return *i;	// We found the element
	}
	else
	{
		return NULL;	// Didn't exist
	}
*/
}

void XElem::applyActualResult(int d3)
{
	if (d3 == 0)//ignore prefetch
		return;
	else if (candidate == d3)
		score += HIT_SCORE_BOOST;
	else
	{
		//change candidate:
		if (score <= KICK_THRESHOLD)
		{
			candidate = d3;
			score = START_SCORE;
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
	bool notFound = false;
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
		xE->applyActualResult(d3);

	mFetches++;
}

int ElemManager::getDelta(int d1, int d2)
{
	YElem* yE = (YElem*) findElem(elements, d1);
	XElem* xE = (XElem*) findElem(yE->elements, d2);
	
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
	
	delta = address - previous;
	previous = address;
	return address + nextFetchDelta;
}

XElem* ElemManager::addCombination(int d1, int d2)
{
	//we already know that at least one of the elements are non-existing.
	YElem* yE = findElem(elements, d1);
	XElem* xE = new XElem(d2);
	
	if (yE == NULL)
	{
		yE = new YElem();
		elements.push_back(yE);//TODO: dont push back, insert at correct position.
	}
	
	yE->elements.push_back(xE);//TODO: again with the push back.
	
	return xE;
}
