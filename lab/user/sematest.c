#include "lib.h"

void umain(){
	int pid,full,empty,barrier,i;
	full=syscall_init_PV_var(0);
	empty=syscall_init_PV_var(1);
	barrier = syscall_init_PV_var(0);
	
	if((pid=fork())==0){
		for(i=0;i<10;i++){
			syscall_P(full);	
			writef("get_product!!!\n");
			syscall_V(empty);
		}
		syscall_V(barrier);
		syscall_P(full);
	}else{
		for(i=0;i<10;i++){
			syscall_P(empty);
				writef("make_product!!\n");
			syscall_V(full);
		}
		syscall_P(barrier);
		syscall_release_PV_var(full);	
		writef("end!\n");
	}
}
