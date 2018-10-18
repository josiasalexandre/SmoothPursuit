#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <new>

#include "NodeT.hpp"

// AN ORDDINARY DOUBLE LINKED LIST
// IT WAS DEVELOPED IN MY ALGORITHMS AND DATA TYPES CLASS
// IT'S USED IN THIS PROJECT JUST FOR FUN

template<typename T>
class List
{
    private:

        // the first, last and the curren node
        Node<T> *first, *last, *current;

        // the list size
        unsigned int size;

        // the iteration flag
        bool iter;

        // iterator pointer
        Node<T> *iterNode;

        // private methods
        // append a node to the end
        void append(Node<T> *n)
        {
            // verify if it's not an empty list
            if (0 < size)
            {
                // push the node to the last position
                last->next = n;
                n->prev = last;
                last = n;
            }
            else
            {
                // set the pointers to the first (and also the last) element
                first = last = current = n;
            }

            // update the list size
            size += 1;
        }

        // prepend a node to the beginning
        void prepend(Node<T> *n)
        {
            // verify if it's not an empty list
            if (0 < size)
            {
                // push the node to the first position
                n->next = first;
                first->prev = n;
                first = n;
            }
            else
            {
                // set the pointers to the first (and also the last) element
                first = last = current = n;
            }
            // update the list size
            size += 1;
        }

        // find a node
        Node<T>* find_node(T e)
        {
            Node<T> *aux = first;

            // iterate over the entire list
            while(aux != nullptr)
            {
                if (aux->element == e) { return aux; }
                // update the aux
                aux = aux->next;
            }

            // not found
            return nullptr;
        }

        // remove a node
        void remove_node(Node<T> *n)
        {
            // update the current node pointer
            if (n == current) { current = current->next; }

            // verify it it's not a single element list'
            if (1 < size)
            {
                if (n != first && n != last)
                {
                    // update the prev and next nodes pointers
                    n->prev->next = n->next;
                    n->next->prev = n->prev;
                }
                else if (n == first)
                {
                    // update the pointers
                    first = first->next;
                    first->prev = nullptr;
                }
                else if (n == last)
                {
                    // update the pointers
                    last = last->prev;
                    last->next = nullptr;
                }
            }
            else
            {
                // just 1 element
                first = last = nullptr;
            }

            // delete the node
            delete n;

            // update the list size
            size -= 1;
        }

        // efficient first node removal
        void remove_first_node()
        {
            if(nullptr != first)
            {
                // update
                Node<T> *tmp = first;

                first = first->next;

                if(nullptr != first)
                {
                    first->prev = nullptr;
                }

                if (tmp == current)
                {
                    current = current->next;
                }

                tmp->next = nullptr;
                tmp->prev = nullptr;

                delete tmp;

                size -= 1;
            }
        }

        // efficient last node removal
        void remove_last_node()
        {
            if(nullptr != last)
            {
                // update
                Node<T> *tmp = last;

                last = last->prev;

                if(nullptr != last) { last->next = nullptr; }

                if (tmp == current) { current = current->next; }

                tmp->prev = nullptr;
                tmp->next = nullptr;

                delete tmp;

                size -= 1;
            }
        }

        // discards a node, without deleting it
        void discard_node(Node<T> *n)
        {
            // update the current node pointer
            if (n == current) { current = current->next; }

            // verify it it's not a single element list'
            if (1 < size)
            {
                if (n != first && n != last)
                {
                    // update the prev and next nodes pointers
                    n->prev->next = n->next;
                    n->next->prev = n->prev;
                }
                else if (n == first)
                {
                    // update the pointers
                    first = first->next;
                    first->prev = nullptr;
                }
                else if (n == last)
                {
                    // update the pointers
                    last = last->prev;
                    last->next = nullptr;
                }
            }
            else
            {
                // just 1 element
                first = last = nullptr;
            }

            // update the list size
            size -= 1;
        }

    public:

        // the basic constructor
        List() : first(nullptr), last(nullptr), current(nullptr), size(0), iter(false), iterNode(nullptr)
        {

        // the basic destructor
        ~List()
        {
            Node<T> *aux;

            iterNode = nullptr;

            // reset the current pointer to the first one
            current = first;

            // iterate over the entire list and delete each node
            while(nullptr != current)
            {
                // get the next node
                aux = current->next;

                // updates the nodes pointers
                current->next = nullptr;
                current->prev = nullptr;

                // delete the current node
                delete current;

                // update the current node
                current = aux;
            }

            // set the last and the first to nullptr
            last = first = nullptr;
        }

        // returns the list size
        unsigned int get_size() { return size; }

        // is empty?
        bool empty() { return (0 == size); }

        // insert a new element at the end
        void push_back(T e)
        { 
            // build a new node
            Node<T> *n = new Node<T>(e);

            if(nullptr == n) { throw std::bad_alloc(); }

            // insert the new node at the end
            append(n);

            // set n to nullptr
            n = nullptr;
        }

        // insert a new element at the beginning
        void push_front(T e)
        {
            // we can't insert an invalid element
            // build a new node
            Node<T> *n = new Node<T>(e);
            if (nullptr == n)
                {                throw std::bad_alloc();
            }

            // insert the new node at the beginning
            prepend(n);

            // set n to nullptr
            n = nullptr;
        }

        // remove the element if its inside the list
        bool erase(T e)
        {
            // find the element inside the list
            Node<T> *aux = find_node(e);

            if (nullptr != aux)
            {
                // remove the node from the current list
                remove_node(aux);

                // element found and deleted
                return true;
            }

            // the element is not inside the list
            return false;
        }

        // discards an element
        bool discard(T e)
        {
            // find the element inside the list
            Node<T> *aux = find_node(e);

            if (nullptr != aux)
            {
                // remove the node from the current list
                discard_node(aux);

                // element found and removed
                return true;
            }

            return false;
        }

        /* DANGER */
        // get first element
        T front() { return first->element; }

        /* DANGER */
        // get last element
        T back() { return last->element; }

        /* DANGER */
        // get the current element
        T current_element() { return current->element; }

        // just a simple iteration trick
        bool iterator()
        {
            if (0 < size)
            {
                if (!iter)
                {
                    iter = true;
                    iterNode = first;
                }

                if (nullptr != iterNode)
                {
                    current = iterNode;
                    iterNode = iterNode->next;
                    return true;
                }
                else
                {
                    iter = false;
                    current = first;
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        /* DANGER */
        // pop out the first element
        T pop_front()
        {
            T e = first->element;

            // remove the first node
            remove_first_node();

            return e;
        }

        /* DANGER */
        // pop out the last element
        T pop_back()
        {
            T e = last->element;

            // remove the first node
            remove_last_node();

            return e;
        }

};

#endif
