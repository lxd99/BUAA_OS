/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>
#define CHE(x) if(x<0) syscall_panic("ERROR!\n");
// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurred during read the IDE disk, panic. 
// 	
// Hint: use syscalls to access device registers and buffers
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	char read = 0;
	int off;
	int x = 0;
	int ret;


	while (offset_begin + offset < offset_end) {
	    off = offset_begin+offset; 
	    
	    ret=syscall_write_dev(&diskno,IDE+0x10,4); 
	    CHE(ret);

	    ret=syscall_write_dev(&off,IDE,4);
	    CHE(ret);

	    ret=syscall_write_dev(&read,IDE+0x20,1);
	    CHE(ret);

	    ret=syscall_read_dev(&x,IDE+0x30,4); 
	    CHE(ret);
	    if(x==0) syscall_panic("ERROE!\n");

	    ret=syscall_read_dev(dst+offset,IDE+0x4000,0x200);//copy from buffer
	    CHE(ret);
	    
	    offset +=0x200;
            // Your code here
            // error occurred, then panic.
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurred during read the IDE disk, panic.
//	
// Hint: use syscalls to access device registers and buffers
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
        // Your code here
	int offset_begin = secno*0x200;
	int offset_end = offset_begin + nsecs*0x200;
	int offset = 0;
	int ret;
	int  off;
	char write = 1;
	int x = 0;
	writef("diskno: %d\n", diskno);
	while (offset_begin + offset < offset_end) {
	    off = offset_begin + offset;

	    ret=syscall_write_dev(src+offset,IDE+0x4000,0x200);//write of buffer
	    CHE(ret);

	    ret=syscall_write_dev(&diskno,IDE+0x10,4); 
	    CHE(ret);

	    ret=syscall_write_dev(&off,IDE,4);
	    CHE(ret);
	    

	    ret=syscall_write_dev(&write,IDE+0x20,1);
	    CHE(ret);

	    ret=syscall_read_dev(&x,IDE+0x30,4); 
	    CHE(ret);
	    if(x==0) syscall_panic("ERROE!\n");
	    
	    offset +=0x200;
	}
}
