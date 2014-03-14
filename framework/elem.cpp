#include "elem.h"
#include "interface.hh"


#include <algorithm>
#include <vector>

using namespace std;

Elem* findElem(std::vector<Elem*>, long delta)
{
	typedef Intcontainer::iterator IntIterator;

	IntContainer i = binary_search(vector.begin(), vector.end(), delta); // Try to find the wanted element

	if(i != vector.end())
	{
			// We found the element
	}
	else
	{
			// Didn't exist
	}

	return NULL;
}

void XElem::applyActualResult(int d3)
{
	if (candidate == d3)
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
			
		if ((yE == NULL || xE == NULL) && d_odd != 0 && d_even != 0)
			addCombination(d_odd, d_even);
		else
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
				addCombination(d_even, d_odd);
		}
		else
			d_even = d3;
	}
	

	//check if we hit the candidate:
	xE->applyActualResult(d3);

	mFetches++;
}

int ElemManager::getDelta(int d1, int d2)
{
	YElem* yE = (YElem*) findElem(elements, d1);
	XElem* xE = (XElem*) findElem(yE->elements, d2);
	
	return xE->getNext();
}

void ElemManager::addCombination(int d1, int d2)
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
}
