#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bin-tree.h"
#include "mem.h"

struct node {
    const void *key;
    void *value;
    struct node *L;
    struct node *R;
};

struct BinTree {
    struct node *root;
    CmpFunction cmp;
};

static int
BinTree_insert_under(BinTree tree, struct node *root, struct node *n);

static void *
BinTree_lookup_under(BinTree tree, struct node *root, const char *key);

static void
BinTree_free_under(struct node *root);

static int
node_cmp(const void *a, const void *b);

BinTree
BinTree_init(CmpFunction cmp)
{
    BinTree tree;
    NEW(tree);
    tree->root = NULL;
    tree->cmp = cmp == NULL ? node_cmp : cmp;
    return tree;
}

int
BinTree_insert(BinTree tree, const void *key, void *value)
{
    assert(tree);
    struct node *root = tree->root;
    struct node *n;
    NEW(n);
    n->key = key;
    n->value = value;
    n->L = n->R = NULL;
    int cmp;

    if (root == NULL) {
        root = n;
    } else {
        cmp = tree->cmp(key, root->key);
        if (cmp < 0) {
            if (root->L == NULL) {
                root->L = n;
                return 0;
            } else {
                BinTree_insert_under(tree, root->L, n);
            }
        } else if (cmp > 0) {
            if (root->R == NULL) {
                root->R = n;
                return 0;
            } else {
                BinTree_insert_under(tree, root->R, n);
            }
        } else {
            /* duplicate */
        }
    }
    return 0;
}

static int
BinTree_insert_under(BinTree tree, struct node *root, struct node *n)
{
    int cmp = tree->cmp(n->key, root->key);
    if (cmp < 0) {
        if (root->L == NULL) {
            root->L = n;
            return 0;
        } else {
            return BinTree_insert_under(tree, root->L, n);
        }
    } else if (cmp > 0) {
        if (root->R == NULL) {
            root->R = n;
            return 0;
        } else {
            return BinTree_insert_under(tree, root->R, n);
        }
    } else {
        /* duplicate */
        return 0;
    }
}

void *
BinTree_lookup(BinTree tree, const void *key)
{
    assert(tree);
    struct node *root = tree->root;
    int cmp;
    if (root == NULL) {
        return NULL;
    } else {
        cmp = tree->cmp(key, root->key);
        if (cmp == 0) {
            return root->value;
        } else if (cmp < 0) {
            if (root->L == NULL) {
                return NULL;
            } else {
                return BinTree_lookup_under(tree, root->L, key);
            }
        } else {
            if (root->R == NULL) {
                return NULL;
            } else {
                return BinTree_lookup_under(tree, root->R, key);
            }
        }
    }
}

static void *
BinTree_lookup_under(BinTree tree, struct node *root, const char *key)
{
    int cmp = tree->cmp(key, root->key);
    if (cmp == 0) {
        return root->value;
    } else if (cmp < 0) {
        if (root->L == NULL) {
            return NULL;
        } else {
            return BinTree_lookup_under(tree, root->L, key);
        }
    } else {
        if (root->R == NULL) {
            return NULL;
        } else {
            return BinTree_lookup_under(tree, root->R, key);
        }
    }
}

void
BinTree_free(BinTree *tree)
{
    assert(tree && *tree);
    struct node *root = (*tree)->root;
    if (root != NULL) {
        BinTree_free_under(root);
        FREE(root);
    } else {
        /* null tree */
    }
    FREE(*tree);
}

static void
BinTree_free_under(struct node *root)
{
    if (root->L != NULL) {
        BinTree_free_under(root->L);
    }
    if (root->R != NULL) {
        BinTree_free_under(root->R);
    }
    FREE(root);
}

static int
node_cmp(const void *a, const void *b)
{
    return a != b;
}
