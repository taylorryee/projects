#include "rwlock.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Filelock.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
typedef struct NodeObj *Node;
typedef struct NodeObj {
    char *key;
    rwlock_t *loc;
    Node next;
} NodeObj;

typedef struct ListObj {
    Node head;
    pthread_mutex_t lock;
    // Head of the linked list
} ListObj;

Node new (char *uri) {
    Node new = malloc(sizeof(NodeObj));
    assert(new != NULL);
    new->key = uri;
    new->loc = rwlock_new(2, 1);
    new->next = NULL;
    return new;
}
List new_list(void) {
    List new = malloc(sizeof(ListObj));
    assert(new != NULL);
    new->head = NULL;
    int rc = pthread_mutex_init(&(new->lock), NULL);
    if (rc != 0) {

        pthread_mutex_destroy(&(new->lock));
        free(new);
        return NULL;
    }

    return new;
}

void file_lock_new(List L, char *uri) {
    pthread_mutex_lock(&(L->lock));
    Node temp = L->head;
    while (temp != NULL) {
        if (strcmp(temp->key, uri) == 0) {
            pthread_mutex_unlock(&(L->lock));
            return;
        }
        temp = temp->next;
    }
    Node insert = new (uri); // Create a new node with the provided key

    // If the list is empty, set the new node as the head
    if (L->head == NULL) {
        L->head = insert;
        pthread_mutex_unlock(&(L->lock));
        return;
    }

    // Traverse the list to find the last node
    Node current = L->head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Append the new node at the end of the list
    current->next = insert;
    pthread_mutex_unlock(&(L->lock));
}

void file_lock_delete(List filelock) {
    pthread_mutex_lock(&(filelock->lock));
    Node current = filelock->head;
    while (current != NULL) {
        Node temp = current;
        current = current->next;
        rwlock_delete(&(temp->loc)); // Utilizing rwlock_delete with double pointer
        free(temp->key);
        free(temp);
    }
    filelock->head = NULL; // Reset the list head after deleting all nodes
    pthread_mutex_unlock(&(filelock->lock));
    pthread_mutex_destroy(&(filelock->lock)); // Destroy the mutex lock
    free(filelock); // Free the list structure itself
}

// Function to acquire a read lock for a specific URI
/*void file_lock_read_lock(List L, char* uri)
{
   pthread_mutex_lock(&(L->lock));
    Node temp=L->head;
    while(temp!=NULL)
    {
        if(strcmp(temp->key, uri) == 0)
        {
            break;
        }
        
        temp=temp->next;
        if(temp==NULL)
        {
            pthread_mutex_unlock(&(L->lock));
            return;
        }
    }
    reader_lock(temp->loc);
    
    pthread_mutex_unlock(&(L->lock));
    return;

}*/

void file_lock_read_lock(List L, char *uri) {
    pthread_mutex_lock(&(L->lock));

    Node temp = L->head;
    bool found = false; // Flag to track whether the condition is met

    while (temp != NULL) {
        if (strcmp(temp->key, uri) == 0) {
            found = true;
            break; // Break out of the loop if condition is met
        }
        temp = temp->next;
    }

    if (!found) {
        pthread_mutex_unlock(&(L->lock));
        fprintf(stdout, "READ LOCK\n");
        fflush(stdout);
        return;
    }

    reader_lock(temp->loc);
    fprintf(stdout, "READ LOCK\n");
    fflush(stdout);
    pthread_mutex_unlock(&(L->lock));

    return;
}

// Function to release a read lock for a specific URI
void file_lock_read_unlock(List L, char *uri) {
    pthread_mutex_lock(&(L->lock));
    Node temp = L->head;
    bool found = false; // Flag to track whether the condition is met

    while (temp != NULL) {
        if (strcmp(temp->key, uri) == 0) {
            found = true;
            break; // Break out of the loop if condition is met
        }
        temp = temp->next;
    }

    if (!found) {
        pthread_mutex_unlock(&(L->lock));
        fprintf(stdout, "READ UNLOCK\n");
        fflush(stdout);
        return;
    }

    reader_unlock(temp->loc);
    fprintf(stdout, "READ UNLOCK\n");
    fflush(stdout);
    pthread_mutex_unlock(&(L->lock));
    return;
}

// Function to acquire a write lock for a specific URI
/*void file_lock_write_lock(List L, char* uri)
{
    pthread_mutex_lock(&(L->lock));
    Node temp=L->head;
    while(temp!=NULL)
    {
        if(strcmp(temp->key, uri) == 0)
        {
            break;
        }
        temp=temp->next;
        if(temp==NULL)
        {
           pthread_mutex_unlock(&(L->lock));
            return;
        }
    }
    writer_lock(temp->loc);
    pthread_mutex_unlock(&(L->lock));
    return;
}*/

void file_lock_write_lock(List L, char *uri) {
    pthread_mutex_lock(&(L->lock));
    Node temp = L->head;
    bool found = false; // Flag to track whether the condition is met

    while (temp != NULL) {
        if (strcmp(temp->key, uri) == 0) {
            found = true;
            break; // Break out of the loop if condition is met
        }
        temp = temp->next;
    }

    if (!found) {
        pthread_mutex_unlock(&(L->lock));
        fprintf(stdout, "WRITE LOCK\n");
        fflush(stdout);
        return;
    }

    writer_lock(temp->loc);
    fprintf(stdout, "WRITE LOCK\n");
    fflush(stdout);
    pthread_mutex_unlock(&(L->lock));
    return;
}

// Function to release a write lock for a specific URI
/*void file_lock_write_unlock(List L, char* uri)
{
   pthread_mutex_lock(&(L->lock));
    Node temp=L->head;
    while(temp!=NULL)
    {
        if(strcmp(temp->key, uri) == 0)
        {
            break;
        }
        temp=temp->next;
        if(temp==NULL)
        {
            pthread_mutex_unlock(&(L->lock));
            return;
        }
    }
    writer_unlock(temp->loc);
    pthread_mutex_unlock(&(L->lock));
    return;
}*/

void file_lock_write_unlock(List L, char *uri) {
    pthread_mutex_lock(&(L->lock));
    Node temp = L->head;
    bool found = false; // Flag to track whether the condition is met

    while (temp != NULL) {
        if (strcmp(temp->key, uri) == 0) {
            found = true;
            break; // Break out of the loop if condition is met
        }
        temp = temp->next;
    }

    if (!found) {
        pthread_mutex_unlock(&(L->lock));
        fprintf(stdout, "WRITE UNLOCK\n");
        fflush(stdout);
        return;
    }

    writer_unlock(temp->loc);
    fprintf(stdout, "WRITE UNLOCK\n");
    fflush(stdout);
    pthread_mutex_unlock(&(L->lock));
    return;
}
