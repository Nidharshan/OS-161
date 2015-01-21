#include <types.h>
//#include <file_syscalls.h>
#include <lib.h>
#include <machine/vm.h>
#include <coremap.h>
#include <addrspace.h>

static struct cm_entry *coremap;
static unsigned int cm_entries;
static unsigned int cm_lastentry;

//first physical address after coremap boot
static paddr_t freespace;

//function for bootstraping the coremap

paddr_t
coremap_bootstrap (void)
{
        //kprintf("coremap_bootstrap:\n\n");
	unsigned total_pages;
	unsigned coremap_size;
	paddr_t first;
	paddr_t last;

	//Getting the ram size
	ram_getsize(&first, &last);

	//Getting total pages
	total_pages = (last - first) / PAGE_SIZE;

	coremap_size = total_pages * sizeof(struct cm_entry);

	//Aligning coremap size with the page size
	coremap_size = roundUp(coremap_size, PAGE_SIZE);

	// stealing pages for coremap
	coremap = (struct cm_entry *) PADDR_TO_KVADDR(first);
	first += coremap_size;
	first = first % PAGE_SIZE != 0 ? (first & PAGE_FRAME) + PAGE_SIZE : first;

	//setting coremap base address
	freespace=first;

	// Calulating the total coremap entries
	cm_entries = (last / PAGE_SIZE) - (first / PAGE_SIZE);

	// initialize coremap entries
	init_coremap();

	cm_lastentry=0;

	return freespace;
}


//function to initialize a coremap
void init_coremap(void){
        unsigned int i=0; 
	//kprintf("coremap init\n");
	for (i = 0; i < cm_entries; i++) {
		coremap[i].is_allocated = 0;
		coremap[i].is_kern_page = 0;
		coremap[i].chunk_cnt = 0;
	}
}

// function to roundup a number in the multiple of page_size
unsigned roundUp(unsigned numToRound, int multiple)
{
	 //kprintf("Roundup\n");

	return (numToRound + multiple - 1) & ~(multiple - 1);
}

// Funtion to ad entry to the coremap
void addtocoremap(paddr_t paddr, int npages, bool iskern){
	int index=getindex(paddr);
        int i=0;
	 //kprintf("coremap addtocoremap\n");

	for(i=0;i<npages;i++){
		coremap[index].chunk_cnt = npages;
		coremap[index].is_allocated = 1;
		coremap[index].is_kern_page = iskern;
		index++;
	}
}


//function to remove entry from coremap

void removefromcoremap(paddr_t paddr){

	int index = getindex(paddr);
        int i=0;
	 //kprintf("coremap removefromcoremap\n");

	for(i=0;i<coremap[index].chunk_cnt;i++){
		coremap[index].chunk_cnt = 0;
		coremap[index].is_allocated = 0;
		coremap[index].is_kern_page = 0;
		index++;
	}

}

//funtion to get the index of a coremap entry
int getindex(paddr_t paddr){
	// kprintf("coremap getindex\n");

	return (paddr-freespace) / PAGE_SIZE;
}

// get the physical address by iterating the coremap
paddr_t getpaddr(int npages){
	int count=0;
        unsigned int i=0;
	 //kprintf("coremap getpaddr\n");

	for(i=0;i<cm_entries;i++){
		if(coremap[i].is_allocated==0){
			count++;
			if(count==npages){
				return indextopaddr(i-count+1);
			}
		}else{
			count=0;
		}
	}
	return 0;
}

//convert the coremap index to physical address
paddr_t indextopaddr(int index){
	return (index * PAGE_SIZE)+freespace;
}

