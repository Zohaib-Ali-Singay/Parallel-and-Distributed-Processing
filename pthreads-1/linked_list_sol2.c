#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


struct Node
{
    int data;
    struct Node* next;
    pthread_mutex_t mutex;
} typedef node;


struct Node* create_node(int value)
{
    node *new_node = (struct Node*)malloc(sizeof(node));

    new_node->data = value;
    new_node->next = NULL;
    pthread_mutex_init(&new_node->mutex, NULL);

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

void delete(node *head)
{
    node *curr = head;

    while(curr!=NULL)
    {
        node *temp_p;
        temp_p = curr;
        pthread_mutex_destroy(&(temp_p->mutex));
        curr = curr->next;
        free(temp_p);
    }

}

void *member(void *rank);
node *head = NULL;
long thread_count;
long value;
// Step 5: Main function
int main(int argc, char *argv[])
{
    long thread, *found = malloc(sizeof(long));
    thread_count = strtol(argv[1], NULL, 10);
    value = strtol(argv[2], NULL, 10);
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

    for (int i = 1; i <= thread_count; i++)
        insert_at_end(&head, i);

    print_list(head);

    for (thread = 0; thread < thread_count; thread++)
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

    
    delete(head);

    printf("List deleted sucessfully\n");
    return 0;
}

void *member(void * rank)
{
    node *curr = head;
    long *found = malloc(sizeof(long));
    /*pthread_mutex_lock(&head->mutex);
    //This method gives errors.
    temp_p = head;

    while(temp_p != NULL && temp_p -> data < value)
    {
        if (temp_p -> next != NULL)
            pthread_mutex_lock(&(temp_p -> next -> mutex));

        if (temp_p ==  head)
            pthread_mutex_unlock(&(head -> mutex));

        else
            pthread_mutex_unlock(&(temp_p -> mutex));

        temp_p = temp_p -> next;
    }

    if (temp_p == NULL || temp_p -> data > value)
    {
        if (temp_p == head)
            pthread_mutex_unlock(&(head -> mutex));

        else
            pthread_mutex_unlock(&(temp_p -> mutex));
        
        *a = 0;
        return a;
    }

    else
    {
        if (temp_p == head)
            pthread_mutex_unlock(&(head -> mutex));

        else
            pthread_mutex_unlock(&(temp_p -> mutex));
        
        *a = 1;
        return a;
    }*/

    pthread_mutex_lock(&(curr->mutex));

    while(curr->next != NULL && curr->data < value)
    {
        pthread_mutex_lock(&(curr->next->mutex));
        pthread_mutex_unlock(&(curr->mutex));

        curr = curr->next;
    }

    if(curr->data == value)
        *found = 1;

    else
        *found = 0;

    pthread_mutex_unlock(&(curr->mutex));

    return found;
}
