#include "interface.hh"

ElemManager Manager;

void prefetch_access(AccessStat stat)
{
  int nextFetch =  Manager.findNextFetch((int)stat.mem_add);
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
}
