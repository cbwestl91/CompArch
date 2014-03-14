/* -*- mode: c++ -*- */

#pragma once
#include "interface.hh"
#include <stdint.h>
#include <vector>

#define MAX_ELEMS 256



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
	std::vector<XElem*> elements;
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
	

	
private:
	/**
	used by getDelta to add new combo
	*/
	XElem* addCombination(int d1, int d2);
	
	XElem* findElem(std::vector<XElem*>, int);
	YElem* findElem(std::vector<YElem*>, int);

	uint16_t elemCount;
	std::vector<YElem*> elements;

	unsigned int mFetches;
};


