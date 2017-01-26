#ifndef JNP7_TREE_H
#define JNP7_TREE_H

#include <iostream>
#include <memory>
#include <stack>

template<typename T>
class Tree;

/*
template<typename T>
class   Traversal {
public:
    virtual std::shared_ptr<Tree<T>> operator()() {
        return nullptr;
    }

protected:
    std::stack<std::pair<std::shared_ptr<Tree<T>>, bool>> nodes_;
};
*/

template<typename T>
class Tree {
    using tsize = int;
    using node_ptr = Tree<T> *;
    using node_smart_ptr = std::shared_ptr<Tree<T>>;
//    using traversal_ptr = std::shared_ptr<Traversal<T>>;
//    using node_ptr_pair = std::pair<node_smart_ptr, node_smart_ptr>;
    using traversal = std::function<void(std::function<void(T)>, node_smart_ptr)>;
public:
    /**
     * Creates empty tree.
     */
    Tree() : value_(0),
             left_son_(nullptr),
             right_son_(nullptr),
             is_set_(false),
             is_initialized_(true) {}

    /**
     * Creates new tree with given root.
     * @param root root of new tree.
     */
    Tree(node_smart_ptr root) : value_(root->value_),
                                lazy_function_(root->lazy_function_),
                                left_son_(root->left_son_),
                                right_son_(root->right_son_),
                                is_set_(root->is_set_),
                                is_initialized_(root->is_initialized_) {}

    /**
     * Creates new tree which is deep copy of given tree.
     * @param other tree to copy
     */
    Tree(Tree const &other) : value_(other.value_),
                              lazy_function_(other.lazy_function_),
                              is_set_(other.is_set_),
                              is_initialized_(other.is_initialized_) {
        if (is_set_) {
            auto l = Tree<T>(other.left_son_);
            auto r = Tree<T>(other.right_son_);
            left_son_ = std::make_shared<Tree<T>>(l);
            right_son_ = std::make_shared<Tree<T>>(r);
        }
    }

    /**
     * Creates new tree which points to same descendants as root of copied tree.
     * @param other tree to copy
     */
    Tree(Tree &&other) : value_(other.value_),
                         lazy_function_(other.lazy_function_),
                         left_son_(other.left_son_),
                         right_son_(other.right_son_),
                         is_set_(other.is_set_),
                         is_initialized_(other.is_initialized_) {}

    /**
     * Invokes given operation on tree in post order traversal and returns its result.
     * @param   operation ternary function which is applied to node value,
     *          value of function invoked on left subtree and value of function invoked on right subtree
     * @param   init value of empty node
     * @return  results of function invoked on root
     */
    template<typename F, typename S>
    S fold(F operation, S init) {
        if (!is_set_) {
            return init;
        } else {
            return operation(getValue(), left_son_->fold(operation, init), right_son_->fold(operation, init));
        }
    }

    /**
     * Creates new tree with elements that satisfy given predicate. If node didn't satisfy predicate had two sons,
     * then right subtree will be attached to most right node in left subtree.
     * @param   predicate function taking value of node and returning boolean
     * @return  new tree
     */
    Tree filter(std::function<bool(T)> predicate) {
        auto lambda = [&predicate](T value, node_smart_ptr left, node_smart_ptr right) -> node_smart_ptr {
            if (predicate(value)) {
                return std::make_shared<Tree>(Tree(value, left, right));
            }

            if (!right->is_set_) {
                return left;
            } else if (!left->is_set_) {
                return right;
            } else {
                auto leftParent = left;
                while (leftParent->right_son_->is_set_) {
                    leftParent = leftParent->right_son_;
                }

                leftParent->right_son_ = right;

                return left;
            }
        };

        return Tree(fold(lambda, createEmptyNode()));
    }

    /**
     * Creates new tree with elements that were transformed by given function.
     * @param transformer   transforming function
     * @return              new tree
     */
    template<typename F, typename U = typename std::result_of<F(T)>::type>
    Tree<U> map(F transformer) {
        auto lambda = ([&](T t, node_smart_ptr l, node_smart_ptr r) -> node_smart_ptr {
            return Tree<U>::createValueNode(transformer(t), l, r);
        });
        return Tree<U>(fold(lambda, Tree<U>::createEmptyNode()));
    }

    /**
     * Creates new tree with elements that were transformed by given function. Transforming function
     * is invoked only once and when node value is accessed.
     * @param transformer   transforming function
     * @return              new tree
     */
    template<typename F, typename U = typename std::result_of<F(T)>::type>
    Tree<T> lazy_map(F transformer) {
        auto lambda = ([&](T t, node_smart_ptr l, node_smart_ptr r) -> node_smart_ptr {
            return Tree<U>::createValueNode(
                    [t, transformer]() -> U {
                        return transformer(t);
                    }, l, r);
        });

        return Tree<U>(fold(lambda, Tree<U>::createEmptyNode()));
    }

    /**
     * Applies binary operation to init and value in each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param init          initial value of accumulator
     * @param traversal     function which determines traversal
     * @return              value of accumulator after traversing tree
     */
    template<typename F, typename S>
    S accumulate(F operation, S init, traversal traversal) const {
        S accumulator = init;

        auto lambda = [&accumulator, &operation](T value) {
            accumulator = operation(value, accumulator);
        };

        traversal(lambda, std::make_shared<Tree<T>>(std::move(*this)));

        return accumulator;
    }

    /**
     * Applies unary function operation to each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param traversal     function which determines traversal
     */
    void apply(std::function<void(T)> operation, traversal traversal) const {
        traversal(operation, std::make_shared<Tree<T>>(std::move(*this)));
    }

    /**
     * Gets height of tree which is maximal distance between root and leaf.
     * @return height of tree
     */
    tsize height() {
        return fold([&](T value, tsize left_h, tsize right_h) -> tsize { return std::max(left_h, right_h) + 1; }, 0);
    }

    /**
     * Gets size of tree which is number of nodes in that tree.
     * @return size of tree
     */
    tsize size() {
        return fold([&](T value, tsize left_s, tsize right_s) -> tsize { return left_s + right_s + (is_set_ ? 1 : 0); },
                    0);
    }

    /**
     * Checks if tree is binary search tree.
     * @return true if tree is binary search tree
     */
    bool is_bst() {

        auto lambda = [&](T value, bool left_bst, bool right_bst) -> bool {
            if (left_son_->is_set_ && left_son_->getValue() > getValue()) {
                return false;
            }

            if (right_son_->is_set_ && right_son_->getValue() < getValue()) {
                return false;
            }

            return left_bst && right_bst;
        };

        return fold(lambda, true);
    }

    /**
     * Prints value in each node of tree in given order. By default goes in order through tree.
     * @param traversal function which determines traversal
     */

    void print(traversal traversal = inorder) const {
        apply([](T val) { std::cout << val << " "; }, traversal);
        std::cout << std::endl;
    }

    static void inorder(std::function<void(T)> operation, node_smart_ptr node) {
        if (node == nullptr || !node->is_set_) {
            return;
        }

        Tree::inorder(operation, node->left_son_);
        operation(node->getValue());
        Tree::inorder(operation, node->right_son_);
    }

    static void postorder(std::function<void(T)> operation, node_smart_ptr node) {
        if (node == nullptr || !node->is_set_) {
            return;
        }

        Tree::postorder(operation, node->left_son_);
        Tree::postorder(operation, node->right_son_);
        operation(node->getValue());
    }

    static void preorder(std::function<void(T)> operation, node_smart_ptr node) {
        if (node == nullptr || !node->is_set_) {
            return;
        }

        operation(node->getValue());
        Tree::preorder(operation, node->left_son_);
        Tree::preorder(operation, node->right_son_);
    }

    /**
     * Creates empty node.
     * @return empty node
     */
    static node_smart_ptr createEmptyNode() {
        return std::make_shared<Tree<T>>(Tree<T>());
    }

    /**
     * Creates node with given value.
     * @param value value of new node
     * @return new node
     */
    static node_smart_ptr createValueNode(T value) {
        return std::make_shared<Tree<T>>(Tree<T>(value));
    }

    /**
     * Creates node with given value and descendants.
     * @param value value of new node
     * @param left left son
     * @param right right son
     * @return new node
     */
    static node_smart_ptr createValueNode(T value, node_smart_ptr left, node_smart_ptr right) {
        return std::make_shared<Tree<T>>(Tree<T>(value, left, right));
    }

private:
    Tree(T value) : value_(value),
                    left_son_(createEmptyNode()),
                    right_son_(createEmptyNode()),
                    is_set_(true),
                    is_initialized_(true) {}

    Tree(T value, node_smart_ptr left, node_smart_ptr right) : value_(value),
                                                               left_son_(left),
                                                               right_son_(right),
                                                               is_set_(true),
                                                               is_initialized_(true) {}

    Tree(std::function<T()> lazy_function) : lazy_function_(lazy_function),
                                             left_son_(createEmptyNode()),
                                             right_son_(createEmptyNode()),
                                             is_set_(true),
                                             is_initialized_(false) {}

    Tree(std::function<T()> lazy_function, node_smart_ptr left, node_smart_ptr right) : lazy_function_(lazy_function),
                                                                                        left_son_(left),
                                                                                        right_son_(right),
                                                                                        is_set_(true),
                                                                                        is_initialized_(false) {}

    static node_smart_ptr createValueNode(std::function<T()> fun) {
        return std::make_shared<Tree<T>>(Tree<T>(fun));
    }

    static node_smart_ptr createValueNode(std::function<T()> fun, node_smart_ptr left, node_smart_ptr right) {
        return std::make_shared<Tree<T>>(Tree<T>(fun, left, right));
    }

    T getValue() {
        if (!is_initialized_) {
            is_initialized_ = true;
            value_ = lazy_function_();
        }

        return value_;
    }

    T value_;
    std::function<T()> lazy_function_;
    node_smart_ptr left_son_;
    node_smart_ptr right_son_;
    bool is_set_;
    bool is_initialized_;
};

#endif //JNP7_TREE_H
