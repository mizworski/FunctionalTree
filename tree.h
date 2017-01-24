#ifndef JNP7_TREE_H
#define JNP7_TREE_H


#include <memory>
#include <stack>

template<typename T>
class Tree {
    using tsize = size_t;
    using tree_ptr = std::shared_ptr<Tree<T>>;
    using tnode = Tree<T>;
public:
    Tree() {}

    Tree(tree_ptr root) {

    }

    Tree(const Tree &other) {

    }
//    ~Tree() {}

    // todo const T& instead of T?
    T fold(std::function<T(T, T, T)> operation, T init) {
        return init;
    }

    Tree<T> filter(std::function<bool(T)> predicate) {
        return *this;
    }

    Tree<T> map(std::function<T(T)> transformer) {
        return *this;
    }

    Tree<T> lazy_map(std::function<T(T)> transformer) {
        return *this;
    }

    T accumulate(std::function<T(T, T)> operation,
                 T init,
                 std::function<tnode(tnode, std::stack<tnode> &)> traversal) { //todo traversal type?
        return init;
    }

    void apply(std::function<void(T)> operation, std::function<tnode(tnode, std::stack<tnode> &)> traversal) {

    }

    tsize height() {
        return 0;
    }

    tsize size() {
        return 0;
    }

    bool is_bst() {
        return false;
    }

    void print(std::function<tnode(tnode, std::stack<tnode> &)> traversal = Tree<T>::inorder) {

    }

    static tnode preorder(tnode node, std::stack<tnode> nodes_to_traverse) { //todo dlaczego nie stack &?
        return Tree<T>();
    }

    static tnode inorder(tnode node, std::stack<tnode> nodes_to_traverse) {
        return Tree<T>();
    }

    static tnode postorder(tnode node, std::stack<tnode> nodes_to_traverse) {
        return Tree<T>();
    }

    static tnode createEmptyNode() {
        return Tree<T>();
    }

    static tnode createValueNode(T value) {
        return Tree<T>();
    }

    static tnode createValueNode(T value, tnode left, tnode right) {
        return Tree<T>();
    }

private:

};

#endif //JNP7_TREE_H
