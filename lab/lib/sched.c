#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.14 ***/
void sched_yield(void)
{

    static int count = 0; // remaining time slices of current env
    static int point = 0; // current env_sched_list index
    int num =  0;
    struct Env *ee;
    

   ee = LIST_FIRST(&env_sched_list[point]);	
   if(count==0){
   	/*LIST_FOREACH(ee,&env_sched_list[point],env_sched_link){
   		if(ee->env_status==ENV_RUNNABLE) break;
		num++;
   	}
   	if(ee==NULL) point = 1-point;
	else if(num==0){
		LIST_REMOVE(ee,env_sched_link);
		LIST_INSERT_TAIL(&env_sched_list[1-point],ee,env_sched_link);
	}*/
	if(ee==NULL){
		point = 1-point;
		ee = LIST_FIRST(&env_sched_list[point]);
	}
	else{
		LIST_REMOVE(ee,env_sched_link);
		LIST_INSERT_TAIL(&env_sched_list[1-point],ee,env_sched_link);
		ee = LIST_FIRST(&env_sched_list[point]);
	}
   }

    /*  hint:
     *  1. if (count==0), insert `e` into `env_sched_list[1-point]`
     *     using LIST_REMOVE and LIST_INSERT_TAIL.
     *  2. if (env_sched_list[point] is empty), point = 1 - point;
     *     then search through `env_sched_list[point]` for a runnable env `e`, 
     *     and set count = e->env_pri
     *  3. count--
     *  4. env_run()
     *
     *  functions or macros below may be used (not all):
     *  LIST_INSERT_TAIL, LIST_REMOVE, LIST_FIRST, LIST_EMPTY
     */
     //printf("count: %d point: %d\n",count,point);
    /* LIST_FOREACH(ee,&env_sched_list[point],env_sched_link)
     	if(ee->env_status==ENV_RUNNABLE) break;	*/

     if(ee==NULL){	
    	point = 1-point; 
	/*LIST_FOREACH(ee,&env_sched_list[point],env_sched_link)
		if(ee->env_status == ENV_RUNNABLE) break;
	if(ee==NULL){
		panic("no env to run!!\n");
	}*/
	ee = LIST_FIRST(&env_sched_list[point]);
	if(ee==NULL) panic("no env to run!!\n");

     }

     if(count==0) count =ee->env_pri;
     count--;
//     printf("%x\n ",ee);
     env_run(ee);
     
}
