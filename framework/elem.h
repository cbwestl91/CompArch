#include "interface.hh"
#include <stdint>
#include <vector>

#define MAX_ELEMS 256



class Elem 
{
public:
	long delta;
};


//search algorithm
Elem* findElem(std::vector<Elem*>);


class XElem: public Elem
{
public:
	
	inline long getNext() { return candidate; };
	

	/**
	* apply actual result, and update score. Change candidate if score == 0
	*/
	void applyActualResult(long d3);
private:
	long candidate;
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
	void previousActualCandidate(long d3);

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


