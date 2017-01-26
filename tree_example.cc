#include <iostream>
#include <functional>
#include <sstream>
#include <string>

#include "tree.h"

void testMap() {

    auto one = Tree<int>::createValueNode(1);
    auto two = Tree<int>::createValueNode(2, one, Tree<int>::createEmptyNode());
    auto three = Tree<int>::createValueNode(3, two, Tree<int>::createEmptyNode());
    Tree<int> tree(three);

    double x = 3;

    auto treeDouble = tree.lazy_map([&](int val) -> double { return static_cast<double>(val) / x; });
    auto t2 = tree.lazy_map([&](int val) ->double {return static_cast<double>(val) / x;});
    treeDouble.print();
    x = 5;
    t2.print();


}

int main(int argc, const char *argv[]) {
    auto two = Tree<int>::createValueNode(2);
    Tree<int> t1(two);
    auto xx = Tree<int>::createValueNode(50, Tree<int>::createEmptyNode(), Tree<int>::createEmptyNode());
    Tree<int> t2(xx);

    t1.print(Tree<int>::inorder);
    t2.print(Tree<int>::inorder);


    std::cout << "size: " << t1.size() << std::endl;
    std::cout << "height: " << t1.height() << std::endl;
    std::cout << "bst: " << t1.is_bst() << std::endl;


    std::cout << "size: " << t2.size() << std::endl;
    std::cout << "height: " << t2.height() << std::endl;
    std::cout << "bst: " << t2.is_bst() << std::endl;

    auto one = Tree<int>::createValueNode(1, Tree<int>::createEmptyNode(), two);
    auto four = Tree<int>::createValueNode(4);
    auto five = Tree<int>::createValueNode(5,
                                           Tree<int>::createValueNode(4),
                                           Tree<int>::createEmptyNode());
    auto root = Tree<int>::createValueNode(3, one, five);
    Tree<int> tree(root);

    tree.print(Tree<int>::inorder);
    tree.print(Tree<int>::preorder);
    tree.print(Tree<int>::postorder);

    std::cout << "size: " << tree.size() << std::endl;
    std::cout << "height: " << tree.height() << std::endl;
    std::cout << "bst: " << tree.is_bst() << std::endl;

    std::cout << "custom print: ";
    tree.apply([](int e) { std::cout << e << ":"; }, Tree<int>::inorder);
    std::cout << std::endl;

    int x = 2;
    Tree<int> minus_x = tree.map([&](int e) -> int { return e - x; });
    Tree<int> lazy_minus_x = tree.lazy_map([&](int e) -> int { return e - x; });
    x = 4;
    std::cout << "-2: ";
    minus_x.print();
    std::cout << "-4: ";
    lazy_minus_x.print();

    Tree<int> odd = minus_x.filter([](int e) -> bool { return (e & 1) == 1; });
    std::cout << "odd: ";
    odd.print();

    int sum1 = tree.accumulate(std::plus<int>(), 0, Tree<int>::inorder);
    std::cout << "sum: " << sum1 << std::endl;

    int sum2 = tree.fold([](int val, int l, int r) -> int { return val + l + r; }, 0);
    std::cout << "sum: " << sum2 << std::endl;

    testMap();

    return 42;
}