#ifndef RBTREE
#define RBTREE
#define RED 0 
#define BLACK 1
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct {
	int key;
	void *content;
}data;
typedef struct _rb_node{
	data *ndata;
	int color;
	struct _rb_node *parent;
	struct _rb_node *left;
	struct _rb_node *right;
}rb_node;
typedef struct {
	rb_node *rb_root;
	size_t size;
}rb_tree;
typedef void (*tra_fun)(data *datapt,void **arg,int size);
void initializetree(rb_tree *ptree) ;
#define tree_is_empty(x) ((x)->rb_root?0:1)
#define treecount(x) ((x)->size)
void traversal_fun(data *datapt,void **arg,int size);
int insertree(rb_tree *ptree,data *datapt);
void traversaltree(rb_tree *ptree,tra_fun fun,void **arg,int size);
data *searchdata(rb_tree *ptree,int key);
data *makedata(int key,void *content,size_t size);
#endif
