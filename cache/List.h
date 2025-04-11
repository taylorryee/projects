#pragma once
#include <stdbool.h>
#include <stdio.h>

typedef struct ListObj *List;
List new_list(int sizee);

int ca_count(List L);

int co_count(List L);

int f_add(List L, char *item);

int l_add(List L, char *item);

int c_add(List L, char *item);

int lsize(List L);

void printList(List L);

