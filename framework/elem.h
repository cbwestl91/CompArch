/* -*- mode: c++ -*- */

#pragma once
#include "interface.hh"
#include <stdint>
#include <vector>

#define MAX_ELEMS 256



class Elem 
{
public:
	int delta;
};


//search algorithm
Elem* findElem(std::vector<Elem*>);


class XElem: public Elem
{
public:
	
	inline int getNext() { return candidate; };
	
	XElem(int delta) 
	{ 
		this->delta = delta; 
		candidate = 0;
		score = 0;
	}
	/**
	* apply actual result, and update score. Change candidate if score == 0
	*/
	void applyActualResult(int d3);
private:
	int candidate;
	uint8_t score;
};


class YElem : public Elem
{
public:
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
	
private:
	/**
	used by getDelta to add new combo
	*/
	void addCombination(int d1, int d2);

	uint16_t elemCount;
	std::vector<YElem*> elements;

	unsigned int mFetches;
};


