// User-level IPC library routines

#include "lib.h"
#include <mmu.h>
#include <env.h>

extern struct Env *env;

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void
ipc_send(u_int whom, u_int val, u_int srcva, u_int perm)
{
	int r;
	writef("%x\n",env);

	while ((r = syscall_ipc_can_multi_send(val, srcva, perm,1,whom)) == -E_IPC_NOT_RECV) {
		writef("dick!\n");
		syscall_yield();
		//writef("QQ");
	}

	writef("r is %x\n",r);
	if (r == 0) {
		return;
	}

	user_panic("error in ipc_send: %d", r);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int
ipc_recv(u_int *whom, u_int dstva, u_int *perm)
{
	//printf("ipc_recv:come 0\n");
	syscall_ipc_recv(dstva);
	writef("recv_finish,whom is %x,perm is %x\n",whom,perm);

	if (whom) {
		writef("%x\n",env);
		syscall_getenvid();
		*whom = env->env_ipc_from;
	}
	//writef("whom finish\n");

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
int ipc_send_double(u_int envid_1,u_int envid_2, int value ,u_int srcva, u_int perm){
	int r=0;
	while((r=syscall_ipc_can_multi_send(value,srcva,perm,2,envid_1,envid_2))==-E_IPC_NOT_RECV) {
	writef("yeidl!\n");
	syscall_yield();
	}
	writef("end!\n");
	return r;
}

