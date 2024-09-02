﻿/*
 * Copyright (C) 2018 xiaoliang<1296283984@qq.com>.
 */

#ifndef __LIFO_H__
#define __LIFO_H__

#include "at_slist.h"

/*********************************************************
 *@type description:
 *
 *[lifo_t]: last-in-first-out queue implemented by single circular linked list
 *********************************************************
 *@类型说明：
 *
 *[lifo_t]：使用单循环链表实现的后进先出队列
 *********************************************************/
typedef struct lifo_s
{
    /* linked list for implementing last-in-first-out queue */
    /* 用于实现后进先出队列的链表 */
    slist_t list;
} lifo_t;


/************************************************************
 *@brief:
 ***last-in-first-out queue structure static initialization
 *
 *@Usage:
 ***lifo_t lifo = LIFO_STATIC_INIT(lifo);
 *
 *@parameter:
 *[head]: last-in-first-out queue variable name, non-address
 *************************************************************/
/************************************************************
 *@简介：
 ***后进先出队列结构体静态初始化
 *
 *@用法：
 ***lifo_t lifo = LIFO_STATIC_INIT(lifo);
 *
 *@参数：
 *[head]：后进先出队列变量名，非地址
 *************************************************************/
#define LIFO_STATIC_INIT(lifo)  {SLIST_STATIC_INIT((lifo).list)}


/************************************************************
 *@brief:
 ***Get the single circular linked list in lifo for traversing lifo
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *
 *@return: the single circular linked list in lifo
 *************************************************************/
/************************************************************
 *@简介：
 ***获取lifo中的单循环链表，用于遍历lifo
 *
 *@参数：
 *[lifo]：后进先出队列
 *
 *@返回：后进先出队列的单循环链表
 *************************************************************/
#define LIFO_LIST(lifo)      (&(lifo)->list)


/************************************************************
 *@brief:
 ***Get the linked list in lifo for traversing lifo
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *
 *@return: the linked list in lifo
 *************************************************************/
/************************************************************
 *@简介：
 ***获取lifo中的链表，用于遍历lifo
 *
 *@参数：
 *[lifo]：后进先出队列
 *
 *@返回：后进先出队列的链表
 *************************************************************/
#define LIFO_OF_LIST(slist)      (container_of(slist, lifo_t, list))


/*********************************************************
 *@brief: 
 ***get the top node of last-in-fist-out queue
 *
 *@contract: 
 ***1. lifo is not null pointer
 ***2. lifo is not empty
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *
 *@return type:
 *[slist_node_t *]: the node of single circular linked list
 *
 *@return: the top node of last-in-first-out queue
 *********************************************************/
/*********************************************************
 *@简要：
 ***获取后进先出队列的顶部节点
 *
 *@约定：
 ***1、lifo不是空指针
 ***2、lifo非空
 *
 *@参数：
 *[lifo]：后进先出队列
 *
 *@返回类型：
 *[slist_node_t *]：单循环链表的节点
 *
 *@返回：后进先出队列的顶部节点
 **********************************************************/
#define LIFO_TOP(lifo) SLIST_NODE_NEXT(SLIST_HEAD(&(lifo)->list))


/*********************************************************
 *@brief: 
 ***last-in-first-out queue initialization.
 *
 *@contract: 
 ***1. "lifo" is not null pointer
 ***2. cannot initialize the "lifo" being used
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *********************************************************/
/*********************************************************
 *@简要：
 ***后进先出队列初始化
 *
 *@约定：
 ***1、lifo不是空指针
 ***2、不可对正在使用的lifo进行初始化
 *
 *@参数：
 *[lifo]：后进先出队列
 **********************************************************/
static inline void lifo_init(lifo_t *lifo)
{
    slist_init(&lifo->list);
}


/*********************************************************
 *@brief: 
 ***determine if the LIFO queue is empty
 *
 *@contract: 
 ***1. lifo is not null pointer
 *
 *@parameter:
 *[lifo]: last-in-fist-out queue
 *
 *@return value:
 *[true]: last-in-first-out is empty
 *[false]: last-in-fist-out is not empty
 *********************************************************/
/*********************************************************
 *@简要：
 ***判断后进先出队列是否为空
 *
 *@约定：
 ***1、lifo不是空指针
 *
 *@参数：
 *[lifo]：后进先出队列
 *
 *@返回值：
 *[true]：后进先出队列为空
 *[false]：后进先出队列非空
 **********************************************************/
#define lifo_is_empty(lifo) slist_is_empty(&(lifo)->list)


/*********************************************************
 *@brief: 
 ***last-in-first-out queue enqueue
 *
 *@contract: 
 ***1. lifo and node are not null pointer
 ***2. the "node" is the deleted node
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *[node]: the node that needs to be enqueued
 *********************************************************/
/*********************************************************
 *@简要：
 ***后进先出队列入队
 *
 *@约定：
 ***1、lifo与node不是空指针
 ***2、node为已删除的节点
 *
 *@参数：
 *[lifo]：后进先出队列
 *[node]：需要入队的节点
 **********************************************************/
static inline void lifo_push(lifo_t *lifo, slist_node_t *node)
{
    slist_node_insert_next(SLIST_HEAD(&lifo->list), node);
}


/*********************************************************
 *@brief: 
 ***last-in-first-out queue dequeue
 *
 *@contract: 
 ***1. "lifo" is not null pointer
 ***2. "lifo" is not empty
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *
 *@return type:
 *[slist_node_t *]: the type of single circular linked list node
 *
 *@return: the node of dequeuing
 *********************************************************/
/*********************************************************
 *@简要：
 ***后进先出队列出队
 *
 *@约定：
 ***1、lifo不是空指针
 ***2、lifo非空
 *
 *@参数：
 *[lifo]：后进先出队列
 *
 *@返回：出队的节点
 **********************************************************/
static inline slist_node_t* lifo_pop(lifo_t *lifo)
{
    return slist_node_del_next(SLIST_HEAD(&lifo->list));
}


/*********************************************************
 *@brief: 
 ***insert the next node at the node of the-last-in-first-out queue
 *
 *@contract: 
 ***1. "node" and "next_node" are not null pointers
 ***2. "node" is the node in the last-in-first-out queue
 ***3. "next_node" is the deleted node
 *
 *@parameter:
 *[node]: "node" is the node in last-in-first-out queue
 *[next_node]: "next_node" is the node of be inserted
 *********************************************************/
/*********************************************************
 *@简要：
 ***在后进先出队列的节点处插入下一个节点
 *
 *@约定：
 ***1、node和next_node不是空指针
 ***2、node为后进先出队列中的节点
 ***3、next_node为已删除的节点
 *
 *@参数：
 *[node]：后进先出队列中的节点
 *[next_node]：需要插入的节点
 **********************************************************/
static inline void lifo_node_insert_next(slist_node_t *node, slist_node_t *next_node)
{
    slist_node_insert_next(node, next_node);
}


/*********************************************************
 *@brief: 
 ***remove the next node of the node in the-last-in-first-out queue,
 ***this function is usually used in the traversal process
 *
 *@contract: 
 ***1. "node" is not a null pointer
 ***2. "node" is in the last-in-first-out queue
 ***3. "node" is not the tail node of the last-in-first-out queue
 *
 *@parameter:
 *[node]: the node in last-in-first-out queue
 *
 *@return type:
 *[slist_node_t*]: the node of single circular linked list
 *
 *@return: the node of be deleted
 *********************************************************/
/*********************************************************
 *@简要：
 ***移除后进先出队列中节点的下一个节点，
 ***该函数通常在遍历过程中使用
 *
 *@约定：
 ***1、node不是空指针
 ***2、node处于后进先出队列中
 ***3、node不是后进先出队列的尾节点
 *
 *@参数：
 *[node]：后进先出队列中的节点
 *
 *@返回：被移除的节点
 **********************************************************/
static inline slist_node_t* lifo_node_del_next(slist_node_t *node)
{
    return slist_node_del_next(node);
}


/*********************************************************
 *@brief: 
 ***remove node from last-in-first-out queue
 *
 *@contract: 
 ***1. lifo and node is not null pointer
 *
 *@parameter:
 *[lifo]: last-in-first-out queue
 *
 *@return value:
 *[true]: successfully removed this node from the queue
 *[false]: this node is not in this queue
 *********************************************************/
/*********************************************************
 *@简要：
 ***从后进先出队列中移除节点
 *
 *@约定：
 ***1. lifo与node不是空指针
 *
 *@参数：
 *[lifo]：后进先出队列
 *
 *@返回值：
 *[true]：成功从队列中移除这个节点
 *[false]：这个节点不在这个队列之中
 **********************************************************/
static inline bool lifo_del_node(lifo_t *lifo, slist_node_t *node)
{
    return slist_del_node(&lifo->list, node);
}


/*********************************************************
 *@brief: 
 ***safely insert the node to the LIFO queue
 *
 *@contract:
 ***1. "node" is the node in the LIFO queue
 ***2. next_node is the deleted node
 *
 *@parameter:
 *[node]: the node of the operation
 *[next_node]: the next node to insert
 *[safe_node]: safe node used during traversal
 *********************************************************/
/*********************************************************
 *@简要：
 ***从后进先出队列中安全插入节点
 *
 *@约定：
 ***1、node为后进先出队列中的节点
 ***2、next_node为已删除的节点
 *
 *@参数：
 *[node]: 操作的节点
 *[next_node]: 要插入的下一个节点
 *[safe_node]: 遍历过程中使用的安全节点
 **********************************************************/
static inline void lifo_node_insert_next_safe(slist_node_t *node, slist_node_t *next_node, slist_node_t **safe_node)
{
    slist_node_insert_next_safe(node, next_node, safe_node);
}


/*********************************************************
 *@brief: 
 ***safely remove the next node from the LIFO queue
 *
 *@contract:
 ***1. "node" is the node in the LIFO queue
 ***2. "node" is not the fifo tail node
 *
 *@parameter:
 *[node]: the node of the operation
 *[safe_node]: safe node used during traversal
 *
 *@return value: the next node to be removed
 *********************************************************/
/*********************************************************
 *@简要：
 ***从后进先出队列中安全移除下一个节点
 *
 *@约定：
 ***1、node为后进先出队列中的节点
 ***2、node不是尾节点
 *
 *@参数：
 *[node]: 操作的节点
 *[safe_node]: 遍历过程中使用的安全节点
 *
 *@返回值: 被移除的下一个节点
 **********************************************************/
static inline slist_node_t *lifo_node_del_next_safe(slist_node_t *node, slist_node_t **safe_node)
{
    return slist_node_del_next_safe(node, safe_node);
}

/*********************************************************
 *@brief: 
 ***safely remove the node from the LIFO queue
 *
 *@parameter:
 *[lifo]: LIFO queue
 *[del_node]: removed node
 *[safe_node]: safe node used during traversal
 *
 *@return value:
 *[true]: successfully removed this node from the queue
 *[false]: this node is not in this queue
 *********************************************************/
/*********************************************************
 *@简要：
 ***从后进先出队列中安全移除节点
 *
 *@参数：
 *[lifo]：后进先出队列
 *[del_node]: 被移除的节点
 *[safe_node]: 遍历过程中使用的安全节点
 *
 *@返回值:
 *[true]: 成功从后进先出队列中移除节点
 *[false]: 当前节点不在后进先出队列之中
 **********************************************************/
static inline bool lifo_del_node_safe(lifo_t *lifo, slist_node_t *del_node, slist_node_t **safe_node)
{
    return slist_del_node_safe(LIFO_LIST(lifo), del_node, safe_node);
}


/*********************************************************
 *@brief: 
 ***transfer all nodes of the LIFO queue to the head of the receive queue,
 ***after the transfer is completed, the original LIFO queue becomes empty
 *
 *@parameter:
 *[lifo]: LIFO queue that was transferred
 *[recv_lifo]: LIFO queue for receiving nodes
 *********************************************************/
/*********************************************************
 *@简要：
 ***将后进先出队列所有节点转移至接收队列的头部，
 ***转移完成后，原后进先出队列变成空
 *
 *@参数：
 *[lifo]：被转移的后进先出队列
 *[recv_lifo]: 接收节点的后进先出队列
 **********************************************************/
static inline void lifo_nodes_transfer_to(lifo_t *lifo, lifo_t *recv_lifo)
{
    slist_nodes_transfer_to(LIFO_LIST(lifo), LIFO_LIST(recv_lifo));
}

#endif /* __LIFO_H__ */
