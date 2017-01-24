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
    /**
     * Creates empty tree.
     */
    Tree() {}

    /**
     * Creates new tree with given root.
     * @param root root of new tree.
     */
    Tree(tree_ptr root) {

    }

    /**
     * Creates new tree which is deep copy of given tree.
     * @param other
     */
    Tree(const Tree &other) {

    }
//    ~Tree() {}

    // todo const T& instead of T?
    /**
     * Invokes given operation on tree in post order traversal and returns its result.
     * @param   operation ternary function which is applied to node value,
     *          value of function invoked on left subtree and value of function invoked on right subtree
     * @param   init value of empty node
     * @return  results of function invoked on root
     */
    T fold(std::function<T(T, T, T)> operation, T init) {
        return init;
    }

    /**
     * Creates new tree with elements that satisfy given predicate. If node didn't satisfy predicate had two sons,
     * then right subtree will be attached to most right node in left subtree.
     * @param   predicate function taking value of node and returning boolean
     * @return  new tree
     */
    Tree<T> filter(std::function<bool(T)> predicate) {
        return *this;
    }

    /**
     * Creates new tree with elements that were transformed by given function.
     * @param transformer   transforming function
     * @return              new tree
     */
    Tree<T> map(std::function<T(T)> transformer) {
        return *this;
    }

    /**
     * Creates new tree with elements that were transformed by given function. Transforming function
     * is invoked only once and when node value is accessed.
     * @param transformer   transforming function
     * @return              new tree
     */
    Tree<T> lazy_map(std::function<T(T)> transformer) {
        return *this;
    }

    /**
     * Applies binary operation to init and value in each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param init          initial value of accumulator
     * @param traversal     function which determines traversal
     * @return              value of accumulator after traversing tree
     */
    T accumulate(std::function<T(T, T)> operation,
                 T init,
                 std::function<tnode(tnode, std::stack<tnode> &)> traversal) { //todo traversal type?
        return init;
    }

    /**
     * Applies unary function operation to each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param traversal     function which determines traversal
     */
    void apply(std::function<void(T)> operation, std::function<tnode(tnode, std::stack<tnode> &)> traversal) {

    }

    /**
     * Gets height of tree which is maximal distance between root and leaf.
     * @return height of tree
     */
    tsize height() {
        return 0;
    }

    /**
     * Gets size of tree which is number of nodes in that tree.
     * @return size of tree
     */
    tsize size() {
        return 0;
    }

    /**
     * Checks if tree is binary search tree.
     * @return true if tree is binary search tree
     */
    bool is_bst() {
        return false;
    }

    /**
     * Prints value in each node of tree in given order. By default goes in order through tree.
     * @param traversal function which determines traversal
     */
    void print(std::function<tnode(tnode, std::stack<tnode> &)> traversal = Tree<T>::inorder) {

    }

    //todo
    static tnode preorder(tnode node, std::stack<tnode> nodes_to_traverse) { //todo dlaczego nie stack &?
        return Tree<T>();
    }

    static tnode inorder(tnode node, std::stack<tnode> nodes_to_traverse) {
        return Tree<T>();
    }

    static tnode postorder(tnode node, std::stack<tnode> nodes_to_traverse) {
        return Tree<T>();
    }

    /**
     * Creates empty node.
     * @return empty node
     */
    static tnode createEmptyNode() {
        return Tree<T>();
    }

    /**
     * Creates node with given value.
     * @param value value of new node
     * @return new node
     */
    static tnode createValueNode(T value) {
        return Tree<T>();
    }

    /**
     * Creates node with given value and descendants.
     * @param value value of new node
     * @param left left son
     * @param right right son
     * @return new node
     */
    static tnode createValueNode(T value, tnode left, tnode right) {
        return Tree<T>();
    }

private:

};

#endif //JNP7_TREE_H
