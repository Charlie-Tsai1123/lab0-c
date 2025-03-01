#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *queue = malloc(sizeof(struct list_head));
    if (!queue)
        return NULL;
    INIT_LIST_HEAD(queue);
    return queue;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *first = head->next;
    element_t *element = list_entry(first, element_t, list);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(first);
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *last = head->prev;
    element_t *element = list_entry(last, element_t, list);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(last);
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int size = 0;
    struct list_head *node = NULL;
    list_for_each (node, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *fast = head, *slow = head;
    while (fast->next != head && fast->next->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    element_t *element = list_entry(slow->next, element_t, list);
    list_del(slow->next);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;

    struct list_head *current = head->next;
    while (current != head) {
        element_t *current_element = list_entry(current, element_t, list);
        struct list_head *next = current->next;
        bool isRepeat = false;
        while (next != head) {
            element_t *next_element = list_entry(next, element_t, list);
            if (strcmp(current_element->value, next_element->value) == 0) {
                isRepeat = true;
                next = next->next;
                list_del(next->prev);
                q_release_element(next_element);
            } else {
                break;
            }
        }
        current = current->next;
        if (isRepeat) {
            list_del(current->prev);
            q_release_element(current_element);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *current = head;
    while (current->next != head && current->next->next != head) {
        struct list_head *move = current->next;
        list_del(move);
        list_add(move, current->next);
        current = move;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *tail = head;
    while (head->next != tail) {
        list_move_tail(head->next, tail);
        tail = tail->prev;
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (k == 1 || !head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *prev = head, *current = head->next;
    int itGroup = k - 1, counter = q_size(head) / k;
    while (counter != 0) {
        itGroup--;
        list_move(current->next, prev);
        if (itGroup == 0) {
            itGroup = k - 1;
            counter--;
            prev = current;
            current = current->next;
        }
    }
    return;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    q_reverse(head);
    struct list_head *current = head->next;
    while (current->next != head) {
        const element_t *current_element = list_entry(current, element_t, list);
        element_t *next_element = list_entry(current->next, element_t, list);
        if (strcmp(current_element->value, next_element->value) < 0) {
            list_del(current->next);
            q_release_element(next_element);
        } else {
            current = current->next;
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    q_reverse(head);
    struct list_head *current = head->next;
    while (current->next != head) {
        const element_t *current_element = list_entry(current, element_t, list);
        element_t *next_element = list_entry(current->next, element_t, list);
        if (strcmp(current_element->value, next_element->value) > 0) {
            list_del(current->next);
            q_release_element(next_element);
        } else {
            current = current->next;
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
