#include "elem.hh"
#include "interface.hh"
#include "balancing.hh"
#include <limits>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <deque>

using namespace std;
#define OPTIMIZE 0

bool insert(deque<XElem*> vec, XElem* n)
{
	//cout << "insert " << n->delta << endl;
    int max = vec.size() - 1;
    int min = 0;
    int mid = max / 2;
    while(min <= max)
    {
        mid = min + (max - min) / 2;
        
        if (n->delta > vec[mid]->delta)
        {
            min = mid + 1;
        }
        else
        {
            max = mid - 1;
        }
    }
    
    if (vec.size() == 0)
    {
//    	cout << "started X" << endl;
        vec.push_back(n);
        return true;
    }
    else if (mid < vec.size() && mid >= 0 && vec[mid]->delta == n->delta)
    {    	
    	return false;
    }
    else if (vec[vec.size() - 1]->delta < n->delta)
    {
    	vec.push_back(n);
    	return true;
    }
    else if (vec[0]->delta > n->delta)
    {
        vec.push_front(n);
        return true;
    }
    else if (vec[mid]->delta > n->delta)
    {
//    	cout << "insert" << endl;
        vec.insert(vec.begin() + mid, n);
        return true;
    }
    else
    {
//    	cout << "retarded insert" << endl;
        vec.insert(vec.begin() + mid + 1, n);
        return true;
    }
        
}


bool insert(deque<YElem*> vec, YElem* n)
{
	//cout << "insert " << n->delta << endl;
    int max = vec.size() - 1;
    int min = 0;
    int mid = max / 2;
    while(min <= max)
    {
        mid = min + (max - min) / 2;
        
        if (n->delta > vec[mid]->delta)
        {
            min = mid + 1;
        }
        else
        {
            max = mid - 1;
        }
    }
    
    if (vec.size() == 0)
    {
    	//cout << "started Y" << endl;
        vec.push_back(n);
        return true;
    }
    else if (mid < vec.size() && mid >= 0 && vec[mid]->delta == n->delta)
    {    	
    	return false;
    }
    else if (vec[vec.size() - 1]->delta < n->delta)
    {
    	vec.push_back(n);
    	return true;
    }
    else if (vec[0]->delta > n->delta)
    {
        vec.push_front(n);
        return true;
    }
    else if (vec[mid]->delta > n->delta)
    {
    	//cout << "insert" << endl;
        vec.insert(vec.begin() + mid, n);
        return true;
    }
    else
    {
    	//cout << "retarded insert" << endl;
        vec.insert(vec.begin() + mid + 1, n);
        return true;
    }
        
}



int ElemManager::findElem(deque<XElem*> v, int delta)
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

int ElemManager::findElem(deque<YElem*> v, int delta)
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

XElem* ElemManager::findPtr(deque<XElem*> v, int delta)
{
	int pos = findElem(v, delta);
	if (pos < v.size() && pos > 0)
		return v[pos];
	else 
		return NULL;
}

YElem* ElemManager::findPtr(deque<YElem*> v, int delta)
{
	int pos = findElem(v, delta);
	if (pos < v.size() && pos > 0)
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
			
		if ((yE == NULL || xE == NULL))
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
	//if (nextFetchDelta)
	//	cout << "attempted fetch " << (address + nextFetchDelta) << ", delta: " << nextFetchDelta << endl;
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
		else
			cout << "retarded remove" << endl;
				
				
	}
	//need to recheck the elem count
	if (elemCount < MAX_ELEMS)
	{
		//we already know that at least one of the elements are non-existing.
		int find = findElem(elements, d1);
		
		YElem* yE = NULL;
		if (find < elements.size() && find >= 0)
			yE = findPtr(elements, yE->delta);
		
		XElem* xE = new XElem(d2, mFetches);

		if (yE == NULL || d1 != yE->delta)
		{
			yE = new YElem(d1);
			insert(elements, yE);
		}
		if (insert(yE->elements, xE))
			elemCount++;
		//cout << "new combo: " << d1 << ", " << d2 << endl;
		return xE;
	}
	cout << "ELEMCOUNT TOO HIGH: " << elemCount << endl;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

ElemManager* manager;

void prefetch_access(AccessStat stat)
{
	int nextFetch = manager->findNextFetch((int)stat.mem_addr);
	
	if (stat.miss)
		cout << "miss:  " << stat.mem_addr << "\t" << stat.time << endl;
	else
		cout << "hit:   " << stat.mem_addr << endl;
	
	if(nextFetch < MAX_PHYS_MEM_ADDR && !in_cache((uint64_t)nextFetch) && nextFetch != stat.mem_addr && current_queue_size() < MAX_QUEUE_SIZE)
	{
		set_prefetch_bit(nextFetch);
		cout << "Fetch: " << nextFetch << "\t" << stat.time << endl;
	  	issue_prefetch((uint64_t)nextFetch);
	  	for (deque<YElem*>::iterator it = manager->elements.begin(); it != manager->elements.end(); it++)
	  		cout << "\t\td: " << (*it)->delta << endl;
  	}
}

void prefetch_complete(Addr addr)
{
	cout << "-----------------------Fetched " << addr << endl;
}

void prefetch_init(void)
{
	manager = new ElemManager();
}
