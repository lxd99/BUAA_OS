#include "lib.h"


void umain(){
	int i;
	char ch;
	for(i=0;i<=10;i++){
		writef("count:%d\n",i);
		syscall_read_dev(&ch,0x10000000,1);
	}

}
