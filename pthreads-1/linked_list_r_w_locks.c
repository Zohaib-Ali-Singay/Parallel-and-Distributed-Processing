#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


struct Node
{
    int data;
    struct Node* next;
    //pthread_rwlock_t rw_lock;
} typedef node;

pthread_rwlock_t rw_lock;
struct Node* create_node(int value)
{
    node *new_node = (struct Node*)malloc(sizeof(node));

    new_node->data = value;
    new_node->next = NULL;
    //pthread_rwlock_init(&(new_node->rw_lock), NULL);

    return new_node;
}


void insert_at_end(struct Node** head, int value)
{
    node *new_node = create_node(value);
    
    if (*head == NULL)
    {
        *head = new_node;
        return;
    }

    node *temp = *head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = new_node;
}

// Step 4: Print the linked list
void print_list(node *head)
{
    struct Node* temp = head;
    printf("Linked list: ");
    while (temp != NULL)
    {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

void delete_list(node *head)
{
    node *curr = head;

    while(curr!=NULL)
    {
        node *temp_p;
        temp_p = curr;
        curr = curr->next;
        free(temp_p);
    }

}

void *delete_node(void *rank);
void *member(void *rank);
node *head = NULL;
long thread_count;
long find_value, delete_value;
// Step 5: Main function
int main(int argc, char *argv[])
{
    //long thread, *found = malloc(sizeof(long));
    thread_count = strtol(argv[1], NULL, 10);
    find_value = strtol(argv[2], NULL, 10);
    delete_value = strtol(argv[3], NULL, 10);
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_rwlock_init(&rw_lock, NULL);

    for (int i = 1; i <= thread_count; i++)
        insert_at_end(&head, i);

    print_list(head);

    /*for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, member, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread],(void **) &found);
    }

    if (*(long *)found == 0)
        printf("Not found\n");

    else
        printf("Found\n");
    */

    for (long t = 0; t < thread_count; t++) {
        pthread_create(&thread_handles[t], NULL, delete_node, (void *)t);
    }

    // collect their results
    for (long t = 0; t < thread_count; t++) {
        long *deleted = malloc(sizeof(long));
        pthread_join(thread_handles[t], (void **)&deleted);
        if (*deleted)
            printf("Thread %ld: deletion succeeded\n", t);
        else
            printf("Thread %ld: value not found\n", t);
        free(deleted);
    }

    printf("b\n");
    delete_node(head);
    printf("a\n");
    print_list(head);


    //printf("List deleted sucessfully\n");
    return 0;
}

void *member(void * rank)
{
    node *curr = head;
    long *found = malloc(sizeof(long));

    pthread_rwlock_rdlock(&rw_lock);

    while(curr->next != NULL && curr->data < find_value)
    {
        curr = curr->next;
    }

    if(curr->data == find_value)
        *found = 1;

    else
        *found = 0;

    pthread_rwlock_rdlock(&rw_lock);

    return found;
}

void *delete_node(void *rank)
{
    node *curr = head;
    long *found = malloc(sizeof(long));
    pthread_rwlock_wrlock(&rw_lock);
    while(curr != NULL)
    {
        if(curr->data == delete_value)
        {
            printf("Element to delete is present.\n");
            break;
        }

        curr = curr->next;
    }

    if (curr == NULL)
    {
        *found = 0;
        pthread_rwlock_unlock(&rw_lock);
        return found;
    }

    curr = head;

    if(head->data == delete_value)
    {
        node *temp = head;
        head = head->next;
        free(temp);
    }

    else
    {
        while(curr != NULL && curr->next->data != delete_value)
            curr = curr->next;

        node *delete_node = curr->next;
        curr->next = curr->next->next;

        free(delete_node);
    }

    *found = 1;

    pthread_rwlock_unlock(&rw_lock);

    return found;
}

