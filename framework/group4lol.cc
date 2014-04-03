#include "interface.hh"
#include <time.h>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <deque>

using namespace std;

#define OPTIMIZE 1
#define MULTIFETCH 1
#define OLD_SCHOOL 1

#define START_SCORE 10000
#define KICK_THRESHOLD 0
#define HIT_SCORE_BOOST 500
#define MISS_SCORE_PUNISHMENT 2000
#define KICK_ELEM_THRESHOLD 0

#define MAX_ELEMS 800


#define LOOK_AHEAD_COUNT 2


class Elem 
{
public:
	int delta;
};


//search algorithm


class XElem: public Elem
{
public:
	
	inline int getNext() { return candidate; };
	
	XElem(int delta, unsigned int totalFetches) 
	{ 
		this->delta = delta; 
		candidate = 0;
		score = totalFetches + START_SCORE;
	}
	/**
	* apply actual result, and update score. Change candidate if score == 0
	*/
	void applyActualResult(int d3, unsigned int totalFetches);
	
	uint8_t getScore() { return score; }
private:
	int candidate;
	uint8_t score;
};


class YElem : public Elem
{
public:
	YElem(int d) 
	{ 
		delta = d; 
	}
	std::deque<XElem*> elements;
};


class ElemManager
{
public:
	/**
	tell the element the actual delta it got
	*/
	void previousActualCandidate(int d3);

	/**
	return 0 if not found, and add combination if more room
	*/
	int getDelta(int d1, int d2);
	
	ElemManager();
	
	unsigned int findNextFetch(unsigned int address);
	

	
	/**
	used by getDelta to add new combo
	*/
	XElem* addCombination(int d1, int d2);
	

	uint16_t elemCount;
	std::deque<YElem*> elements;

private:
	unsigned int mFetches;
};

//helpers:
int findElem(std::deque<XElem*>&, int);
int findElem(std::deque<YElem*>&, int);
XElem* findPtr(std::deque<XElem*>& v, int delta);
YElem* findPtr(std::deque<YElem*>& v, int delta);

/********************************************************************************************
	START OF ELEM.CC
********************************************************************************************/

void insert(deque<XElem*> &vec, XElem* n)
{
	int index = findElem(vec, n->delta);
	vec.insert(vec.begin() + index, n);
}


void insert(deque<YElem*> &vec, YElem* n)
{
	int index = findElem(vec, n->delta);
	vec.insert(vec.begin() + index, n);
}



int findElem(deque<XElem*>& v, int delta)
{
#if OPTIMIZE

	int mid = 0, min = 0, max = v.size() - 1;
 	while (max >= min)
	{
		mid = min + (max - min) / 2;
		if(v[mid]->delta == delta)
			return mid; 
		else if (v[mid]->delta < delta)
			min = mid + 1;
		else         
			max = mid - 1;
	}
	if(min > max)
	{
		return min;
	}
  	return mid;
#else
	for(int i = 0; i < v.size(); i++)
	{
		if(v[i]->delta == delta)
			return i; // Found it		
	}
	return 0;	// Didn't exist
	
#endif
}

int findElem(deque<YElem*>& v, int delta)
{

#if OPTIMIZE

	int mid = 0, min = 0, max = v.size() - 1;
 	while (max >= min)
	{
		mid = min + (max - min) / 2;
		if(v[mid]->delta == delta)
			return mid; 
		else if (v[mid]->delta < delta)
			min = mid + 1;
		else         
			max = mid - 1;
	}

	if(min > max)	
	{
		return min;
	}

  	return mid;
#else

	for(int i = 0; i < v.size(); i++)
	{
		if(v[i]->delta == delta)
			return i; // Found it		
	}
	return -1;	// Didn't exist

#endif
}

XElem* findPtr(deque<XElem*>& v, int delta)
{
	int pos = findElem(v, delta);
	if (pos < v.size() && pos >= 0 && v[pos]->delta == delta)
		return v[pos];
	else 
		return NULL;
}

YElem* findPtr(deque<YElem*>& v, int delta)
{
	int pos = findElem(v, delta);
	if (pos < v.size() && pos >= 0 && v[pos]->delta == delta)
		return v[pos];
	else 
		return NULL;
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
	elemCount = 0;
}

void ElemManager::previousActualCandidate(int d3)
{
	static int d_odd = 0, d_even = 0;
	XElem* xE = NULL;
	YElem* yE = NULL;
	if (mFetches % 2)
	{
		yE = findPtr(elements, d_odd);
		if (yE != NULL) 
			xE = findPtr(yE->elements, d_even);
		
		if (yE == NULL || xE == NULL)
		{
			if (d_odd != 0 && d_even != 0)
				xE = addCombination(d_odd, d_even);
		}

		d_odd = d3;
	}
	else
	{
		yE = findPtr(elements, d_even);
		if (yE != NULL) 
			xE = findPtr(yE->elements, d_odd);
			
		
		if (yE == NULL || xE == NULL)
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
	YElem* yE = findPtr(elements, d1);
	
	if (yE != NULL)
		xE = findPtr(yE->elements, d2);
	
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
	if (elemCount >= MAX_ELEMS)
	{
		YElem* yE = NULL;
		XElem* xE = NULL;
		int minScore = std::numeric_limits<int>::max();
		int saved_j = 0, saved_i = 0;
		
		for (int i = 0; i < elements.size(); i++)
		{
			for (int j = 0; j < elements[i]->elements.size(); j++)
			{
				if (elements[i]->elements[j]->getScore() <= minScore)
				{
					xE = elements[i]->elements[j];
					yE = elements[i];
					minScore = xE->getScore();
					saved_i = i;
					saved_j = j;
				}
			}
		}	
		//remove that shit
		if (xE != NULL && yE != NULL)
		{
			yE->elements.erase(yE->elements.begin() + saved_j);//remove xE from list
			delete xE;
			
			if (yE->elements.size() == 0)
			{
				elements.erase(elements.begin() + saved_i);
				delete yE;
			}
			elemCount--;
		}
				
				
	}
	//need to recheck the elem count
	if (elemCount < MAX_ELEMS)
	{
		YElem* yE = findPtr(elements, d1);
		
		if (yE == NULL)
		{
			yE = new YElem(d1);
			insert(elements, yE);
		}
		XElem* xE = new XElem(d2, mFetches);
	
		insert(yE->elements, xE);
		elemCount++;		
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

ElemManager* manager;

void prefetch_access(AccessStat stat)
{
	Addr nextFetch = (Addr) manager->findNextFetch((int)stat.mem_addr);
/*
	if (stat.miss)
		cout << "miss:  " << stat.mem_addr << "\t" << stat.time << endl;
	else
		cout << "hit:   " << stat.mem_addr << endl;
*/
	if(nextFetch < MAX_PHYS_MEM_ADDR && !in_cache(nextFetch) && nextFetch != stat.mem_addr)
	{
		set_prefetch_bit(nextFetch);
	  	issue_prefetch(nextFetch);
		
		
#if MULTI_FETCH
		set_prefetch_bit(nextFetch - BLOCK_SIZE);
	  	issue_prefetch(nextFetch - BLOCK_SIZE);
		set_prefetch_bit(nextFetch + BLOCK_SIZE);
	  	issue_prefetch(nextFetch + BLOCK_SIZE);
#endif
  	}
}

void prefetch_complete(Addr addr)
{
	//cout << "-----------------------Fetched " << addr << endl;
}

void prefetch_init(void)
{
	manager = new ElemManager();
}
