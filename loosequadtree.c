/*
Christopher Antol
CMSC420-0101
Fall 2011
Project 1 Part 4
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_STRING_LEN 256
#define MAX_NAME_LEN 6
#define MIN_LX 1.5 /* smallest rectangle is 3x3 */
#define MIN_LY 1.5
#define PIXEL_LX 0.5 /* pixel size is 1x1 */
#define PIXEL_LY 0.5

#define NDIR_1D 2 /* number of directions in 1d space */
#define NDIR_2D 4 /* number of directions in 2d space */

typedef enum { NW, NE, SW, SE } quadrant;
typedef enum { LEFT, RIGHT, BOTH } direction;

typedef char tName[MAX_NAME_LEN + 1];

struct List {
	struct Rectangle *rect;
	double distance;
	struct List *next;
};

struct cNodeList {
	struct cNode *node;
	double distance;
	int id;
	struct cNodeList *next;
};

struct Rectangle {
	tName Name; /* name of rectangle */
	struct Rectangle *binSon[NDIR_1D]; /* left and right sons */
	int Center[NDIR_1D]; /* centroid */
	int Length[NDIR_1D]; /* distance to borders of rectangle */
};

struct bNode {  /* nodes in doubly linked list of the set of rectangles */
	struct bNode *prev, *next; /* previous and next nodes */
	struct Rectangle *Rect; /* pointer to rectangle */
};

struct cNode { /* Node in LOOSE quadtree */
	struct cNode *spcSon[NDIR_2D]; /* Four principal quad directions */
	struct bNode *binSon; /* Pointers to retangle sets */
};

struct looseQuad {
	struct cNode *looseQuadRoot; /* root cNode */
	struct Rectangle World; /* world extent */
	float p; /* expansion factor */
};

/* global variables holding roots of corresponding trees */

struct looseQuad looseQuadTree; /* loose quad tree */
struct Rectangle *rectTree; /* rectangle binary tree sorted with respect to rectangle names */
int tracer, first, second;
struct Rectangle *tempTree;
struct List *tempList;
struct cNodeList *tempNodeList;

void initLooseQuadTree(void) {
	looseQuadTree.looseQuadRoot = NULL;
	strcpy(looseQuadTree.World.Name, "LOOSEQUAD");
	looseQuadTree.World.binSon[0] = looseQuadTree.World.binSon[1] = NULL;
	/* world size and expansion factor will have to be assigned later */
}

void initRectTree(void) {rectTree = NULL;}

void doInitQuadtree(int width, double p) {
	int length = (pow(2,width))/2;
	looseQuadTree.World.Length[0] = looseQuadTree.World.Length[1] = length;
	looseQuadTree.World.Center[0] = looseQuadTree.World.Center[1] = length;
	looseQuadTree.p = p;
	printf("LOOSE QUADTREE INITIALIZED WITH PARAMETERS %d %d\n", width, (int)p);
}

void visitCNode(struct cNode *node, int length, int x, int y, int n) {
	if (node->spcSon[0] != NULL || node->spcSon[1] != NULL || node->spcSon[2] != NULL || node->spcSon[3] != NULL) {
		/*draw grid for this square*/
		printf("LD(2,2)\n");
		printf("DL(%d,%d,%d,%d)\n", x, y-length, x, y+length);
		printf("DL(%d,%d,%d,%d)\n", x-length, y, x+length, y);
	}
	/*recursive on each child*/
	if (node->spcSon[0] != NULL)
		visitCNode(node->spcSon[0], length/2, x-(length/2), y+(length/2), 4*n + 1);
	if (node->spcSon[1] != NULL)
		visitCNode(node->spcSon[1], length/2, x+(length/2), y+(length/2), 4*n + 2);
	if (node->spcSon[2] != NULL)
		visitCNode(node->spcSon[2], length/2, x-(length/2), y-(length/2), 4*n + 3);
	if (node->spcSon[3] != NULL)
		visitCNode(node->spcSon[3], length/2, x+(length/2), y-(length/2), 4*n + 4);
		
	struct bNode *curr = node->binSon;
	while (curr!=NULL) {
		/*draw curr->Rect*/
		int x1 = curr->Rect->Center[0]-curr->Rect->Length[0];
		int y1 = curr->Rect->Center[1]+curr->Rect->Length[1];
		int x2 = curr->Rect->Center[0]+curr->Rect->Length[0];
		int y2 = curr->Rect->Center[1]-curr->Rect->Length[1];
		int px = x1;
		int py = y1+1;
		
		/* set offset of text */
		if (y1+2 >= looseQuadTree.World.Center[1] + looseQuadTree.World.Length[1])
			py = y2-2;
		if (py-3 < 1){
			py = curr->Rect->Center[1];
			if (x2+9 >= looseQuadTree.World.Center[0]+looseQuadTree.World.Length[0])
				px = x1-9;
			else
				px = x2+1;
		}
		if (px < 1) {
			px = x1+1;
			py = y1-1;
		}
		printf("LD(0,0)\n");
		printf("DR(%d,%d,%d,%d)\n", x1, y1, x2, y2);
		/*draw node and name*/
		printf("DN(%d,%d,%d)\n", n, px, py);
		printf("DN(%s,%d,%d)\n", curr->Rect->Name, px+2, py);
		
		curr = curr->next;
	}
}

void doDisplay(void) {
	int length = looseQuadTree.World.Length[0];
	int center = looseQuadTree.World.Center[0];
	printf("$$$$ SP(%d, %d)\n", length*2, length*2);
	if (looseQuadTree.looseQuadRoot != NULL)
		visitCNode(looseQuadTree.looseQuadRoot, length, center, center, 0);
	printf("EP\n");
}

void inorder(struct Rectangle *r) {
	if (r->binSon[0] != NULL)
		inorder(r->binSon[0]);
	if (first) {
		printf("%s,%d,%d,%d,%d", r->Name, r->Center[0], r->Center[1], r->Length[0], r->Length[1]);
		first--;
	} else
		printf(" %s,%d,%d,%d,%d", r->Name, r->Center[0], r->Center[1], r->Length[0], r->Length[1]);
	if (r->binSon[1] != NULL)
		inorder(r->binSon[1]);
};

void doListRect() {
	if (rectTree==NULL)
		printf("NO RECTANGLES IN DATABASE\n");
	else {
		first = 1;
		inorder(rectTree);
		printf("\n");
	}
}

int my_strcmp (const char *s1, const char *s2) {
	for (; *s1 == *s2 && *s1; ++s1, ++s2);
	return (((*(const unsigned char *)s1) * 4) & 0xFF) - (((*(const unsigned char *)s2) * 4) & 0xFF);
}

void freeTree(struct Rectangle **treeNode) {
	if (*treeNode == NULL) return;
	freeTree(&(*treeNode)->binSon[0]);
	freeTree(&(*treeNode)->binSon[1]);
	free(*treeNode);
}

void freeNodeList(struct cNodeList **listNode) {
	struct cNodeList *prev = NULL, *curr = *listNode;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	*listNode = NULL;
}

void freeList(struct List **listNode) {
	struct List *prev = NULL, *curr = *listNode;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	*listNode = NULL;
}

void addToNodeList(struct cNodeList **listNode, struct cNode *node, double distance, int id) {
	struct cNodeList *n = (struct cNodeList *)malloc(sizeof(struct cNodeList));
	n->node = node;
	n->distance = distance;
	n->id = id;
	n->next = NULL;
	
	if (*listNode == NULL) {
		*listNode = n;
		return;
	}
	
	struct cNodeList *prev = NULL, *curr = *listNode;
	while (curr != NULL) {
		if (distance < curr->distance) {
			n->next = curr;
			if (prev == NULL)
				*listNode = n;
			else
				prev->next = n;
			break;
		} else if (distance == curr->distance) {
			//do crazy while loop here to insert based on id
			while (curr != NULL && curr->distance == distance) {
				if (curr->id < id) {
					prev = curr;
					curr = curr->next;
				} else {
					break;
				}
			}
			n->next = curr;
			if (prev == NULL)
				*listNode = n;
			else 
				prev->next = n;
			break;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
	if (curr==NULL)
		prev->next = n;
}

struct cNodeList *dequeue(struct cNodeList **listNode) {
	if (*listNode == NULL) return NULL;
	
	struct cNodeList *curr = *listNode;
	*listNode = (*listNode)->next;
	return curr;
}

void addToList(struct List **listNode, struct Rectangle *r, double distance) {

	if (*listNode != NULL) {
		if ((*listNode)->distance > distance) {
			freeList(listNode);
			addToList(listNode,r,distance);
		} else if ((*listNode)->distance < distance)
			return;
	}

	struct List *node = (struct List *)malloc(sizeof(struct List));
	node->rect = r;
	node->distance = distance;
	node->next = NULL;
	
	if (*listNode == NULL) {
		*listNode = node;
		return;
	}
	
	struct List *prev = NULL, *curr = *listNode;
	while (curr != NULL) {
		if (my_strcmp(node->rect->Name, curr->rect->Name) < 0)
			break;
		else {
			prev = curr;
			curr = curr->next;
		}
	}
	
	node->next = curr;
	if (prev == NULL)
		*listNode = node;
	else 
		prev->next = node;
}

void addToRectTree(struct Rectangle **treeNode, struct Rectangle *r){
	if (*treeNode == NULL) {
		struct Rectangle *ptr = (struct Rectangle *)malloc(sizeof(struct Rectangle));
		strcpy(ptr->Name, r->Name);
		ptr->Center[0] = r->Center[0];
		ptr->Center[1] = r->Center[1];
		ptr->Length[0] = r->Length[0];
		ptr->Length[1] = r->Length[1];
		*treeNode = ptr;
		(*treeNode)->binSon[0] = (*treeNode)->binSon[1] = NULL;
	}
	else if (my_strcmp(r->Name,(*treeNode)->Name)<0)
		addToRectTree(&(*treeNode)->binSon[0], r);
	else
		addToRectTree(&(*treeNode)->binSon[1], r);
}

void doCreateRect(char *name, int cx, int cy, int lx, int ly) {
	struct Rectangle *r = (struct Rectangle *)malloc(sizeof(struct Rectangle));
	strcpy(r->Name, name);
	r->Center[0] = cx;
	r->Center[1] = cy;
	r->Length[0] = lx;
	r->Length[1] = ly;
	
	addToRectTree(&rectTree,r);
	
	free(r);
	
	printf("RECTANGLE %s CREATED WITH PARAMETERS %d %d %d %d\n", name, cx, cy, lx, ly);
}

struct Rectangle **findRect(char *name, struct Rectangle **root) {
	struct Rectangle **node = root;
	while (*node != NULL) {
		int compare_result = my_strcmp(name, (*node)->Name);
		if (compare_result<0)
			node = &(*node)->binSon[0];
		else if (compare_result>0)
			node = &(*node)->binSon[1];
		else
			break;
	}
	return node;
}

void addRectToBNode(struct Rectangle *r, struct bNode **start) {
	struct bNode *ptr = (struct bNode *)malloc(sizeof(struct bNode));
	ptr->prev = ptr->next = NULL;
	ptr->Rect = r;
	
	if (*start == NULL)
		*start = ptr;
	else {
		struct bNode **curr = start;
		while ((*curr)->next != NULL) {
			curr = &(*curr)->next;
		}
		(*curr)->next = ptr;
		ptr->prev = *curr;
	}
}

int addRectToTree(struct Rectangle *r, struct cNode **node, int cx, int cy, int length, int n, int rx, int ry) {
	if (*node == NULL) { /* creates quadrant */
		struct cNode *ptr = (struct cNode *)malloc(sizeof(struct cNode));
		ptr->spcSon[0] = ptr->spcSon[1] = ptr->spcSon[2] = ptr->spcSon[3] = NULL;
		ptr->binSon = NULL;
		*node = ptr;
	}
	
	if (tracer) printf("%d ", n);
	
	if (length <= 1) {
		addRectToBNode(r, &((*node)->binSon));
		return;
	}
	
	
	int rcx = rx;
	int rcy = ry;
	
	int p = (int)looseQuadTree.p;
	
	int left = rcx-r->Length[0];
	int right = rcx+r->Length[0];
	int top = rcy+r->Length[1];
	int bottom = rcy-r->Length[1];
	
	if (rcx < cx && rcy < cy) { /* SW */
		if ((top < p*(length/2) + cy) && (right < p*(length/2) + cx) &&
			(left >= cx - length - (p*(length/2))) && (bottom >= cy - length - (p*(length/2)))) {
			addRectToTree(r, &((*node)->spcSon[2]), cx-(length/2), cy-(length/2), length/2, 4*n+3,rx,ry);
		}
		else
			addRectToBNode(r, &((*node)->binSon));
	} else if (rcx < cx && rcy >= cy) { /* NW */
		if ((bottom >= cy - (p*(length/2))) && (right < p*(length/2) + cx) &&
			(left >= cx - length - (p*(length/2))) && (top < p*(length/2) + cy + length)) {
			addRectToTree(r, &((*node)->spcSon[0]), cx-(length/2), cy+(length/2), length/2, 4*n+1,rx,ry);
		}
		else
			addRectToBNode(r, &((*node)->binSon));		
	} else if (rcx >= cx && rcy < cy) { /* SE */
		if ((top < cy + p*(length/2)) && (left >= cx - (p*(length/2))) &&
			(bottom >= cy - length - (p*(length/2))) && (right < p*(length/2) + cx+length)) {
			addRectToTree(r, &((*node)->spcSon[3]), cx+(length/2), cy-(length/2), length/2, 4*n+4,rx,ry);
		}
		else
			addRectToBNode(r, &((*node)->binSon));
	} else if (rcx >= cx && rcy >= cy) { /* NE */
		if ((bottom >= cy-(p*(length/2))) && (left >= cx-(p*(length/2))) &&
			(top < p*(length/2) + cy + length) && (right < p*(length/2) + cx+length)) {
			addRectToTree(r, &((*node)->spcSon[1]), cx+(length/2), cy+(length/2), length/2, 4*n+2,rx,ry);
		}
		else 
			addRectToBNode(r, &((*node)->binSon));
	}
		
	return 1;
	
}

void doInsert(char *name) {
	/*find the rectangle to insert*/
	struct Rectangle *r = *(findRect(name, &rectTree));
	
	/*check whether rectangle is within bounds of quadtree span*/
	int left = (r->Center[0]-r->Length[0]) - (looseQuadTree.World.Center[0]-looseQuadTree.World.Length[0]);
	int right = (looseQuadTree.World.Center[0]+looseQuadTree.World.Length[0])-(r->Center[0]+r->Length[0]);
	int top = (looseQuadTree.World.Center[1]+looseQuadTree.World.Length[1])-(r->Center[1]+r->Length[1]);
	int bottom = (r->Center[1]-r->Length[1]) - (looseQuadTree.World.Center[1]-looseQuadTree.World.Length[1]);
	if (left>=0 && right>=0 && top>=0 && bottom>=0) {
		first = 1;
		addRectToTree(r, &looseQuadTree.looseQuadRoot, looseQuadTree.World.Center[0], looseQuadTree.World.Center[1], looseQuadTree.World.Length[0], 0, r->Center[0], r->Center[1]);
		if (tracer) printf("\n");
		printf("RECTANGLE %s HAS BEEN INSERTED\n", name);
	}
	else
		printf("INSERTION OF RECTANGLE %s FAILED AS %s LIES PARTIALLY OUTSIDE SPACE SPANNED BY LOOSE QUADTREE\n", name, name);
}

int move(struct Rectangle *r,int cx,int cy,int length,struct cNode **node,int n,int ox,int oy,int nx,int ny) {
	int rcx = ox;
	int rcy = oy;
	int found = 0;
	int empty = 0;
	int deleted = 0;
	
	if ((*node) == NULL)
		return 0;
	
	if (tracer) printf("%d ", n);
	
	struct bNode *curr = (*node)->binSon;
	
	while (curr != NULL) {
		if (strcmp(r->Name,curr->Rect->Name)==0) {
			found = 1;
			break;
		} else
			curr = curr->next;
	}
	
	if (found) {
		if (curr->prev == NULL && curr->next == NULL)
			empty = 1;
		else if (curr->prev == NULL) {
			curr->next->prev = NULL;
			curr = curr->next;
		} else if (curr->next == NULL) {
			curr->prev->next = NULL;
			curr = curr->prev;
		}
	
	
		if (empty) {
			/* free bnode and set cnode's pointer to null */
			free((*node)->binSon);
			(*node)->binSon = NULL;
		} else {
			/* traverse back to beginning and point cNode's binson at first element */
			while (curr->prev != NULL) {
				curr = curr->prev;
			}
			(*node)->binSon = curr;
		}
		deleted = 1;
			
	} else {
	
		if (rcx < cx && rcy < cy) { /* SW */
			deleted = move(r, cx-(length/2), cy-(length/2), length/2, &((*node)->spcSon[2]), 4*n+3,ox,oy,nx,ny);
		} else if (rcx < cx && rcy >= cy) { /* NW */
			deleted = move(r, cx-(length/2), cy+(length/2), length/2, &((*node)->spcSon[0]), 4*n+1,ox,oy,nx,ny);
		} else if (rcx >= cx && rcy < cy) { /* SE */
			deleted = move(r, cx+(length/2), cy-(length/2), length/2, &((*node)->spcSon[3]), 4*n+4,ox,oy,nx,ny);
		} else if (rcx >= cx && rcy >= cy) { /* NE */
			deleted = move(r, cx+(length/2), cy+(length/2), length/2, &((*node)->spcSon[1]), 4*n+2,ox,oy,nx,ny);
		} else
			return 0;
	}
	
	/* DO INSERTION STUFF USING NX and NY */
	if (deleted) {
	int ctop = cy+length;
	int cbottom = cy-length;
	int cleft = cx-length;
	int cright = cx+length;
	
	/* case where we encounter clean branching node for insert on way back */
	if (nx > cleft && nx < cright && ny < ctop && ny > cbottom && !second) {
		int p = (int)looseQuadTree.p;
		
		int left = nx-r->Length[0];
		int right = nx+r->Length[0];
		int top = ny+r->Length[1];
		int bottom = ny-r->Length[1];
	if (top<p*(length)+ctop && bottom >= cbottom-(p*length) &&
	right < cright+(p*length) && left >= cleft-(p*length)) {
	if (nx < cx && ny < cy) { /* SW */
		if ((top < p*(length/2) + cy) && (right < p*(length/2) + cx) &&
			(left >= cx - length - (p*(length/2))) && (bottom >= cy - length - (p*(length/2))))
			second = addRectToTree(r, &((*node)->spcSon[2]), cx-(length/2), cy-(length/2), length/2, 4*n+3,nx,ny);
		else {
			second = 1;
			addRectToBNode(r, &((*node)->binSon));
		}			
	} else if (nx < cx && ny >= cy) { /* NW */
		if ((bottom >= cy - (p*(length/2))) && (right < p*(length/2) + cx) &&
			(left >= cx - length - (p*(length/2))) && (top < p*(length/2) + cy + length))
			second = addRectToTree(r, &((*node)->spcSon[0]), cx-(length/2), cy+(length/2), length/2, 4*n+1,nx,ny);
		else {
			second = 1;
			addRectToBNode(r, &((*node)->binSon));
		}			
	} else if (nx >= cx && ny < cy) { /* SE */
		if ((top < cy + p*(length/2)) && (left >= cx - (p*(length/2))) &&
			(bottom >= cy - length - (p*(length/2))) && (right < p*(length/2) + cx+length))
			second = addRectToTree(r, &((*node)->spcSon[3]), cx+(length/2), cy-(length/2), length/2, 4*n+4,nx,ny);
		else {
			second = 1;
			addRectToBNode(r, &((*node)->binSon));
		}			
	} else if (nx >= cx && ny >= cy) { /* NE */
		if ((bottom >= cy-(p*(length/2))) && (left >= cx-(p*(length/2))) &&
			(top < p*(length/2) + cy + length) && (right < p*(length/2) + cx+length))
			second = addRectToTree(r, &((*node)->spcSon[1]), cx+(length/2), cy+(length/2), length/2, 4*n+2,nx,ny);
		else {
			second = 1;
			addRectToBNode(r, &((*node)->binSon));
		}			
	}
	
	}
	}
	
	/* case where we return to root without having found a clean branch, start from scratch */
	if (!second && n == 0) {
			addRectToBNode(r, &((*node)->binSon));
	}
	
	/* check if cnode has any children, if yes then return, else free cnode
		and add rectangle on way out */
	if ((*node)->spcSon[0] == NULL && (*node)->spcSon[1] == NULL && (*node)->spcSon[2] == NULL &&
		(*node)->spcSon[3] == NULL && (*node)->binSon == NULL) {
		free(*node);
		*node = NULL;
	}
	}
	return deleted;
}

void doMove(char *name, int cx, int cy) {
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("RECTANGLE %s WAS NOT FOUND IN THE LOOSE QUADTREE\n", name);
		return;
	}
	int newx = r->Center[0]+cx;
	int newy = r->Center[1]+cy;

	int left = (newx-r->Length[0]) - (looseQuadTree.World.Center[0]-looseQuadTree.World.Length[0]);
	int right = (looseQuadTree.World.Center[0]+looseQuadTree.World.Length[0])-(newx+r->Length[0]);
	int top = (looseQuadTree.World.Center[1]+looseQuadTree.World.Length[1])-(newy+r->Length[1]);
	int bottom = (newy-r->Length[1]) - (looseQuadTree.World.Center[1]-looseQuadTree.World.Length[1]);
	
	if (left>=0 && right>=0 && top>=0 && bottom>=0) {
		first = 1;
		second = 0;
		int foundit = move(r, looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
			looseQuadTree.World.Length[0], &looseQuadTree.looseQuadRoot, 0, r->Center[0], r->Center[1], newx, newy);
		r->Center[0] = newx;
		r->Center[1] = newy;
		if (tracer) printf("\n");
		if (foundit)
			printf("RECTANGLE %s HAS BEEN MOVED BY %d %d\n", name, cx, cy);
		else
			printf("RECTANGLE %s WAS NOT FOUND IN THE LOOSE QUADTREE\n", name);
	} else
		printf("RECTANGLE %s WOULD BE OUTSIDE LOOSE QUADTREE SPACE\n", name);
}

void searchPoint(int px, int py, int cx, int cy, int length, struct cNode *node, struct Rectangle **r, int n) {
	if (node==NULL)
		return;
		
		if (tracer) printf("%d ", n);
		
	/* search current node's rectangles for any matches */
	if (node->binSon!=NULL && node->binSon->Rect!=NULL) {
		struct bNode *curr = node->binSon;
		int right, left, top, bottom;
		while (curr!=NULL) {
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			
			/* if rectangle contains point */
			if (px <= right && px >= left && py <= top && py >= bottom)
				addToRectTree(r, curr->Rect);
				
			curr = curr->next;
		}
	}
	
	int p = (int)looseQuadTree.p;
	
	/* decide which quadrant(s) to check next */
	if ((px < p*(length/2) + cx) && (py >= cy - (p*(length/2)))) /* NW */
		if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
			node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
			node->spcSon[0]->binSon != NULL)) {
			
			searchPoint(px, py, cx-(length/2), cy+(length/2), length/2, node->spcSon[0], r, 4*n+1);
		}
			
	if ((px >= cx-(p*(length/2))) && (py >= cy-(p*(length/2)))) /* NE */
		if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
			node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
			node->spcSon[1]->binSon != NULL)) {
			
			searchPoint(px, py, cx+(length/2), cy+(length/2), length/2, node->spcSon[1], r, 4*n+2);
		}
			
	if ((px < p*(length/2) + cx) && (py < p*(length/2) + cy)) /* SW */
		if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
			node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
			node->spcSon[2]->binSon != NULL)) {
			
			searchPoint(px, py, cx-(length/2), cy-(length/2), length/2, node->spcSon[2], r, 4*n+3);
		}
			
	if ((px >= cx - (p*(length/2))) && (py < cy + (p*(length/2)))) /* SE */
		if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
			node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
			node->spcSon[3]->binSon != NULL)) {
			
			searchPoint(px, py, cx+(length/2), cy-(length/2), length/2, node->spcSon[3], r, 4*n+4);
		}
}

void printoutRectTree(struct Rectangle *r) {
	if (r->binSon[0] != NULL)
		printoutRectTree(r->binSon[0]);
		printf(" %s", r->Name);
	if (r->binSon[1] != NULL)
		printoutRectTree(r->binSon[1]);
}

void doSearchPoint(int px, int py) {
	tempTree = NULL;
	
	/* traverse quadtree checking if point is inside rectangles
		(checking all quadrants that point lies in at each level), adding them to temptree (which is bst)
		then if tempTree != NULL, do inorder on tempTree to printout names */
	first = 1;
	second = 1;
	searchPoint(px, py, looseQuadTree.World.Center[0], looseQuadTree.World.Center[1], looseQuadTree.World.Length[0], looseQuadTree.looseQuadRoot, &tempTree, 0);
	
	if (tracer) printf("\n");
	if (tempTree==NULL)
		printf("NO RECTANGLE CONTAINS QUERY POINT %d %d\n", px, py);
	else {
		printf("POINT %d %d FALLS IN RECTANGLES", px, py);
		printoutRectTree(tempTree);
		printf("\n");
	}
	freeTree(&tempTree);
}

int deleteRect(struct Rectangle *r, int cx, int cy, int length, struct cNode **node, int n) {
	/* traverse to node,
	find rect in bNode and remove from list
	if list is empty (prev and next are null before removal), free bnode (change pointer to NULL)
	if containing cnode has no children after free, free cnode (change pointer to NULL)
	continue freeing as necessary going backwards popping frames off stack */
		
	int rcx = r->Center[0];
	int rcy = r->Center[1];
	int found = 0;
	int empty = 0;
	int deleted = 0;
	
	if ((*node) == NULL)
		return 0;
		
	if (tracer) printf("%d ", n);
	
	struct bNode *curr = (*node)->binSon;
	
	while (curr != NULL) {
		if (strcmp(r->Name,curr->Rect->Name)==0) {
			found = 1;
			break;
		} else
			curr = curr->next;
	}
	
	if (found) {
		if (curr->prev == NULL && curr->next == NULL)
			empty = 1;
		else if (curr->prev == NULL) {
			curr->next->prev = NULL;
			curr = curr->next;
		} else if (curr->next == NULL) {
			curr->prev->next = NULL;
			curr = curr->prev;
		}
	
	
		if (empty) {
			/* free bnode and set cnode's pointer to null */
			free((*node)->binSon);
			(*node)->binSon = NULL;
		} else {
			/* traverse back to beginning and point cNode's binson at first element */
			while (curr->prev != NULL) {
				curr = curr->prev;
			}
			(*node)->binSon = curr;
		}
		
		deleted = 1;
			
	} else {
	
		if (rcx < cx && rcy < cy) { /* SW */
			deleted = deleteRect(r, cx-(length/2), cy-(length/2), length/2, &((*node)->spcSon[2]), 4*n+3);
		} else if (rcx < cx && rcy >= cy) { /* NW */
			deleted = deleteRect(r, cx-(length/2), cy+(length/2), length/2, &((*node)->spcSon[0]), 4*n+1);
		} else if (rcx >= cx && rcy < cy) { /* SE */
			deleted = deleteRect(r, cx+(length/2), cy-(length/2), length/2, &((*node)->spcSon[3]), 4*n+4);
		} else if (rcx >= cx && rcy >= cy) { /* NE */
			deleted = deleteRect(r, cx+(length/2), cy+(length/2), length/2, &((*node)->spcSon[1]), 4*n+2);
		} else
			return 0;
	}
	
	/* check if cnode has any children, if yes then return, else free cnode */
	if (deleted) {
		if ((*node)->spcSon[0] == NULL && (*node)->spcSon[1] == NULL && (*node)->spcSon[2] == NULL && (*node)->spcSon[3] == NULL && (*node)->binSon == NULL) {
			free(*node);
			*node = NULL;
		}
		return 1;
	} else
		return 0;
}

void doDeleteRect(char *name) {
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL)
		printf("RECTANGLE %s NOT FOUND IN RECTANGLE DATABASE\n", name);
	else {
		second = 1;
		int boolean = deleteRect(r, looseQuadTree.World.Center[0], looseQuadTree.World.Center[1], looseQuadTree.World.Length[0], &looseQuadTree.looseQuadRoot, 0);
		if (tracer)
			printf("\n");
		if (boolean)
			printf("DELETED RECTANGLE %s FROM QUADTREE\n", name);
		else
			printf("RECTANGLE %s NOT FOUND IN LOOSE QUADTREE\n", name);
	}
}

void searchAndDeletePoint(int px,int py,int cx,int cy,int length,struct cNode **node,struct Rectangle **r,int n) {

	if (*node==NULL)
		return;
		
	if (tracer) printf("%d ", n);
		
	int p = (int)looseQuadTree.p;
	int empty = 0;
	int deleted = 0;
	
	/* decide which quadrant(s) to check next */
	if ((px < p*length + cx) && (py >= cy - (p*length))) /* NW */
		if ((*node)->spcSon[0] != NULL)
			searchAndDeletePoint(px, py, cx-(length/2), cy+(length/2), length/2, &(*node)->spcSon[0], r, 4*n+1);
			
	if ((px >= cx-(p*length)) && (py >= cy-(p*length))) /* NE */
		if ((*node)->spcSon[1] != NULL)
			searchAndDeletePoint(px, py, cx+(length/2), cy+(length/2), length/2, &(*node)->spcSon[1], r, 4*n+2);
			
	if ((px < p*length + cx) && (py < p*length + cy)) /* SW */
		if ((*node)->spcSon[2] != NULL)
			searchAndDeletePoint(px, py, cx-(length/2), cy-(length/2), length/2, &(*node)->spcSon[2], r, 4*n+3);
			
	if ((px >= cx - (p*length)) && (py < cy + (p*length))) /* SE */
		if ((*node)->spcSon[3] != NULL)
			searchAndDeletePoint(px, py, cx+(length/2), cy-(length/2), length/2, &(*node)->spcSon[3], r, 4*n+4);
		
	/* search current node's rectangles for any matches */
	if ((*node)->binSon!=NULL && (*node)->binSon->Rect!=NULL) {
		struct bNode *curr = (*node)->binSon;
		int right, left, top, bottom;
		while (curr!=NULL) {
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			
			/* if rectangle contains point */
			if (px <= right && px >= left && py <= top && py >= bottom) {
				addToRectTree(r, curr->Rect);
				
				/* do delete stuff here */
				if (curr->prev == NULL && curr->next == NULL)
					empty = 1;
				else if (curr->prev == NULL) {
					curr->next->prev = NULL;
					curr = curr->next;
				} else if (curr->next == NULL) {
					curr->prev->next = NULL;
					curr = curr->prev;
				}
	
	
				if (empty) {
					/* free bnode and set cnode's pointer to null */
					free((*node)->binSon);
					(*node)->binSon = NULL;
					deleted = 1;
					break;
				} else {
					/* traverse back to beginning and point cNode's binson at first element */
					while (curr->prev != NULL) {
						curr = curr->prev;
					}
					(*node)->binSon = curr;
				}
		
				deleted = 1;	
			}
			curr = curr->next;
		}
	}
	
	if ((*node)->spcSon[0] == NULL && (*node)->spcSon[1] == NULL &&
		(*node)->spcSon[2] == NULL && (*node)->spcSon[3] == NULL && (*node)->binSon == NULL) {
		free(*node);
		*node = NULL;
	}
}

void doDeletePoint(int px, int py) {
	/* at every level, check binSons rect list for encasing rectangle, if yes delete and add it to the tempTree */
	/* share code with search point, but delete when found */
	
	/* navigate to all nodes that point lies in, then after recursive call (on way out) perform delete on
		encasing rectangles */
	
	tempTree = NULL;
	second = 1;
	searchAndDeletePoint(px, py, looseQuadTree.World.Center[0], looseQuadTree.World.Center[1], looseQuadTree.World.Length[0], &looseQuadTree.looseQuadRoot, &tempTree, 0);
	if (tracer) printf("\n");
	if (tempTree==NULL)
		printf("NO RECTANGLES DELETED USING POINT %d %d\n", px, py);
	else {
		first = 1;
		printf("DELETED RECTANGLES");
		printoutRectTree(tempTree);
		printf("\n");
	}
	freeTree(&tempTree);
}

void regionSearch(struct Rectangle *r,struct Rectangle **holder,struct cNode *node,int cx,int cy,int length,int n) {
	if (node == NULL)
		return;

	if (tracer) printf("%d ", n);
	
	int rright = r->Center[0] + r->Length[0];
	int rleft = r->Center[0] - r->Length[0];
	int rtop = r->Center[1] + r->Length[1];
	int rbottom = r->Center[1] - r->Length[1];
	
	/* traverse entire quadtree, checking all rectangles to see if they intersect given rectangle */
	
	if (node->binSon!=NULL && node->binSon->Rect!=NULL) {
		struct bNode *curr = node->binSon;
		int right, left, top, bottom;
		while (curr!=NULL) {
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			
			int h1 = left > rright;
			int h2 = right < rleft;
			int h3 = top < rbottom;
			int h4 = bottom > rtop;
			
			
			
			/* if rectangle intersects */
			if (strcmp(r->Name,curr->Rect->Name)) /* don't add self */
				if (!h1 && !h2 && !h3 && !h4) /* here's where the magic happens */
					addToRectTree(holder, curr->Rect);
				
			curr = curr->next;
		}
	}
	
	/* visit all existing/non-empty children */
	 /* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		regionSearch(r, holder, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		regionSearch(r, holder, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		regionSearch(r, holder, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		regionSearch(r, holder, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4);
	}
}

int doRegionSearch(char *name) {
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL)
		printf("NO RECTANGLE %s EXISTS\n", name);	
	
	tempTree = NULL;
	first = 1;
	regionSearch(r, &tempTree, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	if (tracer) printf("\n");
	
	if (tempTree == NULL) {
		printf("%s DOES NOT INTERSECT AN EXISTING RECTANGLE\n", name);
		return 0;
	} else {
		first = 1;
		printf("RECTANGLE %s INTERSECTS RECTANGLES", name);
		printoutRectTree(tempTree);
		printf("\n");
		freeTree(&tempTree);
		return 1;
	}
}

void touch(struct Rectangle *r,struct Rectangle **holder,struct cNode *node,int cx,int cy,int length,int n) {
	if (node == NULL)
		return;

	if (tracer) printf("%d ", n);
	
	int rright = r->Center[0] + r->Length[0];
	int rleft = r->Center[0] - r->Length[0];
	int rtop = r->Center[1] + r->Length[1];
	int rbottom = r->Center[1] - r->Length[1];
	
	/* traverse entire quadtree, checking all rectangles to see if they intersect given rectangle */
	
	if (node->binSon!=NULL && node->binSon->Rect!=NULL) {
		struct bNode *curr = node->binSon;
		int right, left, top, bottom;
		while (curr!=NULL) {
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			
			int h1 = left == rright;
			int h2 = right == rleft;
			int h3 = top == rbottom;
			int h4 = bottom == rtop;
			
			int h5 = top >= rbottom && bottom <= rtop;
			int h6 = right >= rleft && left <= rright;
			
			
			/* if rectangle intersects */
			if (strcmp(r->Name,curr->Rect->Name)) /* don't add self */
				if ((h1 && h5) || (h2 && h5) || (h3 && h6) || (h4 && h6)) /* here's where the magic happens */
					addToRectTree(holder, curr->Rect);
				
			curr = curr->next;
		}
	}
	
	/* visit all existing/non-empty children */
	 /* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		touch(r, holder, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		touch(r, holder, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		touch(r, holder, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		touch(r, holder, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4);
	}
}

void doTouch(char *name) {
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("NO RECTANGLE %s EXISTS\n", name);
		return;
	}
	
	tempTree = NULL;
	first = 1;
	touch(r, &tempTree, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	if (tracer) printf("\n");
	
	if (tempTree == NULL) {
		printf("NO RECTANGLES IN LOOSE QUADTREE TOUCH RECTANGLE %s\n", name);
	} else {
		first = 1;
		printf("RECTANGLE %s TOUCHES RECTANGLES", name);
		printoutRectTree(tempTree);
		printf("\n");
		freeTree(&tempTree);
	}
}

void within(struct Rectangle *r,struct Rectangle **holder,struct cNode *node,int cx,int cy,int length,int n,int d)
{
	if (node==NULL)
		return;
		
	if (tracer) printf("%d ", n);
	/* determine which children intersect donut, call within on each of them */
	int p = (int)looseQuadTree.p;
	
	int dintop = r->Center[1] + r->Length[1];
	int dinbot = r->Center[1] - r->Length[1];
	int dinlef = r->Center[0] - r->Length[0];
	int dinrig = r->Center[0] + r->Length[0];
	int douttop = dintop + d;
	int doutbot = dinbot - d;
	int doutlef = dinlef - d;
	int doutrig = dinrig + d;
	
	int west = cx + length + p*(length/2);
	int east = cx - length - p*(length/2);
	int north = cy - length - p*(length/2);
	int south = cy + length + p*(length/2);
	
	int wrapsNW = cx-(length/2) - length/2 - p*(length/4) >= dinlef &&
		cx-(length/2) + length/2 + p*(length/4) <= dinrig &&
		cy+(length/2) + length/2 + p*(length/4) <= dintop &&
		cy+(length/2) - length/2 - p*(length/4) >= dinbot;
	int wrapsNE = cx+(length/2) - length/2 - p*(length/4) >= dinlef &&
		cx+(length/2) + length/2 + p*(length/4) <= dinrig &&
		cy+(length/2) + length/2 + p*(length/4) <= dintop &&
		cy+(length/2) - length/2 - p*(length/4) >= dinbot;
	int wrapsSW = cx-(length/2) - length/2 - p*(length/4) >= dinlef &&
		cx-(length/2) + length/2 + p*(length/4) <= dinrig &&
		cy-(length/2) + length/2 + p*(length/4) <= dintop &&
		cy-(length/2) - length/2 - p*(length/4) >= dinbot;
	int wrapsSE = cx+(length/2) - length/2 - p*(length/4) >= dinlef &&
		cx+(length/2) + length/2 + p*(length/4) <= dinrig &&
		cy-(length/2) + length/2 + p*(length/4) <= dintop &&
		cy-(length/2) - length/2 - p*(length/4) >= dinbot;
	
	/* if quadrant intersects but is not contained within the donut */
	if ((douttop >= north) && (doutlef <= west) && !wrapsNW) { /* NW */
		within(r, holder, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1, d);
	}
	if ((douttop >= north) && (doutrig >= east) && !wrapsNE) { /* NE */
		within(r, holder, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2, d);
	}
	if ((doutbot <= south) && (doutlef <= west) && !wrapsSW) { /* SW */
		within(r, holder, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3, d);
	}
	if ((doutbot <= south) && (doutrig >= east) && !wrapsSE) { /* SE */
		within(r, holder, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4, d);
	}
	/* check each rectangle to determine intersection with donut */	
	if (node->binSon!=NULL && node->binSon->Rect!=NULL) {
		struct bNode *curr = node->binSon;
		int right, left, top, bottom;
		while (curr!=NULL) {
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			
			int h1 = left > doutrig;
			int h2 = right < doutlef;
			int h3 = top < doutbot;
			int h4 = bottom > douttop;
			
			int intersect = !h1 && !h2 && !h3 && !h4;
			
			h1 = left > dinlef;
			h2 = right < dinrig;
			h3 = top < dintop;
			h4 = bottom > dinbot;
			int inside = h1 && h2 && h3 && h4;
			
			/* if rectangle intersects */
			if (intersect && !inside)
				if (strcmp(r->Name, curr->Rect->Name))
					addToRectTree(holder, curr->Rect);
				
			curr = curr->next;
		}
	}
}

void doWithin(char *name, int d) {
	/* determine which quadrants the donut touches, visit each of those quads and
		check whether the rectangles there intersect the donut */
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("NO RECTANGLE %s EXISTS\n", name);
		return;
	}
	
	tempTree = NULL;
	first = 1;
	within(r, &tempTree, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0, d);
	if (tracer) printf("\n");
	
	if (tempTree == NULL) {
		printf("NO RECTANGLES OVERLAP RECTANGLE %s EXPANDED BY %d\n", name, d);
	} else {
		first = 1;
		printf("RECTANGLE %s EXPANDED BY %d OVERLAPS RECTANGLES", name, d);
		printoutRectTree(tempTree);
		printf("\n");
		freeTree(&tempTree);
	}
}

void buildHorizNodeList(struct Rectangle *r,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node == NULL) return;
	
	/* traverse entire tree, adding all nodes to nodeList, calculating horiz distance from r */
	/* pass list, node, distance, id */
	addToNodeList(nodelist, node, abs(r->Center[0] - cx), n);
	/* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		buildHorizNodeList(r, nodelist, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		buildHorizNodeList(r, nodelist, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		buildHorizNodeList(r, nodelist, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		buildHorizNodeList(r, nodelist, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4);
	}
}


/* traverse entire tree adding nodes to priority list, then add each rectangle in those nodes to rect list */
void horizNeighbor(struct Rectangle *r,struct List **list,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node==NULL) return;
	
	buildHorizNodeList(r, nodelist, node, cx, cy, length, n);
	
	double distance;
	struct cNodeList *currNode = dequeue(nodelist);
	struct cNode *curr;
	while (currNode != NULL) {
		if (tracer) printf("%d ", currNode->id);
		
		curr = currNode->node;
		
		if (curr->binSon!=NULL && curr->binSon->Rect!=NULL) {
			struct bNode *b = curr->binSon;
			int right, left, top, bottom;
			while (b!=NULL) {
				right = b->Rect->Center[0] + b->Rect->Length[0];
				left = b->Rect->Center[0] - b->Rect->Length[0];
				top = b->Rect->Center[1] + b->Rect->Length[1];
				bottom = b->Rect->Center[1] - b->Rect->Length[1];
			
				if (b->Rect->Center[0] < r->Center[0])
					distance = (r->Center[0] - r->Length[0]) - right;
				else
					distance = left - (r->Center[0] + r->Length[0]);
				
				if (distance >= 0)
					addToList(list, b->Rect, distance);
				
				b = b->next;
			}
		}
		
		free(currNode);
		currNode = dequeue(nodelist);
	}
}

void doHorizNeighbor(char *name) {
	/* go to quadrant containing N's center, then on way out
		check the other quadrant's of each node*/
	/* while traversing out, check horiz distance of rectangles, if new min
		then destroy list of old min and create new list with new min,
		if matching min, add to list */
	/* when list of min is complete, traverse list finding lexi first name */
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("NO RECTANGLE %s EXISTS\n", name);
		return;
	}
	tempList = NULL;
	tempNodeList = NULL;
	
	//call function
	horizNeighbor(r, &tempList, &tempNodeList, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	
	if (tracer) printf("\n");
	
	if (tempList == NULL) {
		printf("RECTANGLE %s HAS NO HORIZONTAL NEIGHBOR\n", name);
	} else {
		printf("RECTANGLE %s HAS HORIZONTAL NEIGHBOR %s\n", name, tempList->rect->Name);
		freeList(&tempList); 
	}
	if (tempNodeList != NULL)
		freeNodeList(&tempNodeList);
}

void buildVertNodeList(struct Rectangle *r,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node == NULL) return;
	
	/* traverse entire tree, adding all nodes to nodeList, calculating horiz distance from r */
	/* pass list, node, distance, id */
	addToNodeList(nodelist, node, abs(r->Center[1] - cy), n);
	/* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		buildVertNodeList(r, nodelist, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		buildVertNodeList(r, nodelist, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		buildVertNodeList(r, nodelist, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		buildVertNodeList(r, nodelist, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4);
	}
}


/* traverse entire tree adding nodes to priority list, then add each rectangle in those nodes to rect list */
void vertNeighbor(struct Rectangle *r,struct List **list,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node==NULL) return;
	
	buildVertNodeList(r, nodelist, node, cx, cy, length, n);
	
	double distance;
	struct cNodeList *currNode = dequeue(nodelist);
	struct cNode *curr;
	while (currNode != NULL) {
		if (tracer) printf("%d ", currNode->id);
		
		curr = currNode->node;
		
		if (curr->binSon!=NULL && curr->binSon->Rect!=NULL) {
			struct bNode *b = curr->binSon;
			int right, left, top, bottom;
			while (b!=NULL) {
				right = b->Rect->Center[0] + b->Rect->Length[0];
				left = b->Rect->Center[0] - b->Rect->Length[0];
				top = b->Rect->Center[1] + b->Rect->Length[1];
				bottom = b->Rect->Center[1] - b->Rect->Length[1];
			
				if (b->Rect->Center[1] < r->Center[1])
					distance = (r->Center[1] - r->Length[1]) - top;
				else
					distance = bottom - (r->Center[1] + r->Length[1]);
				
				if (distance >= 0)
					addToList(list, b->Rect, distance);
				
				b = b->next;
			}
		}
		
		free(currNode);
		currNode = dequeue(nodelist);
	}
}

void doVertNeighbor(char *name) {
	/* do the same thing as horiz...just vertical */
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("NO RECTANGLE %s EXISTS\n", name);
		return;
	}
	tempList = NULL;
	tempNodeList = NULL;
	
	//call function
	vertNeighbor(r, &tempList, &tempNodeList, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	
	if (tracer) printf("\n");
	
	if (tempList == NULL) {
		printf("RECTANGLE %s HAS NO VERTICAL NEIGHBOR\n", name);
	} else {
		printf("RECTANGLE %s HAS VERTICAL NEIGHBOR %s\n", name, tempList->rect->Name);
		freeList(&tempList);
	}
	if (tempNodeList != NULL)
		freeNodeList(&tempNodeList);
}

void buildNearestRectList(int px, int py,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node == NULL) return;
	
	/* traverse entire tree, adding all nodes to nodeList, calculating horiz distance from r */
	/* pass list, node, distance, id */
	addToNodeList(nodelist, node, sqrt(pow((px-cx),2)+pow((py-cy),2)), n);
	/* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		buildNearestRectList(px, py, nodelist, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		buildNearestRectList(px, py, nodelist, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		buildNearestRectList(px, py, nodelist, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		buildNearestRectList(px, py, nodelist, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4);
	}
}

void nearestRect(int px, int py,struct List **list,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node==NULL) return;
	
	buildNearestRectList(px, py, nodelist, node, cx, cy, length, n);
	
	double distance;
	struct cNodeList *currNode = dequeue(nodelist);
	struct cNode *curr;
	while (currNode != NULL) {
		if (tracer) printf("%d ", currNode->id);
		
		curr = currNode->node;
		
		if (curr->binSon!=NULL && curr->binSon->Rect!=NULL) {
			struct bNode *b = curr->binSon;
			int right, left, top, bottom;
			while (b!=NULL) {
				right = b->Rect->Center[0] + b->Rect->Length[0];
				left = b->Rect->Center[0] - b->Rect->Length[0];
				top = b->Rect->Center[1] + b->Rect->Length[1];
				bottom = b->Rect->Center[1] - b->Rect->Length[1];
			
				/* distance calculations here */
				/* if point falls within x range, calculate vert distance,
					if point falls in y range, calculate horiz distance
					else calculate distance to nearest point */
				/* if point is inside, distance is 0 */
				
				/* if inside */
				if (px >= left && px <= right && py <= top && py >= bottom)
					distance = 0;
				else if (px >= left && px <= right) /* point is above/below */
					if (b->Rect->Center[1] > py)
						distance = bottom-py;
					else
						distance = py-top;
				else if (py <= top && py >= bottom) /* point is left/right */
					if (b->Rect->Center[0] > px)
						distance = left-px;
					else
						distance = px-right;
				else { /* find distance to closest corner */
					if (px < b->Rect->Center[0]) /*left*/
						if (py < b->Rect->Center[1]) /*bottom*/
							distance = sqrt(pow(px-left,2) + pow(py-bottom,2));
						else /*left top*/
							distance = sqrt(pow(px-left,2) + pow(py-top,2));
					else/*right*/
						if (py < b->Rect->Center[1]) /*bottom*/
							distance = sqrt(pow(px-right,2) + pow(py-bottom,2));
						else /* right top */
							distance = sqrt(pow(px-right,2) + pow(py-top,2));
				}
				
				addToList(list, b->Rect, distance);
				
				b = b->next;
			}
		}
		
		free(currNode);
		currNode = dequeue(nodelist);
	}
}

void doNearestRect(int px, int py) {
	if (looseQuadTree.looseQuadRoot == NULL) {
		printf("NO NEAREST RECTANGLE TO %d %d FOUND\n", px, py);
		return;
	}
	
	tempList = NULL;
	tempNodeList = NULL;
	
	//call function
	nearestRect(px, py, &tempList, &tempNodeList, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	
	if (tracer) printf("\n");
	if (tempList!=NULL) {
		printf("THE NEAREST RECTANGLE TO %d %d IS %s\n", px, py, tempList->rect->Name);
		freeList(&tempList);
	}
	if (tempNodeList!=NULL)
		freeNodeList(&tempNodeList);
}

/* only visit quadrants that intersect the window and add rectangles fully contained in the window */
void window(int llx, int lly, int lx, int ly, struct Rectangle **holder, struct cNode *node, int cx, int cy,
	int length, int n) {
	if (node==NULL) return;
	
	if (tracer) printf("%d ", n);
	
	int qleft, qright, qtop, qbottom, h1, h2, h3, h4, right, left, top, bottom;
	int p = (int)looseQuadTree.p;
	int wleft = llx;
	int wright = llx+lx;
	int wtop = lly+ly;
	int wbottom = lly;
	
	/* check rectangles here */
	/* if rectangle inside window, add it to holder */
	if (node->binSon!=NULL && node->binSon->Rect!=NULL) {
		struct bNode *curr = node->binSon;
		while (curr!=NULL) {
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			
			h1 = top <= wtop && bottom >= wbottom;
			h2 = right <= wright && left >= wleft;
			
			if (h1 && h2)
				addToRectTree(holder, curr->Rect);
				
			curr = curr->next;
		}
	}
	
	/* find intersecting children nodes */
	/* NW */
	qleft = cx-length-p*(length/2);
	qright = cx+p*(length/2);
	qtop = cy+length+p*(length/2);
	qbottom = cy-p*(length/2);
	h1 = qleft > wright;
	h2 = qright < wleft;
	h3 = qtop < wbottom;
	h4 = qbottom > wtop;
	if (!h1 && !h2 && !h3 && !h4)
		window(llx,lly,lx,ly,holder,node->spcSon[0],cx-(length/2),cy+(length/2),length/2,4*n+1);
	/* NE */
	qleft = cx-p*(length/2);
	qright = cx+length+p*(length/2);
	qtop = cy+length+p*(length/2);
	qbottom = cy-p*(length/2);
	h1 = qleft > wright;
	h2 = qright < wleft;
	h3 = qtop < wbottom;
	h4 = qbottom > wtop;
	if (!h1 && !h2 && !h3 && !h4)
		window(llx,lly,lx,ly,holder,node->spcSon[1],cx+(length/2),cy+(length/2),length/2,4*n+2);
	/* SW */
	qleft = cx-length-p*(length/2);
	qright = cx+p*(length/2);
	qtop = cy+p*(length/2);
	qbottom = cy-length-p*(length/2);
	h1 = qleft > wright;
	h2 = qright < wleft;
	h3 = qtop < wbottom;
	h4 = qbottom > wtop;
	if (!h1 && !h2 && !h3 && !h4)
		window(llx,lly,lx,ly,holder,node->spcSon[2],cx-(length/2),cy-(length/2),length/2,4*n+3);
	/* SE */
	qleft = cx-p*(length/2);
	qright = cx+length+p*(length/2);
	qtop = cy+p*(length/2);
	qbottom = cy-length-p*(length/2);
	h1 = qleft > wright;
	h2 = qright < wleft;
	h3 = qtop < wbottom;
	h4 = qbottom > wtop;
	if (!h1 && !h2 && !h3 && !h4)
		window(llx,lly,lx,ly,holder,node->spcSon[3],cx+(length/2),cy-(length/2),length/2,4*n+4);
}

void drawRects(struct Rectangle *r, int llx, int lly, int lx, int ly) {
	if (r->binSon[0] != NULL)
		drawRects(r->binSon[0], llx, lly, lx, ly);
		int x1 = r->Center[0]-r->Length[0];
		int y1 = r->Center[1]+r->Length[1];
		int x2 = r->Center[0]+r->Length[0];
		int y2 = r->Center[1]-r->Length[1];
		int px = x1;
		int py = y1+1;
		/* set offset of text */
		if (y1+2 >= lly+ly)
			py = y2-2;
		if (py-3 < 1){
			py = r->Center[1];
			if (x2+9 >= llx+lx)
				px = x1-9;
			else
				px = x2+1;
		}
		if (px < 1) {
			px = x1+1;
			py = y1-1;
		}
		if (px + 8 >= llx+lx)
			px = llx+lx-8;
		
		printf("LD(0,0)\n");
		printf("DR(%d,%d,%d,%d)\n", x1, y1, x2, y2);
		printf("DN(%s,%d,%d)\n", r->Name, px, py);
	if (r->binSon[1] != NULL)
		drawRects(r->binSon[1], llx, lly, lx, ly);
}

void drawWindow(int llx, int lly, int lx, int ly) {
	printf("LD(2,2)\n");
	printf("DR(%d,%d,%d,%d)\n", llx, lly+ly, llx+lx, lly);
}

/* IMPLEMENT */
void drawQuads(int llx, int lly, int lx, int ly, struct cNode *node, int cx, int cy, int length) {
	/* draw the quadtree as normal but without any rectangles and don't draw quad lines
		inside the window */
	/* then draw window and the rectangles in the tempTree "tree" */
	if (node->spcSon[0] != NULL || node->spcSon[1] != NULL || node->spcSon[2] != NULL || node->spcSon[3] != NULL) {
		/*draw grid for this square*/
		printf("LD(2,2)\n");
		/* bottom to top */
		if ((llx > cx) || (llx+lx < cx))
			printf("DL(%d,%d,%d,%d)\n", cx, cy-length, cx, cy+length);
		else {
			if (cy-length < lly) printf("DL(%d,%d,%d,%d)\n", cx, cy-length, cx, lly);
			if (lly+ly < cy+length) printf("DL(%d,%d,%d,%d)\n", cx, lly+ly, cx, cy+length);
		}
		/* left to right */
		if ((lly > cy) || (lly+ly < cy))
			printf("DL(%d,%d,%d,%d)\n", cx-length, cy, cx+length, cy);
		else {
			if (cx-length < llx) printf("DL(%d,%d,%d,%d)\n", cx-length, cy, llx, cy);
			if (llx+lx < cx+length) printf("DL(%d,%d,%d,%d)\n", llx+lx, cy, cx+length, cy);
		}
	}
	/*recursive on each child*/
	if (node->spcSon[0] != NULL)
		drawQuads(llx, lly, lx, ly, node->spcSon[0], cx-(length/2), cy+(length/2), length/2);
	if (node->spcSon[1] != NULL)
		drawQuads(llx, lly, lx, ly, node->spcSon[1], cx+(length/2), cy+(length/2), length/2);
	if (node->spcSon[2] != NULL)
		drawQuads(llx, lly, lx, ly, node->spcSon[2], cx-(length/2), cy-(length/2), length/2);
	if (node->spcSon[3] != NULL)
		drawQuads(llx, lly, lx, ly, node->spcSon[3], cx+(length/2), cy-(length/2), length/2);
}

void doWindow(int llx, int lly, int lx, int ly) {
	tempTree = NULL;
	window(llx, lly, lx, ly, &tempTree, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
		
	if (tracer) printf("\n");
	
	if (tempTree == NULL) {
		printf("NO RECTANGLES FALL IN WINDOW %d %d %d %d\n", llx, lly, lx, ly);
		
	} else {
		printf("WINDOW %d %d %d %d CONTAINS RECTANGLES", llx, lly, lx, ly);
		printoutRectTree(tempTree);
		printf("\n");
	}
	
	int length = looseQuadTree.World.Length[0];
	printf("$$$$ SP(%d, %d)\n", length*2, length*2);
	if (looseQuadTree.looseQuadRoot != NULL) {
		drawQuads(llx, lly, lx, ly, looseQuadTree.looseQuadRoot,
			looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
			looseQuadTree.World.Length[0]);
		drawWindow(llx, lly, lx, ly);
		drawRects(tempTree, llx, lly, lx, ly);
	}
	printf("EP\n");
	
	if (tempTree != NULL) freeTree(&tempTree);
}

void pushDown(struct cNode *node,int cx,int cy,int length,int n,int p) {
	if (node == NULL) return;
	
	if (tracer) printf("%d ", n);
	
	if (length < 1)
		return;
	
	if (node->binSon!=NULL && node->binSon->Rect!=NULL) {
		struct bNode *curr = node->binSon, *prev = NULL;
		int right, left, top, bottom, rcx, rcy, deleted;
		struct Rectangle *r;
		while (curr!=NULL) {
			r = curr->Rect;
			rcx = curr->Rect->Center[0];
			rcy = curr->Rect->Center[1];
			right = curr->Rect->Center[0] + curr->Rect->Length[0];
			left = curr->Rect->Center[0] - curr->Rect->Length[0];
			top = curr->Rect->Center[1] + curr->Rect->Length[1];
			bottom = curr->Rect->Center[1] - curr->Rect->Length[1];
			deleted = 0;
			
			if (rcx < cx && rcy < cy) { /* SW */
				if ((top < p*(length/2) + cy) && (right < p*(length/2) + cx) &&
				(left >= cx - length - (p*(length/2))) && (bottom >= cy - length - (p*(length/2)))) {
					deleted = 1;
					if (node->spcSon[2] == NULL) { /* creates quadrant */
						struct cNode *ptr = (struct cNode *)malloc(sizeof(struct cNode));
						ptr->spcSon[0] = ptr->spcSon[1] = ptr->spcSon[2] = ptr->spcSon[3] = NULL;
						ptr->binSon = NULL;
						node->spcSon[2] = ptr;
					}
					addRectToBNode(r, &(node->spcSon[2]->binSon));
				}
			} else if (rcx < cx && rcy >= cy) { /* NW */
				if ((bottom >= cy - (p*(length/2))) && (right < p*(length/2) + cx) &&
				(left >= cx - length - (p*(length/2))) && (top < p*(length/2) + cy + length)) {
					deleted = 1;
					if (node->spcSon[0] == NULL) { /* creates quadrant */
						struct cNode *ptr = (struct cNode *)malloc(sizeof(struct cNode));
						ptr->spcSon[0] = ptr->spcSon[1] = ptr->spcSon[2] = ptr->spcSon[3] = NULL;
						ptr->binSon = NULL;
						node->spcSon[0] = ptr;
					}
					addRectToBNode(r, &(node->spcSon[0]->binSon));
				}		
			} else if (rcx >= cx && rcy < cy) { /* SE */
				if ((top < cy + p*(length/2)) && (left >= cx - (p*(length/2))) &&
				(bottom >= cy - length - (p*(length/2))) && (right < p*(length/2) + cx+length)) {
					deleted = 1;
					if (node->spcSon[3] == NULL) { /* creates quadrant */
						struct cNode *ptr = (struct cNode *)malloc(sizeof(struct cNode));
						ptr->spcSon[0] = ptr->spcSon[1] = ptr->spcSon[2] = ptr->spcSon[3] = NULL;
						ptr->binSon = NULL;
						node->spcSon[3] = ptr;
					}
					addRectToBNode(r, &(node->spcSon[3]->binSon));
				}
			} else if (rcx >= cx && rcy >= cy) { /* NE */
				if ((bottom >= cy-(p*(length/2))) && (left >= cx-(p*(length/2))) &&
				(top < p*(length/2) + cy + length) && (right < p*(length/2) + cx+length)) {
					deleted = 1;
					if (node->spcSon[1] == NULL) { /* creates quadrant */
						struct cNode *ptr = (struct cNode *)malloc(sizeof(struct cNode));
						ptr->spcSon[0] = ptr->spcSon[1] = ptr->spcSon[2] = ptr->spcSon[3] = NULL;
						ptr->binSon = NULL;
						node->spcSon[1] = ptr;
					}
					addRectToBNode(r, &(node->spcSon[1]->binSon));
				}
			}
			if (deleted) {
				if (prev==NULL) {
					node->binSon = curr->next;
					free(curr);
					curr = node->binSon;
				} else {
					prev->next = curr->next;
					free(curr);
					curr = prev->next;
				}
			} else {
				prev = curr;
				curr = curr->next;
			}
		}
	}
	
	/* call next node */
	/* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		pushDown(node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1, p);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		pushDown(node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2, p);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		pushDown(node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3, p);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		pushDown(node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4, p);
	}
	
}

void popUp(struct cNode **node, int cx, int cy, int length, struct cNode **parent, int n, int p) {
	/* dig through tree, if rectangle no longer fits in this quad, remove and give to parent */
	/* traverse as far down as possible first, then perform checks */
	/* should pass in parent's cx and cy, length can be attained as 2x length */
	/* delete cnodes as necessary... */
	
	if (*node == NULL) return;
	
	if (tracer) printf("%d ", n);
	
	if ((*node)->spcSon[0] != NULL && (((*node)->spcSon[0]->spcSon[0] != NULL ||
		(*node)->spcSon[0]->spcSon[1] != NULL || (*node)->spcSon[0]->spcSon[2] != NULL ||
		(*node)->spcSon[0]->spcSon[3] != NULL) || (*node)->spcSon[0]->binSon != NULL)) {
		
		popUp(&(*node)->spcSon[0], cx-(length/2), cy+(length/2), length/2, node, 4*n+1, p);
	}
			
	/* NE */
	if ((*node)->spcSon[1] != NULL && (((*node)->spcSon[1]->spcSon[0] != NULL ||
		(*node)->spcSon[1]->spcSon[1] != NULL || (*node)->spcSon[1]->spcSon[2] != NULL ||
		(*node)->spcSon[1]->spcSon[3] != NULL) || (*node)->spcSon[1]->binSon != NULL)) {
		
		popUp(&(*node)->spcSon[1], cx+(length/2), cy+(length/2), length/2, node, 4*n+2, p);
	}
			
	/* SW */
	if ((*node)->spcSon[2] != NULL && (((*node)->spcSon[2]->spcSon[0] != NULL ||
		(*node)->spcSon[2]->spcSon[1] != NULL || (*node)->spcSon[2]->spcSon[2] != NULL ||
		(*node)->spcSon[2]->spcSon[3] != NULL) || (*node)->spcSon[2]->binSon != NULL)) {
		
		popUp(&(*node)->spcSon[2], cx-(length/2), cy-(length/2), length/2, node, 4*n+3, p);
	}
			
	/* SE */
	if ((*node)->spcSon[3] != NULL && (((*node)->spcSon[3]->spcSon[0] != NULL ||
		(*node)->spcSon[3]->spcSon[1] != NULL || (*node)->spcSon[3]->spcSon[2] != NULL ||
		(*node)->spcSon[3]->spcSon[3] != NULL) || (*node)->spcSon[3]->binSon != NULL)) {
		
		popUp(&(*node)->spcSon[3], cx+(length/2), cy-(length/2), length/2, node, 4*n+4, p);
	}
	
	/* now do the work */
	if ((*node)->binSon!=NULL && (*node)->binSon->Rect!=NULL) {
		struct bNode *curr = (*node)->binSon, *prev = NULL;
		int right, left, top, bottom, rcx, rcy, deleted;
		struct Rectangle *r;
		while (curr!=NULL) {
			r = curr->Rect;
			rcx = r->Center[0];
			rcy = r->Center[1];
			right = rcx + r->Length[0];
			left = rcx - r->Length[0];
			top = rcy + r->Length[1];
			bottom = rcy - r->Length[1];
			deleted = 0;
			
			/* if r doesn't fit in p-decreased node, set deleted and add to parent */
			if ((top >= cy + length + p*(length)) || (bottom < cy - length - p*(length)) ||
				(left < cx - length - p*(length)) || (right >= cx + length + p*(length))) {
					deleted = 1;
					addRectToBNode(r, &((*parent)->binSon));
				}
			
			if (deleted) {
				if (prev==NULL) {
					(*node)->binSon = curr->next;
					free(curr);
					curr = (*node)->binSon;
				} else {
					prev->next = curr->next;
					free(curr);
					curr = prev->next;
				}
			} else {
				prev = curr;
				curr = curr->next;
			}
		}
	}
	if ((*node)->spcSon[0] == NULL && (*node)->spcSon[1] == NULL &&
		(*node)->spcSon[2] == NULL && (*node)->spcSon[3] == NULL && (*node)->binSon == NULL) {
		free(*node);
		*node = NULL;
	}
}

void doChangeExpFactor(double p) {
	/* traverse whole tree, if p is increased, check to move rectangle to deeper node;
		if p is decreased, check if rectangle needs to be moved up node(s) */
	if (p > looseQuadTree.p) {
		looseQuadTree.p = p;
		pushDown(looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0, p);
	} else if (p < looseQuadTree.p) {
		looseQuadTree.p = p;
		popUp(&looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], NULL, 0, p);
	} else
		return;
	if (tracer) printf("\n");
}

void buildNearestNodeList(struct Rectangle *r,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node == NULL) return;
	
	/* traverse entire tree, adding all nodes to nodeList, calculating horiz distance from r */
	/* pass list, node, distance, id */
	addToNodeList(nodelist, node, sqrt(pow((r->Center[0]-cx),2)+pow((r->Center[1]-cy),2)), n);
	/* NW */
	if (node->spcSon[0] != NULL && ((node->spcSon[0]->spcSon[0] != NULL || node->spcSon[0]->spcSon[1] != NULL ||
		node->spcSon[0]->spcSon[2] != NULL || node->spcSon[0]->spcSon[3] != NULL) ||
		node->spcSon[0]->binSon != NULL)) {
		
		buildNearestNodeList(r, nodelist, node->spcSon[0], cx-(length/2), cy+(length/2), length/2, 4*n+1);
	}
			
	/* NE */
	if (node->spcSon[1] != NULL && ((node->spcSon[1]->spcSon[0] != NULL || node->spcSon[1]->spcSon[1] != NULL ||
		node->spcSon[1]->spcSon[2] != NULL || node->spcSon[1]->spcSon[3] != NULL) ||
		node->spcSon[1]->binSon != NULL)) {
		
		buildNearestNodeList(r, nodelist, node->spcSon[1], cx+(length/2), cy+(length/2), length/2, 4*n+2);
	}
			
	/* SW */
	if (node->spcSon[2] != NULL && ((node->spcSon[2]->spcSon[0] != NULL || node->spcSon[2]->spcSon[1] != NULL ||
		node->spcSon[2]->spcSon[2] != NULL || node->spcSon[2]->spcSon[3] != NULL) ||
		node->spcSon[2]->binSon != NULL)) {
		
		buildNearestNodeList(r, nodelist, node->spcSon[2], cx-(length/2), cy-(length/2), length/2, 4*n+3);
	}
			
	/* SE */
	if (node->spcSon[3] != NULL && ((node->spcSon[3]->spcSon[0] != NULL || node->spcSon[3]->spcSon[1] != NULL ||
		node->spcSon[3]->spcSon[2] != NULL || node->spcSon[3]->spcSon[3] != NULL) ||
		node->spcSon[3]->binSon != NULL)) {
		
		buildNearestNodeList(r, nodelist, node->spcSon[3], cx+(length/2), cy-(length/2), length/2, 4*n+4);
	}
}


/* traverse entire tree adding nodes to priority list, then add each rectangle in those nodes to rect list */
void nearestNeighbor(struct Rectangle *r,struct List **list,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node==NULL) return;
	
	buildNearestNodeList(r, nodelist, node, cx, cy, length, n);
	
	double distance;
	
	int rright = r->Center[0]+r->Length[0];
	int rleft = r->Center[0]-r->Length[0];
	int rtop = r->Center[1]+r->Length[1];
	int rbottom = r->Center[1]-r->Length[1];
	
	struct cNodeList *currNode = dequeue(nodelist);
	struct cNode *curr;
	while (currNode != NULL) {
		if (tracer) printf("%d ", currNode->id);
		
		curr = currNode->node;
		
		if (curr->binSon!=NULL && curr->binSon->Rect!=NULL) {
			struct bNode *b = curr->binSon;
			int right, left, top, bottom;
			while (b!=NULL) {
				right = b->Rect->Center[0] + b->Rect->Length[0];
				left = b->Rect->Center[0] - b->Rect->Length[0];
				top = b->Rect->Center[1] + b->Rect->Length[1];
				bottom = b->Rect->Center[1] - b->Rect->Length[1];
			
				/* calculate distance */
				if ((r->Center[1] <= top && r->Center[1] >= bottom) || /* on same horizontal plane */
					(b->Rect->Center[1] <= rtop && b->Rect->Center[1] >= rbottom))
					if (r->Center[0] <= b->Rect->Center[0]) /* neighbor is to right */
						distance = left - rright;
					else /* neighbor is to left */
						distance = rleft - right;
				else if ((r->Center[0] <= right && r->Center[0] >= left) || /* on same vertical plane */
					(b->Rect->Center[0] <= rright && b->Rect->Center[1] >= rleft))
					if (r->Center[1] <= b->Rect->Center[1]) /* neighbor is above */
						distance = bottom - rtop;
					else /* neighbor is below */
						distance = rbottom - top;
				else { /* neighbor is diagnal, so find nearest corner */
					if (r->Center[0] < b->Rect->Center[0]) /*left*/
						if (r->Center[1] < b->Rect->Center[1]) /*bottom*/
							distance = sqrt(pow(rright-left,2) + pow(rtop-bottom,2));
						else /*left top*/
							distance = sqrt(pow(rright-left,2) + pow(rbottom-top,2));
					else/*right*/
						if (r->Center[1] < b->Rect->Center[1]) /*bottom*/
							distance = sqrt(pow(rleft-right,2) + pow(rtop-bottom,2));
						else /* right top */
							distance = sqrt(pow(rleft-right,2) + pow(rbottom-top,2));
				}
				
				if (strcmp(b->Rect->Name,r->Name))
					if (distance >= 0)
					addToList(list, b->Rect, distance);
				
				b = b->next;
			}
		}
		
		free(currNode);
		currNode = dequeue(nodelist);
	}
}

void doNearestNeighbor(char *name) {
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("NO RECTANGLE %s EXISTS\n", name);
		return;
	}
	tempList = NULL;
	tempNodeList = NULL;
	
	//call function
	nearestNeighbor(r, &tempList, &tempNodeList, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	
	if (tracer) printf("\n");
	
	if (tempList == NULL) {
		printf("NO NEAREST NEIGHBOR O RECTANGLE %s FOUND\n", name);
	} else {
		printf("THE NEAREST NEIGHBOR TO RECTANGLE %s IS RECTANGLE %s\n", name, tempList->rect->Name);
		freeList(&tempList);
	}
	if (tempNodeList != NULL)
		freeNodeList(&tempNodeList);
}

/* traverse entire tree adding nodes to priority list, then add each rectangle in those nodes to rect list */
void lexiNeighbor(struct Rectangle *r,struct List **list,struct cNodeList **nodelist,
	struct cNode *node,int cx,int cy,int length,int n)
{
	if (node==NULL) return;
	
	buildNearestNodeList(r, nodelist, node, cx, cy, length, n);
	
	double distance;
	
	int rright = r->Center[0]+r->Length[0];
	int rleft = r->Center[0]-r->Length[0];
	int rtop = r->Center[1]+r->Length[1];
	int rbottom = r->Center[1]-r->Length[1];
	
	struct cNodeList *currNode = dequeue(nodelist);
	struct cNode *curr;
	while (currNode != NULL) {
		if (tracer) printf("%d ", currNode->id);
		
		curr = currNode->node;
		
		if (curr->binSon!=NULL && curr->binSon->Rect!=NULL) {
			struct bNode *b = curr->binSon;
			int right, left, top, bottom;
			while (b!=NULL) {
			
				if (my_strcmp(b->Rect->Name,r->Name) <= 0) {
					b = b->next;
					continue;
				}
			
				right = b->Rect->Center[0] + b->Rect->Length[0];
				left = b->Rect->Center[0] - b->Rect->Length[0];
				top = b->Rect->Center[1] + b->Rect->Length[1];
				bottom = b->Rect->Center[1] - b->Rect->Length[1];
			
				/* calculate distance */
				if ((r->Center[1] <= top && r->Center[1] >= bottom) || /* on same horizontal plane */
					(b->Rect->Center[1] <= rtop && b->Rect->Center[1] >= rbottom))
					if (r->Center[0] <= b->Rect->Center[0]) /* neighbor is to right */
						distance = left - rright;
					else /* neighbor is to left */
						distance = rleft - right;
				else if ((r->Center[0] <= right && r->Center[0] >= left) || /* on same vertical plane */
					(b->Rect->Center[0] <= rright && b->Rect->Center[1] >= rleft))
					if (r->Center[1] <= b->Rect->Center[1]) /* neighbor is above */
						distance = bottom - rtop;
					else /* neighbor is below */
						distance = rbottom - top;
				else { /* neighbor is diagnal, so find nearest corner */
					if (r->Center[0] < b->Rect->Center[0]) /*left*/
						if (r->Center[1] < b->Rect->Center[1]) /*bottom*/
							distance = sqrt(pow(rright-left,2) + pow(rtop-bottom,2));
						else /*left top*/
							distance = sqrt(pow(rright-left,2) + pow(rbottom-top,2));
					else/*right*/
						if (r->Center[1] < b->Rect->Center[1]) /*bottom*/
							distance = sqrt(pow(rleft-right,2) + pow(rtop-bottom,2));
						else /* right top */
							distance = sqrt(pow(rleft-right,2) + pow(rbottom-top,2));
				}
				
				if (distance >= 0)
					addToList(list, b->Rect, distance);
				
				b = b->next;
			}
		}
		
		free(currNode);
		currNode = dequeue(nodelist);
	}
}

void doLexicallyGreaterNearestNeighbor(char *name) {
	struct Rectangle *r = *(findRect(name, &rectTree));
	if (r==NULL) {
		printf("NO RECTANGLE %s EXISTS\n", name);
		return;
	}
	tempList = NULL;
	tempNodeList = NULL;
	
	//call function
	lexiNeighbor(r, &tempList, &tempNodeList, looseQuadTree.looseQuadRoot,
		looseQuadTree.World.Center[0], looseQuadTree.World.Center[1],
		looseQuadTree.World.Length[0], 0);
	
	if (tracer) printf("\n");
	
	if (tempList == NULL) {
		printf("NO LEXICALLY GREATER NEAREST NEIGHBOR TO RECTANGLE %s FOUND\n", name);
	} else {
		printf("THE LEXICALLY GREATER NEAREST NEIGHBOR TO RECTANGLE %s IS RECTANGLE %s\n", name, tempList->rect->Name);
		freeList(&tempList);
	}
	if (tempNodeList != NULL)
		freeNodeList(&tempNodeList);
}

void doRaytrace(int a, int b) {
	printf("COMMAND RAYTRACE IS NOT IMPLEMENTED\n");
}

void doTrace(char *flag) {
	if (strcmp(flag, "ON\n")==0)
		tracer = 1;
	else if (strcmp(flag, "OFF\n")==0)
		tracer = 0;
}

void parseCommand(char *cmd) {
	int args = 0;
	char *splitter;
	char name[MAX_NAME_LEN+1];
	int argOne, argTwo, argThree, argFour;
	double argFive;
	splitter = strtok (cmd," (,)");
	if(strcmp(splitter,"INIT_QUADTREE")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0) {
					argOne = atoi(splitter);
					args++;
				}
				else if(args==1) {
					argFive = atof(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doInitQuadtree(argOne,argFive);
	}
	else if(strcmp(splitter,"DISPLAY")==0) {
		doDisplay();
	}
	else if(strcmp(splitter,"LIST_RECTANGLES")==0) {
		doListRect();
	}
	else if(strcmp(splitter,"CREATE_RECTANGLE")==0) {
		splitter = strtok(NULL," (,)");
		while(splitter!=NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0) {
					strcpy(name,splitter);
					args++;
				}
				else if(args == 1) {
					argOne = atoi(splitter);
					args++;
				}
				else if(args == 2) {
					argTwo = atoi(splitter);
					args++;
				}
				else if(args == 3) {
					argThree = atoi(splitter);
					args++;
				}
				else if(args == 4) {
					argFour = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doCreateRect(name,argOne,argTwo,argThree,argFour);
	}
	else if(strcmp(splitter,"INSERT")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doInsert(name);
	}
	else if(strcmp(splitter,"TRACE")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doTrace(name);
	}
	else if(strcmp(splitter,"MOVE")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
				else if(args==1){
					argOne = atoi(splitter);
					args++;
				}
				else if(args==2){
					argTwo = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doMove(name, argOne, argTwo);
		
	}
	else if(strcmp(splitter,"SEARCH_POINT")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					argOne = atoi(splitter);
					args++;
				}
				else if(args==1){
					argTwo = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doSearchPoint(argOne,argTwo);
	}
	else if(strcmp(splitter,"DELETE_RECTANGLE")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doDeleteRect(name);
	}
	else if(strcmp(splitter,"DELETE_POINT")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					argOne = atoi(splitter);
					args++;
				}
				else if(args==1){
					argTwo = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doDeletePoint(argOne,argTwo);
	}
	else if(strcmp(splitter,"REGION_SEARCH")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doRegionSearch(name);
	}
	else if(strcmp(splitter,"TOUCH")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doTouch(name);
	}
	else if(strcmp(splitter,"WITHIN")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
				else if(args==1){
					argOne = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doWithin(name,argOne);
	}
	else if(strcmp(splitter,"HORIZ_NEIGHBOR")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doHorizNeighbor(name);
	}
	else if(strcmp(splitter,"VERT_NEIGHBOR")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doVertNeighbor(name);
	}
	else if(strcmp(splitter,"NEAREST_RECTANGLE")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					argOne = atoi(splitter);
					args++;
				}
				else if(args==1){
					argTwo = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doNearestRect(argOne,argTwo);
	}
	else if(strcmp(splitter,"WINDOW")==0) {
		splitter = strtok(NULL," (,)");
		while(splitter!=NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args == 0) {
					argOne = atoi(splitter);
					args++;
				}
				else if(args == 1) {
					argTwo = atoi(splitter);
					args++;
				}
				else if(args == 2) {
					argThree = atoi(splitter);
					args++;
				}
				else if(args == 3) {
					argFour = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doWindow(argOne,argTwo,argThree,argFour);
	}
	else if(strcmp(splitter,"CHANGE_EXPANSION_FACTOR")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					argFive = atof(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doChangeExpFactor(argFive);
	}
	else if(strcmp(splitter,"NEAREST_NEIGHBOR")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doNearestNeighbor(name);
	}
	else if(strcmp(splitter,"LEXICALLY_GREATER_NEAREST_NEIGHBOR")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					strcpy(name,splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doLexicallyGreaterNearestNeighbor(name);
	}
	else if(strcmp(splitter,"RAYTRACE")==0) {
		splitter = strtok(NULL," (,)");
		while (splitter != NULL) {
			if(strcmp(splitter,"")!=0) {
				if(args==0){
					argOne = atoi(splitter);
					args++;
				}
				else if(args==1){
					argTwo = atoi(splitter);
					args++;
				}
			}
			splitter = strtok(NULL," (,)");
		}
		doRaytrace(argOne,argTwo);
	}
}

int main(void) {
	initLooseQuadTree();
	initRectTree();
	tracer = 0;
	
	char text[MAX_STRING_LEN+1];
	
	while(fgets(text, sizeof text, stdin) != 0) {
		parseCommand(text);
	}
	/*
	while(1) {
		fgets(text, sizeof text, stdin);
		if(strcmp(text, "\n") == 0) {
			printf("\n");
			break;
		} else {
			parseCommand(text);
		}
	}*/
	exit(0);
}
