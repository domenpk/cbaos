#ifndef _LIST_H_
#define _LIST_H_

#include "magic.h"

/* ideas for this picked from linux */

/* if you have an apparent problem with lists, check you didn't add the same
 * member twice */

/* list initialization helpers */
#define LIST_INIT(name) { .next = &(name), .prev = &(name) }

#define LIST_DECLARE_INIT(name)	\
	struct list name = {	\
		.next = &name,	\
		.prev = &name,	\
	}

struct list {
	struct list *next;
	struct list *prev;
};

static inline void list_init(struct list *list)
{
	list->next = list;
	list->prev = list;
}

static inline void list_add(struct list *list, struct list *knew)
{
	knew->next = list->next;
	knew->prev = list;
	list->next->prev = knew;
	list->next = knew;
}

static inline void list_add_tail(struct list *list, struct list *knew)
{
	knew->next = list;
	knew->prev = list->prev;
	list->prev->next = knew;
	list->prev = knew;
}

static inline void list_del(struct list *todel)
{
	todel->prev->next = todel->next;
	todel->next->prev = todel->prev;
	todel->prev = todel->next = (struct list *)MAGIC_LIST_INVALID;
}

static inline int list_empty(struct list *list)
{
	return list == list->next;
}

#define list_entry(list, type, member)	\
	({ (type*)((char*)list-(char*)&(((type*)0)->member)); })

#define list_for_each(list, iterator)	\
	for (iterator=(list)->next; iterator!=list; iterator=iterator->next)

#define list_for_each_safe(list, iterator, saved)	\
	for (iterator=(list)->next, saved=iterator->next; iterator!=list; iterator=saved, saved=iterator->next)

#endif
