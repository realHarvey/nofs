/*
 *  include / list.h
 *  Copyright(c) 2023  Harvey
 */
#pragma once

/*
 * 根据结构体成员变量member的基地址
 * 计算出该结构体变量的基地址
 *
 * type   该结构体类型
 * ptr    指向member的指针
 * member 成员变量名
 *
 * 对0地址处读number, 并取得number的地址
 * 即为number相对于此struct的偏移
 * 减去此偏移即可得到struct的基地址
 */
#define container_of(type, ptr, number) \
((type *)( (void *)ptr - (unsigned long)&((type *)0)->number ))

/*
 * head + nodes
 */
struct klist {
        struct klist * next, * prev;
};

/*
 * LIST_INIT : 新建 & 初始化节点
 * list_init : 初始化 | 重置已建立的节点
 */
#define LIST_INIT(head) \
struct klist head = { &(head), &(head) }

static inline void list_init(struct klist * node)
{
        node->next = node->prev = node;
}

/*
 * list_add : 紧邻头节点向后添加
 *      head -> second_node -> first_node -> head
 * list_add_tail : 紧邻头节点向前添加
 */
static inline void list_add(struct klist * new, struct klist * head)
{
        head->next->prev = new;
        new->next = head->next;
        new->prev = head;
        head->next = new;
}

static inline void list_add_tail(struct klist * new, struct klist * head)
{
        head->prev->next = new;
        new->prev = head->prev;
        new->next = head;
        head->prev = new;
}

/*
 * 删除node并重新初始化它(安全删除)
 * 被删除之后 再次加入无需初始化
 */
static inline void list_del(struct klist * node)
{
        node->prev->next = node->next;
        node->next->prev = node->prev;

        node->prev = node->next = node;
}

static inline int list_is_empty(struct klist * head)
{
        return head->next == head;
}

/*
 * 遍历内嵌klist的所有节点
 *
 * head(klist *)     : 链表head
 * iter(container *) : 迭代器指针
 * member(klist)     : 该container内的klist成员名
 */
#define list_for_each_container(head, iter, member)             \
for (                                                           \
  iter = container_of(typeof(*iter), (head)->next, member);     \
  &(iter->member) != head;                                      \
  iter = container_of(typeof(*iter), iter->member.next, member) \
)

