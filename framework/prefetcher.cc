#include "interface.hh"
#include "elem.h"

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
