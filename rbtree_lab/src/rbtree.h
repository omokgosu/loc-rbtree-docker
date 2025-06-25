#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

// rb 트리의 노드들
typedef struct node_t {
  color_t color; // color 타입
  key_t key; // 키 (입력값 value)
  struct node_t *parent, *left, *right; // 부모,왼쪽,오른쪽 노드 포인터
} node_t;

typedef struct {
  node_t *root; // rbtree 루트를 가리킴
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
