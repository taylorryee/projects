#include "List.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct NodeObj *Node;
typedef struct NodeObj {
    char *val;
    int rbit;
    Node next;
    Node prev;
} NodeObj;

typedef struct ListObj {
    Node head;
    Node back;
    int co;
    int ca;
    char *cnext;
    int index;
    int size;
    int cur_size;
} ListObj;

Node newNode(char *vall, int rbitt) {
    Node new = malloc(sizeof(NodeObj));
    assert(new != NULL);
    new->val = vall;
    new->rbit = rbitt;
    new->next = NULL;
    new->prev = NULL;

    return new;
}

List new_list(int sizee) {
    List new = malloc(sizeof(ListObj));
    assert(new != NULL);
    new->head = NULL;
    new->back = NULL;
    new->co = 0;
    new->ca = 0;
    new->cnext = NULL;
    new->size = sizee;
    new->cur_size = 0;
    new->index = 0;
    return new;
}

int co_count(List L) {
    assert(L != NULL);
    return L->co;
}

int ca_count(List L) {
    assert(L != NULL);
    return L->ca;
}

int lsize(List L) {
    return L->cur_size;
}

//returns 0 if miss, 1 if hit
int f_add(List L, char *item) {
    assert(L != NULL);
    Node insert = newNode(item, -1);
    bool flag = false;
    if (L->head == NULL) {
        L->head = insert;
        L->back = insert;
        L->co++;
        L->cur_size++;
        return 0;
    }

    Node temp = L->head;
    while (temp != NULL) {
        if (strcmp(temp->val, item) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    Node seen_temp = L->head->prev;
    while (seen_temp != NULL) {
        if (strcmp(seen_temp->val, item) == 0) {
            flag = true;
            L->ca++;
        }
        seen_temp = seen_temp->prev;
    }
    if (!flag) {
        L->co++;
    }
    L->back->next = insert;
    insert->prev = L->back;
    L->back = insert;
    if (L->cur_size < L->size) {
        L->cur_size++;
        //return 0;
    } else if (L->cur_size == L->size) {
        L->head = L->head->next;
        //return 0;
    }

    return 0;
}

int l_add(List L, char *item) {
    assert(L != NULL);
    bool flag = false;
    Node insert = newNode(item, -1);
    if (L->head == NULL) {
        L->head = insert;
        L->back = insert;
        L->co++;
        L->cur_size++;
        return 0;
    }
    Node temp = L->head;
    while (temp != NULL) {
        if (strcmp(temp->val, item) == 0) {
            //fprintf(stdout,"GET HERE\n");
            if (temp != L->back) {
                if (temp != L->head) {
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                    L->back->next = temp;
                    temp->prev = L->back;
                    temp->next = NULL;
                    L->back = temp;
                } else {
                    if (L->head->prev == NULL) {
                        L->back->next = temp;
                        temp->prev = L->back;
                        L->head = temp->next;
                        L->head->prev = NULL;
                        temp->next = NULL;
                        L->back = temp;

                    } else {
                        L->back->next = temp;
                        temp->next->prev = temp->prev;
                        temp->prev->next = temp->next;
                        L->head = temp->next;
                        temp->prev = L->back;
                        temp->next = NULL;
                        L->back = temp;
                    }
                }
            }
            return 1;
        }
        temp = temp->next;
    }
    Node seen_temp = L->head->prev;

    while (seen_temp != NULL) {
        if (strcmp(seen_temp->val, item) == 0) {
            flag = true;
            L->ca++;
        }
        seen_temp = seen_temp->prev;
    }
    if (!flag) {
        //fprintf(stdout,"SHOULD GET HERE\n");
        L->co++;
    }
    L->back->next = insert;
    insert->prev = L->back;
    L->back = insert;
    if (L->cur_size < L->size) {
        L->cur_size++;
    } else if (L->cur_size == L->size) {
        L->head = L->head->next;
    }

    return 0;
}

void printList(List L) {
    Node i = L->head;
    while (i != NULL) {
        fprintf(stdout, "listval:%s rbit:%d\n", i->val, i->rbit);
        i = i->next;
    }
}

//1 for hit, 0 for miss
int c_add(List L, char *item) {
    Node insert = newNode(item, 0);
    bool flag = false;
    if (L->head == NULL) {
        L->head = insert;
        L->back = insert;
        L->co++;
        L->cur_size++;
        L->cnext = item;
        return 0;
    }
    Node temp = L->head;
    while (temp != NULL) {
        if (strcmp(temp->val, item) == 0) {
            if (temp->rbit == 0) {
                temp->rbit = 1;
            }
            return 1;
        }
        temp = temp->next;
    }
    Node seen_temp = L->head->prev;
    while (seen_temp != NULL) {
        if (strcmp(seen_temp->val, item) == 0) {
            flag = true;
            L->ca++;
        }
        seen_temp = seen_temp->prev;
    }
    if (L->cur_size < L->size) {
        L->back->next = insert;
        insert->prev = L->back;
        L->back = insert;
        L->cur_size++;
    } else if (L->cur_size == L->size) {

        Node temp = L->head;
        while (temp != NULL) {
            if (strcmp(temp->val, L->cnext) == 0) {

                if (temp->rbit == 0) {
                    if (temp == L->head) {
                        if (L->head->prev == NULL) {

                            Node insert2 = temp;
                            if (L->head->next == NULL) {
                                L->head = insert;
                                L->head->prev = insert2;
                                insert2->next = L->head;
                                L->cnext = L->head->val;

                            } else {
                                L->head->next->prev = insert;
                                insert->next = L->head->next;
                                insert->prev = L->head;
                                L->head->next = insert;
                                L->head = insert;
                                L->cnext = L->head->next->val;
                            }

                        } else {
                            Node insert2 = temp;
                            if (L->head->next == NULL) {
                                L->head->prev->next = insert;
                                insert->prev = L->head->prev;
                                L->head->prev = NULL;
                                L->head = insert;
                                L->cnext = L->head->val;
                            } else {
                                L->head->prev->next = insert;
                                insert->prev = L->head->prev;
                                insert->next = L->head->next;
                                L->head->prev = insert;
                                L->head->prev = NULL;
                                L->head = insert;
                                L->cnext = L->head->next->val;
                            }
                            if (flag == false) {

                                Node temp3 = L->head->prev;
                                while (temp3->prev != NULL) {
                                    temp3 = temp3->prev;
                                }
                                temp3->prev = insert2;
                            }
                        }
                    } else if (temp == L->back) {
                        Node insert2 = temp;
                        L->back->prev->next = insert;
                        insert->prev = L->back->prev;
                        L->back->prev = NULL;
                        L->back = insert;
                        L->cnext = L->head->val;
                        if (flag == false) {
                            Node temp3 = L->head;
                            while (temp3->prev != NULL) {
                                temp3 = temp3->prev;
                            }
                            temp3->prev = insert2;
                        }
                    } else {
                        Node insert2 = temp;
                        insert->prev = temp->prev;
                        insert->next = temp->next;
                        temp->prev->next = insert;
                        temp->next->prev = insert;
                        temp->prev = NULL;
                        temp->next = NULL;
                        L->cnext = insert->next->val;
                        if (flag == false) {
                            Node temp3 = L->head;
                            while (temp3->prev != NULL) {
                                temp3 = temp3->prev;
                            }
                            temp3->prev = insert2;
                        }
                    }
                    if (!flag) {
                        L->co++;
                    }

                    return 0;
                } else if (temp->rbit == 1) {

                    temp->rbit = 0;
                    if (strcmp(temp->val, L->back->val) == 0) {
                        L->cnext = L->head->val;
                    } else {

                        L->cnext = temp->next->val;
                    }
                }
            }
            if (temp == L->back) {
                temp = L->head;
            } else {
                temp = temp->next;
            }
            //temp=temp->next;
        }
    }
    if (!flag) {
        L->co++;
    }

    return 0;
}

