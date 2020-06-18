#include "lib.h"


void umain()
{
	int a = 0;
	int id = 0;
	//writef("begin\n");
	//writef("env_pgdir_check:%x\n",*((Pte*)vpt+PPN(ULIM)));
	//writef("id'addr:%x\n",&id);
	//id=fork();
	//writef("dick!\n");
	//int dick =syscall_env_alloc();
	
	/*if(id!=0){
		writef("father\n");
	}else{
		writef("son\n");
	}*/

	/*if ((id= fork()) == 0) {
		//writef("dick!!!\n");
		if ((id = fork()) == 0) {
			a += 3;

			for (;;) {
				writef("\t\tthis is child2 :a:%d\n", a);
			}
		}
		//writef("dick!\n");


		a += 2;

		for (;;) {
			writef("\tthis is child :a:%d\n", a);
		}
	}
	//writef("dick!\n");
	a++;

	for (;;) {
		writef("this is father: a:%d\n", a);
	}*/
	//syscall_mem_map(0,0x400000,0,0x500000,PTE_V|PTE_R);
	syscall_ipc_can_multi_send(7,0,PTE_V|PTE_R,6,1,2,3,4,5,6);
}
