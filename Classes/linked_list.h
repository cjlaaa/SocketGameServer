//
//  linked_list.h
//  testServer_Mac
//  链表宏定义头文件
//
//  Created by hENRYcHANG on 14-9-9.
//  Copyright (c) 2014年 hENRYcHANG. All rights reserved.
//

#ifndef testServer_Mac_linked_list_h
#define testServer_Mac_linked_list_h

//数据插入到链表
#define INSERT_TO_LIST(list,item,prev,next)     \
if(!list)                                       \
{                                               \
    list = item;                                \
	list->prev = list->next = list;             \
}                                               \
else                                            \
{                                               \
	item->prev = list->prev;                    \
	item->next = list;                          \
	item->prev = item;                          \
	item->prev->next = item;                    \
}

//从链表中取数据
#define REMOVE_FROM_LIST(list,item,prev,next)	\
if(item==list)									\
{												\
	if (item==item->next)						\
		list = NULL;							\
	else										\
		list = item->next;						\
}												\
if(list)										\
{												\
	item->prev->next = item->next;				\
	item->next->prev = item->prev;				\
}

//链表数据检索循环开始的宏定义
#define LIST_WHILE(list,item,temp_d,next)		\
if(item = list)									\
{												\
	do {										\
		temp_d = item->next;

//链表数据检索循环结束的宏定义
#define LIST_WHILEEND(list,item,temp_d)			\
		item = temp_d;							\
	} while(list&&item&&(item!=list));			\
}

//跳过链表的宏定义.
#define LIST_SKIP(item,temp_d)					\
{												\
	item = temp_d;								\
	continue;									\
}

#endif

//EOF
