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
    using node_ptr_pair = std::pair<node_smart_ptr, node_smart_ptr>;
    using traversal = std::function<void(std::function<void(T)>, node_smart_ptr)>;
public:
    /**
     * Creates empty tree.
     */
    Tree() : value_(0),
             left_son_(nullptr),
             right_son_(nullptr),
             is_set_(false) {}

    /**
     * Creates new tree with given root.
     * @param root root of new tree.
     */
    Tree(node_smart_ptr root) : value_(root->value_),
                                left_son_(root->left_son_),
                                right_son_(root->right_son_),
                                is_set_(root->is_set_) {}

    /**
     * Creates new tree which is deep copy of given tree.
     * @param other tree to copy
     */
    Tree(const Tree &other) : value_(other.value_), is_set_(other.is_set_) {
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
                         left_son_(other.left_son_),
                         right_son_(other.right_son_),
                         is_set_(other.is_set_) {}

    /**
     * Invokes given operation on tree in post order traversal and returns its result.
     * @param   operation ternary function which is applied to node value,
     *          value of function invoked on left subtree and value of function invoked on right subtree
     * @param   init value of empty node
     * @return  results of function invoked on root
     */
    template<typename F, typename S>
    S fold(F operation, S init) const {
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
    Tree filter(std::function<bool(T)> predicate) const {
        auto lambda = [&predicate](T value, node_smart_ptr left, node_smart_ptr right) -> node_smart_ptr {
            if (predicate(value)) {
                return std::make_shared<Tree>(Tree(value, left, right));
            }

            if (!right->is_set_) {
                return left;
            }
            else if (!left->is_set_) {
                return right;
            }
            else {
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
    template<typename F, typename S>
    S accumulate(F operation, S init, traversal traversal) const {
        S accumulator = init;

        auto lambda = [&accumulator, &operation](T value) {
            accumulator = operation(value, accumulator);
        };

        traversal(lambda, std::make_shared<Tree<T>>(std::move(*this)));

        return accumulator;
    }
    /*
    T accumulate(std::function<T(T, T)> operation,
                 T init,
                 std::function<traversal_ptr(node_smart_ptr)> traversal) const {
        auto tp = std::make_shared<Tree<T>>(std::move(*this));
        auto it = traversal(tp);

        for (auto node = (*it)(); node != nullptr; node = (*it)()) {
            if (node->is_set_) {
                init = operation(node->value_, init);
            }
        }

        return init;
    }
*/
    /**
     * Applies unary function operation to each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param traversal     function which determines traversal
     */
    /*
   void apply(std::function<void(T)> operation, std::function<traversal_ptr(node_smart_ptr)> traversal) const {
       auto tp = std::make_shared<Tree<T>>(std::move(*this));
       auto it = traversal(tp);

       for (auto node = (*it)(); node != nullptr; node = (*it)()) {
           if (node->is_set_) {
               operation(node->value_);
           }
       }
   }
*/
    void apply(std::function<void(T)> operation, traversal traversal) {
        traversal(operation, std::make_shared<Tree<T>>(std::move(*this)));
    }

    /**
     * Gets height of tree which is maximal distance between root and leaf.
     * @return height of tree
     */
    tsize height() const {
        return fold([&](T value, tsize left_h, tsize right_h) -> tsize { return std::max(left_h, right_h) + 1; }, 0);
    }

    /**
     * Gets size of tree which is number of nodes in that tree.
     * @return size of tree
     */
    tsize size() const {
        return fold([&](T value, tsize left_s, tsize right_s) -> tsize { return left_s + right_s + (is_set_ ? 1 : 0); }, 0);
    }

    /**
     * Checks if tree is binary search tree.
     * @return true if tree is binary search tree
     */
    bool is_bst() const {

        auto lambda = [&](T value, bool left_bst, bool right_bst) -> bool {
            if (left_son_->is_set_ && left_son_->value_ > value_) {
                return false;
            }

            if (right_son_->is_set_ && right_son_->value_ < value_) {
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
    /*
   void print(std::function<traversal_ptr(node_smart_ptr)> traversal = inorder) const {
       apply([](T val) { std::cout << val << " "; }, traversal);
       std::cout << std::endl;
   }
   */
    void print() {

    }

    void print(traversal traversal1) {

    }

    //todo
    /*
    static traversal_ptr preorder(node_smart_ptr node) {
        return std::make_shared<PreOrder>(PreOrder(node));
    }

    static traversal_ptr inorder(node_smart_ptr node) {
        return std::make_shared<InOrder>(InOrder(node));
    }

    static traversal_ptr postorder(node_smart_ptr node) {
        return std::make_shared<PostOrder>(PostOrder(node));
    }
*/

    static void inorder(std::function<void(T)> operation, node_smart_ptr node) {
        if (node == nullptr || !node->is_set_) {
            return;
        }

        Tree::inorder(operation, node->left_son_);
        operation(node->value_);
        Tree::inorder(operation, node->right_son_);
    }

    static void postorder(std::function<void(T)> operation, node_smart_ptr node) {
        if (node == nullptr || !node->is_set_) {
            return;
        }

        Tree::postorder(operation, node->left_son_);
        Tree::postorder(operation, node->right_son_);
        operation(node->value_);
    }

    static void preorder(std::function<void(T)> operation, node_smart_ptr node) {
        if (node == nullptr || !node->is_set_) {
            return;
        }

        operation(node->value_);
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
                    is_set_(true) {}

    Tree(T value, node_smart_ptr left, node_smart_ptr right) : value_(value),
                                                               left_son_(left),
                                                               right_son_(right),
                                                               is_set_(true) {}

    template<typename RVal>
    RVal fold_generic(std::function<T(RVal, RVal, T)> operation, RVal init) const {
        if (!is_set_) {
            return init;
        } else {
            return operation(left_son_->fold(operation, init), right_son_->fold(operation, init), value_);
        }
    }


    T value_;
    node_smart_ptr left_son_;
    node_smart_ptr right_son_;
    bool is_set_;
};

#endif //JNP7_TREE_H
