
#pragma once
#include <stdbool.h>
#include <stdio.h>

// Define the structure for

// Define the linked map structure

// Define the structure for file lock encapsulation
typedef struct ListObj *List;
List new_list(void);

void file_lock_new(List L, char *uri);

// Function to free memory and delete the file lock
void file_lock_delete(List L);

// Function to acquire a read lock for a specific URI
void file_lock_read_lock(List filelock, char *uri);

// Function to release a read lock for a specific URI
void file_lock_read_unlock(List filelock, char *uri);

// Function to acquire a write lock for a specific URI
void file_lock_write_lock(List filelock, char *uri);

// Function to release a write lock for a specific URI
void file_lock_write_unlock(List filelock, char *uri);
