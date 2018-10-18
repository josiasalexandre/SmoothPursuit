#ifndef NODE_H
#define NODE_H

template<typename T>
class Node
{
    public:

        // the prev node
        Node *prev;

        // the next node
        Node *next;

        // the current value
        T element;

        // the basic constructor
        Node(T e) : prev(nullptr), next(nullptr), element(e) {}

        // the copy constructor
        Node(const Node &n) : prev(nullptr), next(nullptr), element(n.element) {}

        // the basic destructor
        ~Node() {

            if (nullptr != prev) {
                delete prev;
            }

            if (nullptr != next){
                delete next;
            }

        }
};

#endif
