#ifndef LIST_H
#define LIST_H

/*
 * 通用的链表实现
 * 
 * 一般将NODE加入自己定义的数据结构中，用NODE维护链表结构
 * NODE和自己定义的结构体通过list_entry转换
 *
 * 自定义结构体
 * struct USER_DATA{
 *	NODE list;
 *	int value;
 * }my_data;
 */

#ifdef __cplusplus
extern "C" {
#endif

/* 定义一个通用的链表节点NODE */
typedef struct list_head {
        struct list_head *next, *prev;
}NODE;
 
 
/* 定义并初始化一个链表头 */
#define LIST_HEAD(name) \
        NODE name = { &(name), &(name) } 

/* 初始化链表头 */
static inline void list_init(NODE *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(NODE *now,
                              NODE *prev,
                              NODE *next)
{
        next->prev = now;
        now->next = next;
        now->prev = prev;
        prev->next = now;
}

/* 将now加入head为头的链表中（前插）*/
static inline void list_add(NODE *now, NODE *head)
{
        __list_add(now, head, head->next);
}

/* 将now加入head为头的链表中（后插）*/
static inline void list_add_tail(NODE *now, NODE *head)
{
	__list_add(now, head->prev, head);
}

/* 删除链表中的节点*/
static inline void list_del(NODE *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
	entry->prev = entry->next = entry;
}

/* 判断链表是否为空*/
static inline int list_empty(const NODE *head)
{
	return head->next == head;
}

/* list节点是不是head链表中的最后一个*/
static inline int list_is_last(const NODE *list,
			       const NODE *head)
{
	return list->next == head;
}

/*
 * 用于将NODE 转换为包含他的USER_DATA结构
 * (NODE node,  struct USER_DATA,  ptr对应的NODE在struct USER_DATA 中的名字)  
 */
#define list_entry(node, type, member)                                   \
        ((type *)((char *)(node)-(unsigned long)(&((type *)0)->member)))

/*
 * 遍历NODE组成的链表head
 * ( struct USER_DATA *udptr, NODE *head, NODE在struct USER_DATA 中的名字)
 */
#define list_for_each_entry(udptr, head, member)                          \
        for (udptr = list_entry((head)->next, typeof(*udptr), member);      \
             &udptr->member != (head);                                    \
             udptr = list_entry(udptr->member.next, typeof(*udptr), member))

#define list_for_each_entry_safe(udptr, n, head, member)		\
	for (udptr = list_entry((head)->next, typeof(*udptr), member),	\
	       n = list_entry(udptr->member.next, typeof(*udptr), member);	\
	     &udptr->member != (head); 					\
	     udptr = n, n = list_entry(n->member.next, typeof(*n), member))




#ifdef __cplusplus
}
#endif

#endif
