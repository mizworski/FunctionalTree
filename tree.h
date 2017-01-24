#ifndef JNP7_TREE_H
#define JNP7_TREE_H


#include <memory>
#include <stack>

template<typename T>
class Tree {
    using tsize = size_t;
    using tree_ptr = std::shared_ptr<Tree<T>>;
    using tnode = std::shared_ptr<Tree<T>>;//Tree<T>;
public:
    /**
     * Creates empty tree.
     */
    Tree() : left_son_(nullptr), right_son_(nullptr), value_(0), is_set_(false) {}

    /**
     * Creates new tree with given root.
     * @param root root of new tree.
     */
    Tree(tree_ptr root) : left_son_(root->left_son_),
                          right_son_(root->right_son_),
                          value_(root->value_),
                          is_set_(root->is_set_) {
//        std::cout << root->left_son_ << std::endl;
//        std::cout << root->right_son_ << std::endl;
//        std::cout << root->value_ << std::endl;
    }

    /**
     * Creates new tree which is deep copy of given tree.
     * @param other
     */
    Tree(const Tree &other) {}
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
        if (!is_set_) {
            return init;
        } else {
            return operation(value_, left_son_->fold(operation, init), right_son_->fold(operation, init));
        }
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
        return std::make_shared<Tree<T>>(Tree<T>());
    }

    static tnode inorder(tnode node, std::stack<tnode> nodes_to_traverse) {
        return std::make_shared<Tree<T>>(Tree<T>());
    }

    static tnode postorder(tnode node, std::stack<tnode> nodes_to_traverse) {
        return std::make_shared<Tree<T>>(Tree<T>());
    }

    /**
     * Creates empty node.
     * @return empty node
     */
    static tnode createEmptyNode() {
        return std::make_shared<Tree<T>>(Tree<T>());
    }

    /**
     * Creates node with given value.
     * @param value value of new node
     * @return new node
     */
    static tnode createValueNode(T value) {
        Tree<T> t = Tree<T>(value);
        tnode pt = std::make_shared<Tree<T>>(t);
        Tree<T> wt = *pt;
        std::cout << pt->value_ << std::endl;
        return pt;

    }

    /**
     * Creates node with given value and descendants.
     * @param value value of new node
     * @param left left son
     * @param right right son
     * @return new node
     */
    static tnode createValueNode(T value, tnode left, tnode right) {
        auto t = Tree<T>(value, left, right);
        return std::make_shared<Tree<T>>(t);
    }

    Tree(T value) : left_son_(nullptr), right_son_(nullptr), value_(value), is_set_(true) {}
    Tree(T value, tnode left, tnode right) : left_son_(left), right_son_(right), value_(value), is_set_(true) {}
private:

    tnode left_son_;
    tnode right_son_;
    T value_;
    bool is_set_;
};

#endif //JNP7_TREE_H
