#include "lib.h"
#include <mmu.h>
#include <env.h>
#include <kerelf.h>

#define debug 0
#define TMPPAGE		(BY2PG)
#define TMPPAGETOP	(TMPPAGE+BY2PG)

int
init_stack(u_int child, char **argv, u_int *init_esp)
{
	int argc, i, r, tot;
	char *strings;
	u_int *args;

	// Count the number of arguments (argc)
	// and the total amount of space needed for strings (tot)
	tot = 0;
	for (argc=0; argv[argc]; argc++)
		tot += strlen(argv[argc])+1;

	// Make sure everything will fit in the initial stack page
	if (ROUND(tot, 4)+4*(argc+3) > BY2PG)
		return -E_NO_MEM;

	// Determine where to place the strings and the args array
	strings = (char*)TMPPAGETOP - tot;
	args = (u_int*)(TMPPAGETOP - ROUND(tot, 4) - 4*(argc+1));

	if ((r = syscall_mem_alloc(0, TMPPAGE, PTE_V|PTE_R)) < 0)
		return r;
	// Replace this with your code to:
	//
	//	- copy the argument strings into the stack page at 'strings'
	char *ctemp,*argv_temp;
	u_int j;
	ctemp = strings;
	for(i = 0;i < argc; i++)
	{
		argv_temp = argv[i];
		for(j=0;j < strlen(argv[i]);j++)
		{
			*ctemp = *argv_temp;
			ctemp++;
			argv_temp++;
		}
		*ctemp = 0;
		ctemp++;
	}
	//	- initialize args[0..argc-1] to be pointers to these strings
	//	  that will be valid addresses for the child environment
	//	  (for whom this page will be at USTACKTOP-BY2PG!).
	ctemp = (char *)(USTACKTOP - TMPPAGETOP + (u_int)strings);
	for(i = 0;i < argc;i++)
	{
		args[i] = (u_int)ctemp;
		ctemp += strlen(argv[i])+1;
	}
	//	- set args[argc] to 0 to null-terminate the args array.
	ctemp--;
	args[argc] = ctemp;
	//	- push two more words onto the child's stack below 'args',
	//	  containing the argc and argv parameters to be passed
	//	  to the child's umain() function.
	u_int *pargv_ptr;
	pargv_ptr = args - 1;
	*pargv_ptr = USTACKTOP - TMPPAGETOP + (u_int)args;
	pargv_ptr--;
	*pargv_ptr = argc;
	//
	//	- set *init_esp to the initial stack pointer for the child
	//
	*init_esp = USTACKTOP - TMPPAGETOP + (u_int)pargv_ptr;
//	*init_esp = USTACKTOP;	// Change this!

	if ((r = syscall_mem_map(0, TMPPAGE, child, USTACKTOP-BY2PG, PTE_V|PTE_R)) < 0)
		goto error;
	if ((r = syscall_mem_unmap(0, TMPPAGE)) < 0)
		goto error;

	return 0;

error:
	syscall_mem_unmap(0, TMPPAGE);
	return r;
}

int usr_is_elf_format(u_char *binary){
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
	if (ehdr->e_ident[0] == ELFMAG0 &&
        ehdr->e_ident[1] == ELFMAG1 &&
        ehdr->e_ident[2] == ELFMAG2 &&
        ehdr->e_ident[3] == ELFMAG3) {
        return 1;
    }   

    return 0;
}

int 
usr_load_elf(u_int va , Elf32_Phdr *ph, int child_envid, u_char * bin){
	//Hint: maybe this function is useful 
	//      If you want to use this func, you should fill it ,it's not hard
    struct Env *env = &(envs[ENVX(child_envid)]);
    u_long i,r,cnt=0;
    u_long offset = va - ROUNDDOWN(va, BY2PG);
    u_char* pos ; 
    u_long dva = ROUNDDOWN(va,BY2PG);
    u_int bin_size = ph->p_filesz;
    u_int sgsize = ph->p_memsz;
    for (i = 0; cnt<bin_size; i += BY2PG) {
        /* Hint: You should alloc a new page. */
	//writef("check_load_va is %x ",dva+i);
	if(syscall_mem_alloc(0,TMPPAGE,PTE_R|PTE_V)<0)
		return -1;
	
	r = i==0?offset:0;
	pos = TMPPAGE;
	for(; r<BY2PG && cnt<bin_size; r++,cnt++)	
		*(pos+r) = *(bin+cnt);
	//check
	syscall_mem_map(0,TMPPAGE,child_envid,dva+i,PTE_R|PTE_V);
	syscall_mem_unmap(0,TMPPAGE);
	//writef("cool!\n");

	//writef("check_load_should_be:%x\n",page2pa(p));
    }
    /*Step 2: alloc pages to reach `sgsize` when `bin_size` < `sgsize`.
    * hint: variable `i` has the value of `bin_size` now! */
    sgsize += offset;
    //writef("begin load bss!\n");
    while (i < sgsize) {
	if(syscall_mem_alloc(child_envid,dva+i,PTE_R|PTE_V)<0)
		return -1;
	i += BY2PG;
    }
    //printf("bin: %x bins: %x segsize: %x va: %x dva: %x,i: %x,cnt: %x\n",
    	//bin,bin_size,sgsize,va,dva,i,cnt);
    
    return 0;
}

int spawn(char *prog, char **argv)
{
	u_char elfbuf[512],*binary;
	int r;
	u_int child_envid;
	int size, text_start=0;
	u_int i, *blk;
	u_int esp;
	Elf32_Ehdr* elf;
	Elf32_Phdr* phdr;
	struct Fd * fd;
	struct File file;
	// Note 0: some variable may be not used,you can cancel them as you like
	// Step 1: Open the file specified by `prog` (prog is the path of the program)
	if((r=open(prog, O_RDONLY))<0){
		//user_panic("spawn ::open line 102 RDONLY wrong !\n");
		return r;
	}
	// Your code begins here
	// Before Step 2 , You had better check the "target" spawned is a execute bin 
	fd = num2fd(r);
	binary = fd2data(fd);
	size = (((struct Filefd*) fd)->f_file).f_size;
	if(size<4||usr_is_elf_format(binary)==0){
		//user_panic("spawn ::a not elf fomat !\n");
		return -1;
	}
	
	// Step 2: Allocate an env (Hint: using syscall_env_alloc())
	r = syscall_env_alloc();
	if(r<0){
		//user_panic("spawn ::env_alloc fail!\n");
		return r;

	}
	child_envid = r;
	// Step 3: Using init_stack(...) to initialize the stack of the allocated env
	r = init_stack(child_envid,argv,&esp);
	if(r<0){
		//user_panic("spawn ::init_stack fail!\n");
		return r;
	}
	// Step 3: Map file's content to new env's text segment
	//        Hint 1: what is the offset of the text segment in file? try to use objdump to find out.
	//        Hint 2: using read_map(...)
	//		  Hint 3: Important!!! sometimes ,its not safe to use read_map ,guess why 
	//				  If you understand, you can achieve the "load APP" with any method
	// Note1: Step 1 and 2 need sanity check. In other words, you should check whether
	//       the file is opened successfully, and env is allocated successfully.
	// Note2: You can achieve this func in any way 锛宺emember to ensure the correctness
	//        Maybe you can review lab3 
	elf = binary;
        u_char *ptr_ph_table = binary + elf->e_phoff;
        Elf32_Half ph_entry_count = elf->e_phnum;
        Elf32_Half ph_entry_size = elf->e_phentsize;
        while (ph_entry_count--) {
                phdr = (Elf32_Phdr *)ptr_ph_table;
                if (phdr->p_type == PT_LOAD) {
			//writef("off is %x va is %x fileseize is %x memsize is %x\n",\
			phdr->p_offset,phdr->p_vaddr,phdr->p_filesz,phdr->p_memsz);
			if(usr_load_elf(phdr->p_vaddr,phdr,child_envid,binary+phdr->p_offset)!=0){
		   		//user_panic("spawn ::load fail!\n");
		   		return -1;
			}
		}
                ptr_ph_table += ph_entry_size;
        }
	//close(fd2num(fd));

	// Your code ends here

	struct Trapframe *tf;
	//writef("\n::::::::::spawn size : %x  sp : %x::::::::\n",size,esp);
	tf = &(envs[ENVX(child_envid)].env_tf);
	//text 鏈韩灏辫鍔犺浇鍒癠TEXT娈典笉闇€鐗规畩澶勭悊
	//the lds load .text to off at 0x1000,va at UTEXT
	tf->pc = UTEXT;
	tf->regs[29]=esp;


	// Share memory
	u_int pdeno = 0;
	u_int pteno = 0;
	u_int pn = 0;
	u_int va = 0;
	for(pdeno = 0;pdeno<PDX(UTOP);pdeno++)
	{
		if(!((* vpd)[pdeno]&PTE_V))
			continue;
		for(pteno = 0;pteno<=PTX(~0);pteno++)
		{
			pn = (pdeno<<10)+pteno;
			if(((* vpt)[pn]&PTE_V)&&((* vpt)[pn]&PTE_LIBRARY))
			{
				va = pn*BY2PG;

				if((r = syscall_mem_map(0,va,child_envid,va,(PTE_V|PTE_R|PTE_LIBRARY)))<0)
				{

					writef("va: %x   child_envid: %x   \n",va,child_envid);
					writef("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
					return r;
				}
			}
		}
	}


	if((r = syscall_set_env_status(child_envid, ENV_RUNNABLE)) < 0)
	{
		writef("set child runnable is wrong\n");
		return r;
	}
	return child_envid;		

}

int
spawnl(char *prog, char *args, ...)
{
	return spawn(prog, &args);
}

