#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

void left_rotate(rbtree *t , node_t * x) {
  node_t *y = x->right;
  x->right = y->left;

  if ( y->left != t->nil ) {
    y->left->parent = x; // y.left 가 있으면 parent 연결 해줘야됨
  }

  y->parent = x->parent; // 부모를 대신 연결

  // 부모노드의 자식정보도 바꿔줌
  if ( x->parent == t->nil ) { // x의 parent 가 없으면 root 란 뜻
    t->root = y;
  } else if ( x == x->parent->left ) {// 왼쪽자식이면 
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

// 왼쪽 회전과 똑같지만 반대
void right_rotate(rbtree *t , node_t *x) {
  node_t *y = x->left;
  x->left = y->right;

  if ( y->right != t->nil ) {
    y->right->parent = x;
  }

  y->parent = x->parent;

  if ( x->parent == t->nil ) {
    t->root = y;
  } else if ( x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}

rbtree *new_rbtree(void) {
  // 기본 rbtree 구조체 생성 root 와 nil을 가리킴
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // nil 을 color = black 을 가지는 놈으로 생성
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil->right = p->nil->parent = p->nil;

  // root에도 초기값 nil 을 설정해줌 실제로는 nil 의 주소값
  p->root = p->nil;

  return p;
}

void delete_node(rbtree *t , node_t* node) {
  if ( node == t->nil ) return;
  delete_node(t,node->left);
  delete_node(t,node->right);
  free(node);
}

void delete_rbtree(rbtree *t) {
  delete_node(t, t->root); // 재귀적으로 노드해제
  free(t->nil); // nil 노드 free 로 메모리 해제
  free(t); // 그 다음 rbtree 구조체 free
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 1. 이진탐색트리처럼 값을 비교해서 노드를 삽입한다.

  node_t *current_node = t->root; // 루트부터 시작
  node_t *parrent_node = t->nil; // 루트의 부모 기본 t.nil

  // 현재 노드가 NIL 노드가 될 때까지 반복 부모의 정보 저장
  while ( current_node != t->nil ) {
    parrent_node = current_node;

    if ( key < current_node->key ) {
        current_node = current_node->left;
    } else {
        current_node = current_node->right;
    }
  }

  // 새 노드를 만들어서 삽입
  node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
  new_node->parent = parrent_node;
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->key = key;
  new_node->color = RBTREE_RED;

  if ( parrent_node == t->nil ) { // 트리의 루트가 비어있다면
    t->root = new_node;
  } else if ( key < parrent_node->key ) { // key 값이 더 작다면
    parrent_node->left = new_node;
  } else { // key 값이 더 크거나 같다면
    parrent_node->right = new_node;
  }

  // 2. Red-Black-Tree의 규칙을 위반했는지 확인하고 fix-up 을 통해 개선한다.
  // 삽입된 노드는 항상 Red 이기 때문에, 우선 부모가 Red 인지를 확인하고 그 후에 3가지 케이스로 나눈다.
  while ( new_node->parent->color == RBTREE_RED ) {
    // 삽입된 노드의 부모가 왼쪽 자식인 경우
    if ( new_node->parent == new_node->parent->parent->left) {
      node_t *uncle = new_node->parent->parent->right;
      // 1. 삼촌이 Red 인 경우 
      if ( uncle->color == RBTREE_RED ) {
        // 입력노드의 부모가 RED 인게 문제인 거니까 , 부모랑 삼촌이 같으면 부모의색깔을 Blakc으로 바꿔주고
        // 할아버지를 Red 로 바꿔줘서 문제를 해결하는 거임.
        new_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        
        // 그리고 할아버지가 Red로 바뀌었으니까 그 Red를 기준으로 또 새로 체크해줌
        new_node = new_node->parent->parent;
      } else {
        // 삼촌이 Black 이고
        // 입력된 노드가 오른쪽 자식인 경우 즉 꺽쇠인 경우는 미리 회전을 시켜줘서 일직선을 만들어 줘야함
        if ( new_node == new_node->parent->right ) {
          new_node = new_node->parent;
          left_rotate(t,new_node);
        }

        //
        new_node->parent->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        right_rotate(t, new_node->parent->parent);
      }
    } else {
      // 오른쪽 자신인 경우이고 나머진 위와 같음
      node_t *uncle = new_node->parent->parent->left;
      
      if ( uncle->color == RBTREE_RED ) {
        new_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;

        new_node = new_node->parent->parent;
      } else {
        if ( new_node == new_node->parent->left ) {
            new_node = new_node->parent;
            right_rotate(t, new_node);
        }
        new_node->parent->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        left_rotate(t, new_node->parent->parent);
      }
    }
  }  

  t->root->color = RBTREE_BLACK;
  // root 가 변경될 가능성이 있어서 root 를 return 하나?
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current_node = t->root;
  
  while ( current_node != t->nil ) {
    if ( current_node->key == key ) return current_node;

    if ( key < current_node->key ) {
        current_node = current_node->left;
    } else {
        current_node = current_node->right;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *min_node = t->nil;
  node_t *current_node = t->root;

  while ( current_node != t->nil ) {
    min_node = current_node;
    current_node = current_node->left;
  }

  return min_node;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *max_node = t->nil;
  node_t *current_node = t->root;

  while ( current_node != t->nil ) {
    max_node = current_node;
    current_node = current_node->right;
  }

  return max_node;
}

// 후임자(successor) 찾기 함수
node_t *find_successor(rbtree *t, node_t *node) {
  if (node->right != t->nil) {
    // 오른쪽 서브트리가 있으면 오른쪽 서브트리의 최솟값
    node_t *current = node->right;
    while (current->left != t->nil) {
      current = current->left;
    }
    return current;
  }
  
  // 오른쪽 서브트리가 없으면 부모를 따라 올라가면서 첫 번째 오른쪽 자식인 부모 찾기
  node_t *parent = node->parent;
  while (parent != t->nil && node == parent->right) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

// 삭제 후 색상 조정 함수
void fix_delete(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      // x가 왼쪽 자식인 경우
      node_t *w = x->parent->right; // 형제 노드
      
      if (w->color == RBTREE_RED) {
        // 케이스 1: 형제가 빨간색
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        // 케이스 2: 형제의 두 자식이 모두 검은색
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->right->color == RBTREE_BLACK) {
          // 케이스 3: 형제의 오른쪽 자식이 검은색
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // 케이스 4: 형제의 오른쪽 자식이 빨간색
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    } else {
      // x가 오른쪽 자식인 경우 (왼쪽과 대칭)
      node_t *w = x->parent->left;
      
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  if (p == NULL || p == t->nil) {
    return 0; // 삭제할 노드가 없음
  }
  
  node_t *y = p; // 실제로 삭제될 노드
  node_t *x;     // y의 자식 노드 (색상 조정 대상)
  color_t y_original_color = y->color;
  
  if (p->left == t->nil) {
    // 왼쪽 자식이 없거나 둘 다 없는 경우
    x = p->right;
    // p를 x로 대체
    if (p->parent == t->nil) {
      t->root = x;
    } else if (p == p->parent->left) {
      p->parent->left = x;
    } else {
      p->parent->right = x;
    }
    x->parent = p->parent;
  } else if (p->right == t->nil) {
    // 오른쪽 자식이 없는 경우
    x = p->left;
    // p를 x로 대체
    if (p->parent == t->nil) {
      t->root = x;
    } else if (p == p->parent->left) {
      p->parent->left = x;
    } else {
      p->parent->right = x;
    }
    x->parent = p->parent;
  } else {
    // 양쪽 자식이 모두 있는 경우
    y = find_successor(t, p);
    y_original_color = y->color;
    x = y->right;
    
    if (y->parent == p) {
      // y가 p의 직접적인 자식인 경우
      x->parent = y;
    } else {
      // y가 p의 자식이 아닌 경우
      if (y->parent == t->nil) {
        t->root = x;
      } else if (y == y->parent->left) {
        y->parent->left = x;
      } else {
        y->parent->right = x;
      }
      x->parent = y->parent;
      
      // y를 p 위치로 이동
      y->right = p->right;
      y->right->parent = y;
    }
    
    // p를 y로 대체
    if (p->parent == t->nil) {
      t->root = y;
    } else if (p == p->parent->left) {
      p->parent->left = y;
    } else {
      p->parent->right = y;
    }
    y->parent = p->parent;
    
    // y의 왼쪽 자식 설정
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  
  // 색상 조정이 필요한 경우
  if (y_original_color == RBTREE_BLACK) {
    fix_delete(t, x);
  }
  
  // 삭제된 노드 메모리 해제
  free(p);
  
  return 0;
}

// 중위 순회를 위한 헬퍼 함수
void in_order_traversal(const rbtree *t, node_t* root, key_t *arr, int *count, const size_t n) {
  if (root == t->nil || *count >= n) return;

  // 왼쪽 서브트리 순회
  in_order_traversal(t, root->left, arr, count, n);
  
  // 현재 노드 처리 (배열 크기 제한 확인)
  if (*count < n) {
    arr[*count] = root->key;
    (*count)++;
  }
  
  // 오른쪽 서브트리 순회
  in_order_traversal(t, root->right, arr, count, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  
  int count = 0;
  in_order_traversal(t, t->root, arr, &count, n);
  
  return count;
}
