#include "interface.hh"

ElemManager Manager;

void prefetch_init(void)
{
}

void prefetch_access(AccessStat stat)
{
  int nextFetch =  Manager.findNextFetch((int)stat.mem_add);
  if(!in_cache((uint64_t)nextFetch))
    {
      issue_prefetch((uint64_t)nextFetch);
    } 
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
