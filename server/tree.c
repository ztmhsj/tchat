#include"rb_tree.h"
struct ss{
	int i;
	double p;
};
struct Tt{
	int i;
	double b;
};
int main(void)
{
	struct Tt t1={1,9.9};
	struct Tt t2={1,9.9};
	data *datapt;
	rb_tree tree;
	initializetree(&tree);
	datapt=makedata(1,&t1,sizeof(t1));
	insertree(&tree,datapt);
	datapt=makedata(2,&t2,sizeof(t2));
	insertree(&tree,datapt);
	datapt=searchdata(&tree,1);
	printf(" %d \n",datapt->key);
	printf(" %f \n",((struct Tt *)datapt->content)->b);
}
