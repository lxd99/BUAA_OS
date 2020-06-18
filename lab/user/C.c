#include "lib.h"


void umain(){
	int i,who,dstva,perm;
	//dstva = 0x600000;
	//syscall_mem_alloc(0,dstva,PTE_V|PTE_R);
	writef("C\n");
	for(;;){
	   i = ipc_recv(&who,0,&perm);
	   writef("C  got %x\n",i);
	   i++;
	   ipc_send(who,i,0,PTE_V|PTE_R);
	   if(i==5) break;
	}

}
