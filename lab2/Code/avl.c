#include "avl.h"
#include "semantic.h"

AVL_node *announce_stack[101];
AVL_node *define_stack[101];
int announce_top = 0;
int define_top = 0;
//符号表栈，用于嵌套定义

AVL_node *st[100001]; //插入时用的栈

void table_init()
{
	announce_stack[announce_top] = NULL;
	define_stack[define_top] = NULL;
}

void stack_push()
{
	announce_top++;
	announce_stack[announce_top] = NULL;

	define_top++;
	define_stack[define_top] = NULL;
}
void stack_pop()
{
	announce_top--;
	define_top--;
}

int cmp(AVL_node *a, AVL_node *b)
{
	return strcmp(a->node->name, b->node->name);
} //比较名字，>0表示a更大，<0表示b更大，=0表示相等

AVL_node *new_avl_node(ListNode *k)
{
	AVL_node *p = (AVL_node *)malloc(sizeof(AVL_node));
	p->node = k;
	p->cnt = 1; //相同人数的城市个数
	p->lc = NULL;
	p->rc = NULL;
	p->bf = 0;	//平衡因子
	p->sum = 1; //该树总节点个数
	return p;
}

void RotateL(AVL_node *(*ptr))
{
	AVL_node *subL = (*ptr); // A结点
	(*ptr) = subL->rc;		 // C结点上升，替代A
	subL->rc = (*ptr)->lc;	 //  D成为A的右结点
	(*ptr)->lc = subL;		 //  A成为C的左结点
	(*ptr)->bf = subL->bf = 0;

	subL->sum -= (*ptr)->sum;
	if (subL->rc != NULL)
		subL->sum += subL->rc->sum;
	(*ptr)->sum = (*ptr)->cnt;
	if ((*ptr)->lc != NULL)
		(*ptr)->sum += (*ptr)->lc->sum;
	if ((*ptr)->rc != NULL)
		(*ptr)->sum += (*ptr)->rc->sum;
}

void RotateR(AVL_node *(*ptr))
{
	//左子树比右子树高, 旋转后新根在(*ptr)
	AVL_node *subR = (*ptr); //要右旋转的结点
	(*ptr) = subR->lc;
	subR->lc = (*ptr)->rc; //转移(*ptr)右边负载
	(*ptr)->rc = subR;	   // (*ptr)成为新根
	(*ptr)->bf = subR->bf = 0;

	subR->sum -= (*ptr)->sum;
	if (subR->lc != NULL)
		subR->sum += subR->lc->sum;
	(*ptr)->sum = (*ptr)->cnt;
	if ((*ptr)->lc != NULL)
		(*ptr)->sum += (*ptr)->lc->sum;
	if ((*ptr)->rc != NULL)
		(*ptr)->sum += (*ptr)->rc->sum;
}

void RotateLR(AVL_node *(*ptr))
{
	AVL_node *subR = (*ptr);   // A结点
	AVL_node *subL = subR->lc; // B结点
	(*ptr) = subL->rc;		   // E结点上升
	subL->rc = (*ptr)->lc;	   // E的左孩子成为B的右孩子
	(*ptr)->lc = subL;		   // B结点成为E的左孩子
	if ((*ptr)->bf <= 0)
		subL->bf = 0;
	else
		subL->bf = -1;
	subR->lc = (*ptr)->rc; // E的右孩子成为B的左孩子
	(*ptr)->rc = subR;	   // A结点成为E的右孩子
	if ((*ptr)->bf == -1)
		subR->bf = 1;
	else
		subR->bf = 0;
	(*ptr)->bf = 0;

	subR->sum -= subL->sum;
	if (subR->lc != NULL)
		subR->sum += subR->lc->sum;

	subL->sum -= (*ptr)->sum;
	if (subL->rc != NULL)
		subL->sum += subL->rc->sum;

	(*ptr)->sum = subL->sum + subR->sum + (*ptr)->cnt;
};

void RotateRL(AVL_node *(*ptr))
{
	AVL_node *subL = (*ptr); // A结点
	AVL_node *subR = subL->rc;
	(*ptr) = subR->lc;
	subR->lc = (*ptr)->rc;
	(*ptr)->rc = subR;
	if ((*ptr)->bf >= 0)
		subR->bf = 0;
	else
		subR->bf = 1;
	subL->rc = (*ptr)->lc;
	(*ptr)->lc = subL;
	if ((*ptr)->bf == 1)
		subL->bf = -1;
	else
		subL->bf = 0;
	(*ptr)->bf = 0;

	subL->sum -= subR->sum;
	if (subL->rc != NULL)
		subL->sum += subL->rc->sum;

	subR->sum -= (*ptr)->sum;
	if (subR->lc != NULL)
		subR->sum += subR->lc->sum;

	(*ptr)->sum = subL->sum + subR->sum + (*ptr)->cnt;
}

bool Insert(AVL_node *(*ptr), ListNode *k_)
{
	//在以(*ptr)为根的AVL树中插入新元素k,如果插入成功,函数返回true,否则返回false.
	AVL_node *k = new_avl_node(k_);
	AVL_node *pr = NULL, *p = (*ptr), *q;
	int d;
	int tail = 0;
	while (p != NULL)
	{
		//寻找插入位置
		pr = p;
		st[++tail] = pr;
		pr->sum++;
		//否则用栈记忆查找路径
		if (cmp(k, p) < 0) //比较部分
			p = p->lc;
		else if (cmp(k, p) > 0) //比较部分
			p = p->rc;
		else
		{
			p->cnt++;
			return true;
		}
	}
	p = k;
	//创建新结点,data=el,bf=0
	if (p == NULL)
	{
		printf("存储空间不足!\n");
		exit(0);
	}
	if (pr == NULL)
	{
		(*ptr) = p;
		return true;
	}
	//空树,新结点成为根结点
	if (cmp(k, pr) < 0) //比较部分
		pr->lc = p;
	//新结点插入
	else
		pr->rc = p;
	while (tail > 0)
	{
		//重新平衡化
		pr = st[tail--];
		//从栈中退出父结点
		if (p == pr->lc)
			pr->bf--;
		//调整父结点的平衡因子
		else
			pr->bf++;
		if (pr->bf == 0)
			break;
		//第1种情况,平衡退出
		if (pr->bf == 1 || pr->bf == -1)
			//第2种情况，|bf|=1
			p = pr;
		else
		{
			//第3种情况,|bf|=2
			d = (pr->bf < 0) ? -1 : 1;
			//区别单双旋转标志
			if (p->bf == d)
			{
				//两结点平衡因子同号,单旋转
				if (d == -1)
					RotateR(&pr);
				//右单旋转
				else
					RotateL(&pr);
				//左单旋转
			}
			else
			{
				//两结点平衡因子反号,双旋转
				if (d == -1)
					RotateLR(&pr);
				//先左后右双旋转,“<”型
				else
					RotateRL(&pr);
				//先右后左双旋转,“>”型
			}
			break;
			//不再向上调整
		}
	}
	if (tail == 0)
		(*ptr) = pr;
	//调整到树的根结点
	else
	{
		//中间重新链接
		q = st[tail--];
		if (cmp(q, pr) > 0) //比较部分
			q->lc = pr;
		else
			q->rc = pr;
	}
	return true;
}

AVL_node *search(int x, AVL_node *k)
{
	//一个节点的大小顺序：左子树>自己>右子树
	int now = 0;
	if (k->lc != NULL)
		now = k->lc->sum;
	//取左子树大小
	if (x <= now)
		return search(x, k->lc);
	//要找的点在左子树
	if (x > now + k->cnt)
		return search(x - now - k->cnt, k->rc);
	//在右子树
	return k;
} //寻找第x大城市

void print_avl_tree(AVL_node *k)
{
	if (k == NULL)
	{
		printf(".\n");
		//	printf(".");
		return;
	}
	//	printf("%s(%d) ", k->node->name, k->node->type->kind);
	print_avl_tree(k->lc);
	print_avl_listnode(k->node);
	print_avl_tree(k->rc);
}

void print_avl_listnode(ListNode *k)
{
	printf("%s : ", k->name);
	print_avl_type(k->type);
}

void print_avl_type(Type *k)
{
	if (k == NULL)
		return;
	if (k->kind == BASIC)
	{
		printf("BASIC");
		if (k->basic == Basic_int)
			printf("_int\n");
		else
			printf("_float\n");
	}
	else if (k->kind == ARRAY)
	{
		printf("ARRAY");
		printf("[%d]", k->array.size);
		print_avl_type(k->array.elem);
	}
	else if (k->kind == STRUCTURE)
	{
		printf("STRUCTURE");
		printf("{\n");
		for (ListNode *i = k->structure; i; i = i->next)
		{
			print_avl_listnode(i);
		}
		printf("}\n");
	}
	else
	{
		print_avl_type(k->func.func_type);
		printf("FUNC");
		printf("(\n");
		for (ListNode *i = k->func.para; i; i = i->next)
			print_avl_listnode(i);
		printf(")\n");
	}
}

ListNode *avl_search_listnode(AVL_node *k, char *name, enum Kind kind)
{
	if (k == NULL)
		return NULL;
	if (strcmp(k->node->name, name) == 0 &&
		kind == k->node->type->kind)
	{
		return copy_listnode(k->node);
	}

	else if (strcmp(name, k->node->name) < 0)
		return avl_search_listnode(k->lc, name, kind);
	else
		return avl_search_listnode(k->rc, name, kind);
}

Type *avl_search_type(AVL_node *k, char *name)
{
	ListNode *tmp = avl_search_listnode(k, name, BASIC);
	if (tmp == NULL)
		tmp = avl_search_listnode(k, name, ARRAY);
	if (tmp == NULL)
		tmp = avl_search_listnode(k, name, STRUCTURE);
	if (tmp == NULL)
		tmp = avl_search_listnode(k, name, FUNC);
	if (tmp == NULL)
		return NULL;
	return tmp->type;
}

ListNode *search_listnode(enum Table_kind m, char *name, enum Kind kind, bool flag)
{
	ListNode *now = NULL;
	int bottom = 0;
	if (m == define)
	{
		if (flag)
			bottom = define_top;
		for (int i = define_top; i >= bottom; i--)
		{

			now = avl_search_listnode(define_stack[i], name, kind);
			//	printf("now i=%d\n",i);
			if (now)
				break;
		}
	}
	else
	{
		if (flag)
			bottom = announce_top;
		for (int i = announce_top; i >= bottom; i--)
		{
			now = avl_search_listnode(announce_stack[i], name, kind);
			if (now)
				break;
		}
	}
	//	        		if(now){
	//		printf("find it !\n");
	//		print_avl_listnode(now);
	//	}
	return now;
}

ListNode *search_all_listnode(enum Table_kind m, char *name, bool flag)
{
	ListNode *tmp1 = search_listnode(m, name, BASIC, flag);
	ListNode *tmp2 = search_listnode(m, name, ARRAY, flag);
	ListNode *tmp3 = search_listnode(m, name, FUNC, flag);
	ListNode *tmp4 = search_listnode(m, name, STRUCTURE, flag);
	ListNode *tmp = NULL;
	if (tmp1)
		tmp = tmp1;
	else if (tmp2)
		tmp = tmp2;
	else if (tmp3)
		tmp = tmp3;
	else
		tmp = tmp4;
	return tmp;
}

Type *search_type(enum Table_kind m, char *name, bool flag)
{
	Type *now = NULL;
	int bottom = 0;
	if (m == define)
	{
		if (flag)
			bottom = define_top;
		for (int i = define_top; i >= bottom; i--)
		{
			now = avl_search_type(define_stack[i], name);
			if (now)
				break;
		}
	}
	else
	{
		if (flag)
			bottom = announce_top;
		for (int i = announce_top; i >= bottom; i--)
		{
			now = avl_search_type(announce_stack[i], name);
			if (now)
				break;
		}
	}
	return now;
}

void check_announce(AVL_node *k, enum Kind kind)
{
	if (k == NULL)
		return;
	if (k->node->type->kind == kind)
	{
		ListNode *now = search_listnode(define, k->node->name, kind, false);
		if (now==NULL)
		{
			error(18, k->node->type->func.func_line,
				  "Undefined function", k->node->name);
		}
	}
	check_announce(k->lc, kind);
	check_announce(k->rc, kind);
}