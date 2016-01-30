#ifndef __LIST_H_
#define __LIST_H_

struct list_head{
	struct list_head *next, *prev;
};

//list init function.
static inline void INIT_LIST_HEAD(struct list_head *list){
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
							  struct list_head *prev,
							  struct list_head *next){
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head){
	__list_add(new, head, head->next);
}

#define list_for_each(pos, head) \
	for(pos = (head)->next; pos != (head); pos = pos->next)

#define container_of(ptr, type, member) \
	(type *)((char *)ptr - (size_t)&((type *)0)->member)

#endif



