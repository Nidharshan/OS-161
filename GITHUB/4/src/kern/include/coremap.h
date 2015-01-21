#ifndef COREMAP_H_
#define COREMAP_H_

#include<types.h>
#include<machine/vm.h>
#include<vm.h>

typedef enum {FALSE,TRUE} bool;

struct cm_entry{
	unsigned int chunk_cnt:17;
	unsigned char is_kern_page:1;
	unsigned char is_allocated:1;
};

paddr_t coremap_bootstrap (void);
paddr_t getpaddr(int npages);
void init_coremap(void);
void addtocoremap(paddr_t paddr,int npages, bool iskern);
unsigned roundUp(unsigned  numToRound, int multiple);
void removefromcoremap(paddr_t paddr);
int getindex(paddr_t paddr);
paddr_t indextopaddr(int index);
#endif /* COREMAP_H_ */

