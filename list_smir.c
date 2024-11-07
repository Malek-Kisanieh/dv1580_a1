#include <stdio.h>
#include <stdint.h>
#include "memory_manager.h"

// Definition of a singly linked list node.
typedef struct Node {
    uint16_t data;        // The data stored in the node.
    struct Node* next;    // Pointer to the next node in the list.
} Node;

// Initializes a linked list and the custom memory manager.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// - size: Size of the memory pool to be initialized.
void list_init(Node** head, size_t size) {
    *head = NULL;
    mem_init(size);
}

// Inserts a new node at the end of the list.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// - data: The data to be inserted into the new node.
// Errors:
// - Prints an error message if memory allocation fails.
void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Inserts a new node immediately after a given node.
// Parameters:
// - prev_node: The node after which the new node should be inserted.
// - data: The data to be inserted into the new node.
// Errors:
// - Prints an error if the previous node is NULL.
// - Prints an error message if memory allocation fails.
void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Previous node cannot be NULL\n");
        return;
    }

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

// Inserts a new node before a given node.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// - next_node: The node before which the new node should be inserted.
// - data: The data to be inserted into the new node.
// Errors:
// - Prints an error if the next_node is NULL.
// - Prints an error if the specified next node is not found in the list.
// - Prints an error message if memory allocation fails.
void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (next_node == NULL) {
        printf("Next node cannot be NULL\n");
        return;
    }

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;

    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    Node* current = *head;
    while (current != NULL && current->next != next_node) {
        current = current->next;
    }

    if (current == NULL) {
        printf("The specified next node is not in the list\n");
        mem_free(new_node);
        return;
    }

    new_node->next = next_node;
    current->next = new_node;
}

// Deletes the first node with the specified data.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// - data: The data of the node to be deleted.
// Errors:
// - Prints an error if the list is empty.
// - Prints an error if the data is not found in the list.
void list_delete(Node** head, uint16_t data) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }

    Node* current = *head;
    Node* previous = NULL;

    while (current != NULL && current->data != data) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Data not found in the list\n");
        return;
    }

    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }

    mem_free(current);
}

// Searches for a node with the specified data.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// - data: The data to search for.
// Returns:
// - A pointer to the node containing the data if found, otherwise NULL.
Node* list_search(Node** head, uint16_t data) {
    Node* current = *head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Displays all elements in the list.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
void list_display(Node** head) {
    Node* current = *head;
    printf("[");
    while (current != NULL) {
        printf("%u", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");
}

// Displays elements between two nodes (inclusive).
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// - start_node: The starting node (inclusive). If NULL, starts from the head.
// - end_node: The ending node (inclusive). If NULL, goes until the end.
void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* current = start_node ? start_node : *head;

    printf("[");
    while (current != NULL && (end_node == NULL || current != end_node->next)) {
        printf("%d", current->data);
        if (current->next != NULL && current != end_node) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");
}

// Counts the number of nodes in the list.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
// Returns:
// - The total number of nodes in the list.
int list_count_nodes(Node** head) {
    int count = 0;
    Node* current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Frees all nodes in the list and deinitializes the memory manager.
// Parameters:
// - head: Pointer to the head pointer of the linked list.
void list_cleanup(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* next_node = current->next;
        mem_free(current);
        current = next_node;
    }
    *head = NULL;
    mem_deinit();
}