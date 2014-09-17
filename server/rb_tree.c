#include"rb_tree.h"
typedef struct{
	rb_node *parent;
	rb_node *child;
	rb_node **insertpt;
}search_result;
static rb_node *makenode(data *datapt);
static int toleft(rb_node *temp_nodept,int key);
static int toright(rb_node *temp_nodept,int key);
static search_result search_node(rb_tree *ptree,int key);
static void rb_rotate_left(rb_node *target,rb_tree *ptree);
static void rb_rotate_right(rb_node *target,rb_tree *ptree);
static void rb_insert_fixup(rb_node *target,rb_tree *ptree);
void traversal_fun(data *datapt,void **arg,int size)
{
	printf("%d\n",datapt->key);
}

void initializetree(rb_tree *ptree)
{
	ptree->rb_root=NULL;
	ptree->size=0;
}
data *makedata(int key,void *content,size_t size)
{
	data *ndata=malloc(sizeof(data));
	if(ndata==NULL)
		return NULL;
	ndata->key=key;
	ndata->content=malloc(size);
	if(ndata->content==NULL)
		return NULL;
	memcpy(ndata->content,content,size);
	return ndata;
}
static void rb_rotate_left(rb_node *target,rb_tree *ptree)
{
	rb_node *tright=target->right;
	rb_node *tparent=target->parent;
	if(target->right=tright->left)
		tright->left->parent=target;
	tright->left=target;
	target->parent=tright;
	tright->parent=tparent;
	if(tparent){
		if(tparent->left==target)
			tparent->left=tright;
		else if(tparent->right==target)
			tparent->right=tright;
	}else
		ptree->rb_root=tright;
}
static void rb_rotate_right(rb_node *target,rb_tree *ptree)
{
	rb_node *tparent=target->parent;
	rb_node *tleft=target->left;
	if(target->left=tleft->right)
		tleft->right->parent=target;
	tleft->right=target;
	target->parent=tleft;
	tleft->parent=tparent;
	if(tparent){
		if(tparent->left==target)
			tparent->left=tleft;
		else if(tparent->right==target)
			tparent->right=tleft;
	}else 
		ptree->rb_root=tleft;
}
void rb_tree_initializer(rb_tree *ptree)
{
	ptree->rb_root=NULL;
	ptree->size=0;
}
static rb_node *makenode(data *datapt)
{
	rb_node *new_nodept=(rb_node *)malloc(sizeof(rb_node));
	if(new_nodept==NULL)
		return NULL;
	new_nodept->left=new_nodept->right=NULL;
	new_nodept->color=RED;
	new_nodept->parent=NULL;
	new_nodept->ndata=datapt;
	return new_nodept;
}
static void rb_insert_fixup(rb_node *target,rb_tree *ptree)
{
	rb_node *tparent,*tgrandpa,*tuncle,*temp;
	tparent=target->parent;
	printf("111111111111111\n");
	while(tparent&&tparent->color==RED){  //短路求值的作用
		tgrandpa=tparent->parent;
		if(tgrandpa->left==tparent){
		printf("211111111111111\n");
			tuncle=tgrandpa->right;
			if(tuncle&&tuncle->color==RED){
				tgrandpa->color=RED;
				tparent->color=BLACK;
				tuncle->color=BLACK;
				target=tgrandpa;
			}else if(tparent->right==target){
				rb_rotate_left(tparent,ptree);
				temp=target;
				target=tparent;
				tparent=temp;
			}else{
				rb_rotate_right(tparent,ptree);
				tgrandpa->color=RED;
				tparent->color=BLACK;
			}
		}else{
	printf("311111111111111\n");
			tuncle=tgrandpa->left;
			if(tuncle&&tuncle->color==RED){
				tgrandpa->color=RED;
				tparent->color=BLACK;
				tuncle->color=BLACK;
				target=tgrandpa;
			}else if(tparent->left==target){
				rb_rotate_right(tparent,ptree);
				temp=target;
				target=tparent;
				tparent=temp;
			}else{
				rb_rotate_left(tparent,ptree);
				tparent->color=BLACK;
				tgrandpa->color=RED;
			}
		}
	}
	ptree->rb_root->color=BLACK;
}
int insertree(rb_tree *ptree,data *datapt)
{
	rb_node *new_nodept;
	search_result result;
	new_nodept=makenode(datapt);
	result=search_node(ptree,datapt->key);
	if(result.child==NULL&&result.insertpt!=NULL){
		new_nodept->parent=result.parent;
		*result.insertpt=new_nodept;
		++ptree->size;
		rb_insert_fixup(new_nodept,ptree);
	}
	else if(result.child!=NULL&&result.insertpt==NULL){
		fprintf(stderr,"exist equality data can not insert\n");
		return -1;
	}else{
		fprintf(stderr,"error in insertree\n");
		exit(1);
	}
	return 0;
}
data *search_data(rb_tree *ptree,int key)
{
	search_result result;
	result=search_node(ptree,key);
	if(result.child==NULL)
		return NULL;
	else
		return result.child->ndata;
}
static search_result search_node(rb_tree *ptree,int key)
{
	search_result result;
	result.parent=result.child=NULL;
	result.insertpt=NULL;
	if(ptree->rb_root==NULL){
		result.insertpt=&ptree->rb_root;
		return result;
	}
	result.child=ptree->rb_root;
	while(result.child!=NULL){
		if(toleft(result.child,key)){
			result.parent=result.child;
			result.insertpt=&result.child->left;
			result.child=result.child->left;
		}else if(toright(result.child,key)){
			result.parent=result.child;
			result.insertpt=&result.child->right;
			result.child=result.child->right;
		}else{
			result.insertpt=NULL;
			break;
		}
	}
	return result;
}
static int toleft(rb_node *temp_nodept,int key)
{
	if(temp_nodept->ndata->key<key)
		return 1;
	else
		return 0;
}
static int toright(rb_node *temp_nodept,int key)
{
	if(temp_nodept->ndata->key>key)
		return 1;
	else 
		return 0;
}
data *searchdata(rb_tree *ptree,int key)
{
	search_result result;
	result=search_node(ptree,key);
	if(result.child==NULL&&result.insertpt!=NULL)
		return NULL;
	else if(result.child!=NULL&&result.insertpt==NULL)
		return result.child->ndata;
	else{
		fprintf(stderr,"error in searchdata\n");
		exit(1);
	}
}
void traversaltree(rb_tree *ptree,tra_fun fun,void **arg,int size)
{
	if(ptree->rb_root==NULL)
		return ;
	int i=0;
	rb_node **nodebufpt,*nodept;
	if(fun==NULL)
		fun=traversal_fun;
	nodebufpt=(rb_node **)calloc(sizeof(rb_node *),ptree->size);
	if(nodebufpt==NULL){
		fprintf(stderr,"can not allocate memory to travesaltree\n");
		return ;
	}
	nodept=ptree->rb_root;
	while(nodept!=NULL||i!=-1){
		while(nodept!=NULL){
			nodebufpt[i]=nodept;
			nodept=nodept->left;
			++i;
		}
		if(--i!=-1){
			fun(nodebufpt[i]->ndata,arg,size);
			nodept=nodebufpt[i]->right;
		}
	}
	free(nodebufpt);
}
