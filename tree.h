#ifndef JNP7_TREE_H
#define JNP7_TREE_H

#include <iostream>
#include <memory>
#include <stack>

template<typename T>
class Tree;

template<typename T>
class Traversal {
public:
    virtual std::shared_ptr<Tree<T>> get_next() {
        std::cout << "base class" << std::endl;
        return nullptr;
    }

protected:
    std::stack<std::pair<std::shared_ptr<Tree<T>>, bool>> nodes_;
};


template<typename T>
class Tree {
    using tsize = T;
    using node_ptr = Tree<T> *;
    using node_smart_ptr = std::shared_ptr<Tree<T>>;
    using traversal_ptr = std::shared_ptr<Traversal<T>>;
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
     * @param other
     */
    Tree(const Tree &other) : value_(other.value_), is_set_(other.is_set_) {
        if (is_set_) {
            auto l = Tree<T>(other.left_son_);
            auto r = Tree<T>(other.right_son_);
            left_son_ = std::make_shared<Tree<T>>(l);
            right_son_ = std::make_shared<Tree<T>>(r);
        }
    }

    Tree(Tree &&other) : value_(other.value_),
                         left_son_(other.left_son_),
                         right_son_(other.right_son_),
                         is_set_(other.is_set_) {}

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
                 std::function<traversal_ptr(node_smart_ptr)> traversal) { //todo traversal type?
        auto tp = std::make_shared<Tree<T>>(std::move(*this)); //todo
        auto it = traversal(tp);

        for (auto node = it->get_next(); node != nullptr; node = it->get_next()) {
            if (node->is_set_) {
                init = operation(node->value_, init);
            }
        }

        return init;
    }

    /**
     * Applies unary function operation to each node. Traversal of tree is given as a function.
     * @param operation     function which will be applied
     * @param traversal     function which determines traversal
     */
    void apply(std::function<void(T)> operation, std::function<traversal_ptr(node_smart_ptr)> traversal) {
        auto tp = std::make_shared<Tree<T>>(std::move(*this)); //todo
        auto it = traversal(tp);

        for (auto node = it->get_next(); node != nullptr; node = it->get_next()) {
            if (node->is_set_) {
                operation(node->value_);
            }
        }
    }

    /**
     * Gets height of tree which is maximal distance between root and leaf.
     * @return height of tree
     */
    T height() {
        return fold([&](T l_height, T r_height, T val) -> T { return std::max(l_height, r_height) + 1; }, 0);
        //todo czy moze tak zostac? zamiast tsize? bo fold w koncu jest na T
    }

    /**
     * Gets size of tree which is number of nodes in that tree.
     * @return size of tree
     */
    T size() {
        return fold([&](T l_size, T r_size, T val) -> T { return l_size + r_size + (is_set_ ? 1 : 0); }, 0);
    }

    /**
     * Checks if tree is binary search tree.
     * @return true if tree is binary search tree
     */
    bool is_bst() {
        return true;
    }

    /**
     * Prints value in each node of tree in given order. By default goes in order through tree.
     * @param traversal function which determines traversal
     */
    void print(std::function<traversal_ptr(node_smart_ptr)> traversal = inorder) {
        apply([](T val) { std::cout << val << " "; }, traversal);
        std::cout << std::endl;
    }


    //todo
    static traversal_ptr preorder(node_smart_ptr node) {
        return std::make_shared<PreOrder>(PreOrder(node));
    }

    static traversal_ptr inorder(node_smart_ptr node) {
        return std::make_shared<InOrder>(InOrder(node));
    }

    static traversal_ptr postorder(node_smart_ptr node) {
        return std::make_shared<PostOrder>(PostOrder(node));
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
    class PreOrder : public Traversal<T> {
    public:
        PreOrder(node_smart_ptr root) {
            if (root != nullptr) {
                if (root->right_son_ != nullptr) this->nodes_.push({root->right_son_, false});
                if (root->left_son_ != nullptr) this->nodes_.push({root->left_son_, false});
                this->nodes_.push({root, true});
            }
        }

        PreOrder(PreOrder &&other) {
            this->nodes_ = other.nodes_;
        }

        node_smart_ptr get_next() override {
            if (this->nodes_.empty()) {
                return nullptr;
            }

            auto top = this->nodes_.top();
            this->nodes_.pop();
            node_smart_ptr node = top.first;

            if (!top.second) {
                if (node->right_son_ != nullptr) this->nodes_.push({node->right_son_, false});
                if (node->left_son_ != nullptr) this->nodes_.push({node->left_son_, false});
                this->nodes_.push({node, true});
                top = this->nodes_.top();
                this->nodes_.pop();
            }
            return top.first;
        }
    };

    class InOrder : public Traversal<T> {
    public:
        InOrder(node_smart_ptr root) {
            if (root != nullptr) {
                if (root->right_son_ != nullptr) this->nodes_.push({root->right_son_, false});
                this->nodes_.push({root, true});
                if (root->left_son_ != nullptr) this->nodes_.push({root->left_son_, false});
            }
        }

        InOrder(InOrder &&other) {
            this->nodes_ = other.nodes_;
        }

        node_smart_ptr get_next() override {
            if (this->nodes_.empty()) {
                return nullptr;
            }

            auto top = this->nodes_.top();
            this->nodes_.pop();
            node_smart_ptr node = top.first;

            if (!top.second) {
                if (node->right_son_ != nullptr) this->nodes_.push({node->right_son_, false});
                this->nodes_.push({node, true});
                if (node->left_son_ != nullptr) this->nodes_.push({node->left_son_, false});
                top = this->nodes_.top();
                this->nodes_.pop();
            }

            return top.first;
        }
    };

    class PostOrder : public Traversal<T> {
    public:
        PostOrder(node_smart_ptr root) {
            if (root != nullptr) {
                this->nodes_.push({root, true});
                if (root->right_son_ != nullptr) this->nodes_.push({root->right_son_, false});
                if (root->left_son_ != nullptr) this->nodes_.push({root->left_son_, false});
            }
        }

        PostOrder(PostOrder &&other) {
            this->nodes_ = other.nodes_;
        }

        node_smart_ptr get_next() override {
            if (this->nodes_.empty()) {
                return nullptr;
            }

            auto top = this->nodes_.top();
            this->nodes_.pop();
            node_smart_ptr node = top.first;

            if (!top.second) {
                this->nodes_.push({node, true});
                if (node->right_son_ != nullptr) this->nodes_.push({node->right_son_, false});
                if (node->left_son_ != nullptr) this->nodes_.push({node->left_son_, false});
                top = this->nodes_.top();
                this->nodes_.pop();
            }

            return top.first;
        }
    };

    Tree(T value) : value_(value),
                    left_son_(createEmptyNode()),
                    right_son_(createEmptyNode()),
                    is_set_(true) {}

    Tree(T value, node_smart_ptr left, node_smart_ptr right) : value_(value),
                                                               left_son_(left),
                                                               right_son_(right),
                                                               is_set_(true) {}

    T value_;
    node_smart_ptr left_son_;
    node_smart_ptr right_son_;
    bool is_set_;
};

#endif //JNP7_TREE_H
