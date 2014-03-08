#include "interface.hh"
#include <stdint>

#define MAX_ELEMS 256
#define 

class XElem
{
public:
	long d2;
	
	inline long getNext() { return candidate; };
	

	/**
	apply actual result, and update score. Change candidate if score == 0
	*/
	void applyActualResult(long d3);
private:
	long candidate;
	uint8_t score;
	
};


class YElem
{
public:
	long d1;
	XElem* getElem(long d2);
	void applyScore(long d2, long d3);
private:
	std::vector<XElem*> next;
};


class ElemManager
{
public:
	/**
	tell the element the actual delta it got
	*/
	void lastActualCandidate(long d3);

	/**
	Search for the element
	*/
	XElem* getElem(long d1, long d2);

	/**
	return 0 if not found, and add combination if more room
	*/
	long getDelta(long d1, long d2);
	
private:
	/**
	used by getDelta to add new combo
	*/
	void addCombination(long d1, long d2);

	uint16_t elemCount;
	std::vector<YElem*> elements;
};


