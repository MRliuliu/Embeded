#include <stdlib.h>
#include <stdio.h>

#include "list.h"


struct xnode{
	double x;
	double y;
	struct list_head head;
};
#define to_xnode(p) \
	container_of(p, struct xnode, head)
void xnode_add(struct list_head* list, double x, double y){
	struct xnode * xp;   
	xp = malloc(sizeof(struct xnode));
	xp->x = x;
	xp->y = y;
	list_add( &xp->head , list);
}

int main(void){     
	struct list_head xlist;
	INIT_LIST_HEAD(&xlist);   
	struct list_head * tmp;  
	//添加节点         
	int i;
	for(i=0; i<5; i++)
		xnode_add(&xlist, 1.1+i, 2.2+i);
	//打印节点
	list_for_each(tmp,&xlist){  
		struct xnode *xp = to_xnode(tmp);
		printf("(%f,%f)\n", xp->x, xp->y);
	}             
}

