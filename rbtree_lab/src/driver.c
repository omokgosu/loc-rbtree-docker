#include "rbtree.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    // RB 트리생성
    rbtree *tree = new_rbtree();

    printf("트리생성 완료");
    printf("root : %p\n" , tree->root);
    printf("nil : %p\n" , tree->nil);
    printf("nil color: %d\n", tree->nil->color);

    delete_rbtree(tree);
}