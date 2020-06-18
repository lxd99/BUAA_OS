#include "pageReplace.h" // 测评需求，请务必包含该头文件
#include <bits/stdc++.h>
#define MAX_PHY_PAGE 64 // 这里只使用了8个物理页框
#define MAX_PAGE 12
#define get_Page(x) (x>>MAX_PAGE)
#define mk(a,b) std::make_pair(a,b)
//#define debug
#define LIST_REMOVE(p) \
{\
    if(p->e_link.l) *(p->e_link.l) = p->e_link.r; \
    if(p->e_link.r)  p->e_link.r->e_link.l = p->e_link.l; \
    p->e_link.l= NULL; \
    p->e_link.r = NULL; \
}while(0)    

#define LIST_INSERT_HEAD(p)  \
{\
    if(head.to != NULL) head.to->e_link.l = &(p->e_link.r); \
    p->e_link.r = head.to; \
    p->e_link.l = &(head.to); \
    head.to = p;   \
}while(0)

#define LIST_INSERT_TAIL(p) \
{  \
    if(tail.to == NULL){ \
	 LIST_INSERT_HEAD(p); }\
    else{ \
        p->e_link.l = &(tail.to->e_link.r); \
        tail.to->e_link.r = p;   \
    }                           \
    tail.to = p;  \
}while(0) 
int len,flag[1<<22];
struct link{
    struct data **l,*r;
};
struct data{
    link e_link;
    int  id;
}p[MAX_PHY_PAGE],*mp,*mpp;
struct HEAD{
    struct data *to;
}head,tail;
void pageReplace(long * physic_memery, long nwAdd) {
	static int count=0;
    count++;
    int pa = get_Page(nwAdd);
    if(flag[pa]){
        mp = &p[flag[pa]-1];
        if(mp==tail.to) return;
        LIST_REMOVE(mp);
        LIST_INSERT_TAIL(mp);
        return ;
    }
    if(len<MAX_PHY_PAGE){
        p[len].id = len;
        flag[pa] = len+1;
        LIST_INSERT_TAIL((&p[len]));
        physic_memery[len] = pa;
        len++;
        return ;
    }
    mp = head.to;
    int mlen = mp->id; 
	LIST_REMOVE(mp);  
    LIST_INSERT_TAIL(mp);
    flag[physic_memery[mlen]] = 0;
    flag[pa]=mlen+1;
    physic_memery[mlen] = pa;
}
