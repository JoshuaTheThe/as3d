#ifndef CORE_LL_H
#define CORE_LL_H

#include <iostream>
#include <assert.h>

template <typename Value, typename Key, Value init> class ll
{
private:
        struct node
        {
                Value value;
                Key key;
                node *next, *prev;
        };
        
        node *first, *last, *last_accessed;
        size_t size;

        node *searchfrom(node *start, Key key, bool direction)
        {
                node *current = start;
                while (current)
                {
                        if (current->key == key)
                        {
                                this->last_accessed = current;
                                return current;
                        }
                        if (direction)
                                current = current->prev;
                        else
                                current = current->next;
                }

                return nullptr;
        }

        node *append(Value value, Key &key)
        {
                node *n = new node;
                n->value = value;
                n->next = nullptr;
                n->prev = nullptr;
                n->key = key;

                if (!first)
                {
                        first = n;
                        last = n;
                }
                else
                {
                        last->next = n;
                        n->prev = last;
                        last = n;
                }

                size++;
                return n;
        }

public:
        ll(void)
        {
                last_accessed = last = first = nullptr;
                size = 0;
        }

        ~ll(void)
        {
                node *a = first, *b = nullptr;
                while (a)
                {
                        if (b)
                        {
                                delete b;
                        }
                        b = a;
                        a = a->next;
                }

                delete b;
                size = 0;
                last_accessed = last = first = nullptr;
        }

        Value &operator[](Key idx)
        {
                node *a = this->searchfrom(this->last_accessed, idx, false);
                if (a)
                {
                        return a->value;
                }

                node *b = this->searchfrom(this->last_accessed, idx, true);
                if (b)
                {
                        return b->value;
                }

                return append(init, idx)->value;
        }

        size_t length(void) const
        {
                return this->size;
        }
};

#endif
