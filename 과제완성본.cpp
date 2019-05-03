#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "lab2_sync_types.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/*
void order(lab2_node *node)
{
    if (node->left != NULL) order(node->left);
    printf("%d, ", node->key);
    if (node->right != NULL) order(node->right);

}
*/
int lab2_node_print_inorder(lab2_tree *tree) {
    // You need to implement lab2_node_print_inorder function.
    lab2_node *node = tree->root;
    if (tree != NULL) {
        lab2_node_print_inorder(tree->left);
	    printf("%d, ", node->key);
	    lab2_node_print_inorder(tree->right);
	    
        return LAB2_SUCCESS;
    }
    else {
        return LAB2_ERROR;
    }
}
lab2_tree *lab2_tree_create() {                                 
    // You need to implement lab2_tree_create function.
    lab2_tree *root = malloc(sizeof(lab2_tree));
    return root;
}
lab2_node * lab2_node_create(int key) {                    
    // You need to implement lab2_node_create function.
    lab2_node *root = malloc(sizeof(lab2_node));
    root->key = key;
    root->left = NULL;
    root->right = NULL;
    pthread_mutex_init(&root->mutex, NULL);
    return root;

}
int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){
    lab2_node *p = tree->root;
    lab2_node *q = NULL;	// 임시저장소 
    int element = new_node->key;

    while (p != NULL) {
        q = p;
        if (p->key > element)
            p = p->left;
        else if (p->key < element)
            p = p->right;
        else
            return LAB2_ERROR;
    }

     if (tree->root != NULL){
        if (q->key > element)
            q->left = new_node;
        else
            q->right = new_node;
    }
    else
        tree->root = new_node;
    return LAB2_SUCCESS;
}
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    int element = new_node->key;

    while (p != NULL){
        q = p;
        if (p->key > element)
            p = p->left;
        else if (p->key < element)
            p = p->right;
        else
            return LAB2_ERROR;
    }
    if (tree->root != NULL) {
        if (q->key == element)
            return LAB2_ERROR;

        pthread_mutex_lock(&q->mutex);
        if (q->key > element)
            q->left = new_node;
        else
            q->right = new_node;
        pthread_mutex_unlock(&q->mutex);
    }
    else {
        pthread_mutex_lock(&new_node->mutex);
        tree->root = new_node;
        pthread_mutex_unlock(&new_node->mutex);
    }
    return LAB2_SUCCESS;
}
int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    pthread_mutex_lock(&mutex);
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    int element = new_node->key;

    while (p != NULL) {
        q = p;
        if (p->key > element)
            p = p->left;
        else if (p->key < element)
            p = p->right;
        else
            pthread_mutex_unlock(&mutex);
            return LAB2_ERROR;
    }
    if (tree->root != NULL) {
        if (q->key > element)
            q->left = new_node;
        else
            q->right = new_node;
    }
    else
        tree->root = new_node;
    pthread_mutex_unlock(&mutex);
    return LAB2_SUCCESS;
}
int lab2_node_remove(lab2_tree *tree, int key) {
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *root = tree->root;

    while (p != NULL && p->key != key){
        q = p;
        if (p->key > key)
            p = p->left;
        else
            p = p->right;
    }

    if (p == NULL)
    	return LAB2_ERROR;

    int element = p->key;

    lab2_node *c;

    if (p->left == NULL)
        c = p->right;
    else
        c = p->left;
        
    if (p == root)
        root = c;
    else {
        if (p == q->left)
            q->left = c;
        else
            q->right = c;
    }
	/* 자식이 둘 일때 */
    if (p->left != NULL && p->right != NULL) { 
        lab2_node *s = p->left;
        lab2_node *t = p;

        while (s->right != NULL) {
            t = s;
            s = s->right;
        }
        p->key = s->key;
        p = s;
        q = t;
    }
    return LAB2_SUCCESS;
}
int lab2_node_remove_fg(lab2_tree *tree, int key) {
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *root = tree->root;

    while (p != NULL && p->key != key) {
        q = p;
        if (p->key > key)
            p = p->left;
        else
            p = p->right;
    }

    if (p == NULL)
        return LAB2_ERROR;

    int element = p->key;
	/* 자식이 둘 일경우 */
    if (p->left != NULL && p->right != NULL) {
        pthread_mutex_lock(&p->mutex);
        lab2_node *s = p->left;
        lab2_node *t = p;
        while (s->right != NULL) {
            t = s;
            s = s->right;
        }
        pthread_mutex_unlock(&p->mutex);
        
        pthread_mutex_lock(&s->mutex);
        p->key = s->key;
        p = s;
        q = t;
        pthread_mutex_unlock(&s->mutex);
    }

    lab2_node *c;
	/* 자식이 하나 또는 없을 경우 */
    if (p->left == NULL)
        c = p->right;
    else
        c = p->left;
        
	pthread_mutex_lock(&p->mutex);
    if (p == root)
		root = c;
    else {
        if (p == q->left)
            q->left = c;
        else
            q->right = c;
    }
    pthread_mutex_unlock(&p->mutex);
    
    return LAB2_SUCCESS;
}

int lab2_node_remove_cg(lab2_tree *tree, int key) {
    pthread_mutex_lock(&mutex);
    lab2_node *p = tree->root;
    lab2_node *q = NULL;
    lab2_node *root = tree->root;

    while (p != NULL && p->key != key){
        q = p;
        if (p->key > key)
            p = p->left;
        else
            p = p->right;
    }

    if (p == NULL){
        pthread_mutex_unlock(&mutex);
        return LAB2_ERROR;
    }

    int element = p->key;

    if (p->left != NULL && p->right != NULL){
        lab2_node *s = p->left;
        lab2_node *t = p;
        while (s->right != NULL){
            t = s;
            s = s->right;
        }

        p->key = s->key;
        p = s;
        q = t;
    }

    lab2_node *c;

    if (p->left == NULL)
        c = p->right;
    else
        c = p->left;

    if (p == root)
        root = c;
    else {
        if (p == q->left)
            q->left = c;
        else
            q->right = c;
    }

    pthread_mutex_unlock(&mutex);
    return LAB2_SUCCESS;
}

void lab2_tree_delete(lab2_tree *tree) {
       free(tree);
}
void lab2_node_delete(lab2_node *node) {
       free(node);
}
