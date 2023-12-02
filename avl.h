#pragma once
#include <iostream>
#include <vector>

struct avl {
  std::string elem;
  int type;
  avl *left = nullptr;
  avl *right = nullptr;

  int h(avl *t) {
    if (t == nullptr)
      return 0;
    return std::max(h(t->left), h(t->right)) + 1;
  }

  void smallRotateRight(avl *&t) {
    avl *p = t->left;
    t->left = p->right;
    p->right = t;
    t = p;
  }

  void smallRotateLeft(avl *&t) {
    avl *p = t->right;
    t->right = p->left;
    p->left = t;
    t = p;
  }

  void bigRotateRight(avl *&t) {
    avl *p = t->left;
    avl *q = p->right;
    t->left = q->right;
    p->right = q->left;
    q->right = t;
    q->left = p;
    t = q;
  }

  void bigRotateLeft(avl *&t) {
    avl *p = t->right;
    avl *q = p->left;
    t->right = q->left;
    p->left = q->right;
    q->left = t;
    q->right = p;
    t = q;
  }

  void balance(avl *&t) {
    if (t == nullptr)
      return;
    int hr = h(t->right);
    int hl = h(t->left);
    if (abs(hr - hl) <= 1) {
      return;
    }
    if (hl > hr) {
      if (t->left != nullptr) {
        if (h(t->left->left) < h(t->left->right)) {
          bigRotateRight(t);
          return;
        }
      }
      smallRotateRight(t);
    } else if (hl < hr) {
      if (t->right != nullptr) {
        if (h(t->right->left) > h(t->right->right)) {
          bigRotateLeft(t);
          return;
        }
      }
      smallRotateLeft(t);
    }
  }

  void insert(avl *&t, std::pair<std::string, int> x) {
    if (t == nullptr) {
      t = new avl;
      t->elem = x.first;
      t->type = x.second;
      return;
    }
    if (x.first > t->elem) {
      if (t->right == nullptr) {
        t->right = new avl;
        t->right->elem = x.first;
        t->right->type = x.second;
        balance(t);
        return;
      }
      insert(t->right, x);
    } else if (x.first < t->elem) {
      if (t->left == nullptr) {
        t->left = new avl;
        t->left->elem = x.first;
        t->left->type = x.second;
        balance(t);
        return;
      }
      insert(t->left, x);
    }
    balance(t);
    return;
  }

  int find(avl *t, std::string s) {
    if (t == nullptr)
      return -1;
    if (t->elem == s)
      return t->type;
    else if (s < t->elem)
      return find(t->left, s);
    else
      return find(t->right, s);
  }
};
