#ifndef JNP7_TREE_H
#define JNP7_TREE_H

#include <memory>
#include <stack>

template<typename T>
class Tree;

template<typename T>
class Traversal {
public:
    virtual std::shared_ptr<Tree<T>> operator()() {}
protected:
    std::stack<std::shared_ptr<Tree<T>>> nodes_;
};


template<typename T>
class Tree {
    using tsize = T;
    using tree_ptr = std::shared_ptr<Tree<T>>;
    using tnode = std::shared_ptr<Tree<T>>;
    using travelsal_order = std::shared_ptr<Traversal<T>>;
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
                          is_set_(root->is_set_) {}

    /**
     * Creates new tree which is deep copy of given tree.
     * @param other
     */
    Tree(const Tree &other) {}

    Tree(Tree &&other) : Tree(other.value_, other.left_son_, other.right_son_) {}

    // todo const T& instead of T?
    /**
     * Invokes given operation on tree in post order traversal and returns its result.
     * @param   operation ternary function which is applied to node value,
     *          value of function invoked on left subtree and value of function invoked on right subtree
     * @param   init value of empty node
     * @return  results of function invoked on root
     */
    T fold(std::function<T(T, T, T)> operation, T init) {
        if (this == nullptr || !is_set_) { //todo this == nullptr
            return init;
        } else {
            return operation(left_son_->fold(operation, init), right_son_->fold(operation, init), value_);
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
                 std::function<travelsal_order(tnode)> traversal) { //todo traversal type?
        return init;
    }

    /**
     * Applies unary function operation to each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param traversal     function which determines traversal
     */
    void apply(std::function<void(T)> operation, std::function<travelsal_order(tnode)> traversal) {

    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "IncompatibleTypes"
    /**
     * Gets height of tree which is maximal distance between root and leaf.
     * @return height of tree
     */
    T height() {
        return fold([&] (T val, T l_height, T r_height) -> T { return std::max(l_height, r_height) + 1; }, 0);
        //todo czy moze tak zostac? zamiast tsize? bo fold w koncu jest na T
    }
#pragma clang diagnostic pop

    /**
     * Gets size of tree which is number of nodes in that tree.
     * @return size of tree
     */
    tsize size() {
        return fold([&] (T val, T l_size, T r_size) -> T { return l_size + r_size + (is_set() ? 1 : 0); }, 0);
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
    void print(travelsal_order traversal) {

    }

    class PreOrder : public Traversal<T> {
    public:
        PreOrder(tnode root) {
            if (root != nullptr) {
                if (root->right_son_ != nullptr) this->nodes_.push(root->right_son_);
                if (root->left_son_ != nullptr) this->nodes_.push(root->left_son_);
                this->nodes_.push(root);
            }
        }

        PreOrder(PreOrder &&other) {
            this->nodes_ = other.nodes_;
        }

        tnode operator()() override {
            tnode node = this->nodes_.top();
            this->nodes_.pop();
            if (node->right_son_!= nullptr) this->nodes_.push(node->right_son_);
            if (node->left_son_ != nullptr) this->nodes_.push(node->left_son_);
            this->nodes_.push(node);

            node = this->nodes_.top();
            this->nodes_.pop();
            return node;
        }
    };

    class InOrder : public Traversal<T> {
    public:
        InOrder(tnode root) {
            if (root != nullptr) {
                if (root->right_son_ != nullptr) this->nodes_.push(root->right_son_);
                this->nodes_.push(root);
                if (root->left_son_ != nullptr) this->nodes_.push(root->left_son_);
            }
        }

        InOrder(InOrder &&other) {
            this->nodes_ = other.nodes_;
        }

        tnode operator()() override {
            tnode node = this->nodes_.top();
            this->nodes_.pop();
            if (node->right_son_!= nullptr) this->nodes_.push(node->right_son_);
            this->nodes_.push(node);
            if (node->left_son_ != nullptr) this->nodes_.push(node->left_son_);

            node = this->nodes_.top();
            this->nodes_.pop();
            return node;
        }
    };

    class PostOrder : public Traversal<T> {
    public:
        PostOrder(tnode root) {
            if (root != nullptr) {
                this->nodes_.push(root);
                if (root->right_son_ != nullptr) this->nodes_.push(root->right_son_);
                if (root->left_son_ != nullptr) this->nodes_.push(root->left_son_);
            }
        }

        PostOrder(PostOrder &&other) {
            this->nodes_ = other.nodes_;
        }

        tnode operator()() override {
            tnode node = this->nodes_.top();
            this->nodes_.pop();
            this->nodes_.push(node);
            if (node->right_son_!= nullptr) this->nodes_.push(node->right_son_);
            if (node->left_son_ != nullptr) this->nodes_.push(node->left_son_);

            node = this->nodes_.top();
            this->nodes_.pop();
            return node;
        }
    };

    //todo
    static travelsal_order preorder(tnode node) {
        return std::make_shared<Traversal<T>>(PreOrder(node));
    }

    static travelsal_order inorder(tnode node) {
        return std::make_shared<Traversal<T>>(InOrder(node));
    }

    static travelsal_order postorder(tnode node) {
        return std::make_shared<Traversal<T>>(PostOrder(node));
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
        return std::make_shared<Tree<T>>(Tree<T>(value));
    }

    /**
     * Creates node with given value and descendants.
     * @param value value of new node
     * @param left left son
     * @param right right son
     * @return new node
     */
    static tnode createValueNode(T value, tnode left, tnode right) {
        return std::make_shared<Tree<T>>(Tree<T>(value, left, right));
    }

    bool is_set() const {
        return is_set_;
    }
private:
    Tree(T value) : left_son_(nullptr), right_son_(nullptr), value_(value), is_set_(true) {}

    Tree(T value, tnode left, tnode right) : left_son_(left), right_son_(right), value_(value), is_set_(true) {}

    tnode left_son_;
    tnode right_son_;
    T value_;
    bool is_set_;
};

#endif //JNP7_TREE_H
