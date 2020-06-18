#include "lib.h"



void umain(){
	int i=0,who,dstva,perm;
	int envid1=0x800;
	int envid2=0x1802;
	//dstva = 0x600000;
	//syscall_mem_alloc(0,dstva,PTE_V|PTE_R);
	writef("B\n");
	for(;;){
	  ipc_send_double(envid1,envid2,i,0,PTE_V|PTE_R);
	  i = ipc_recv(&who,0,&perm);
	  i = ipc_recv(&who,0,&perm);
	  if(i==5) break;
	  i++;
	}
}
