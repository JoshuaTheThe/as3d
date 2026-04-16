#ifndef CORE_LL_H
#define CORE_LL_H

#include <iostream>
#include <assert.h>

template <typename Value, typename Key>
class ll
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

        node *append(const Value &value, Key &key)
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

        void remove(node *k)
        {
                if (k->prev)
                        k->prev->next = k->next;
                else
                        first = k->next;
                if (k->next)
                        k->next->prev = k->prev;
                else
                        last = k->prev;
                if (last_accessed == k)
                        last_accessed = k->prev ? k->prev : k->next;
                delete k;
                size--;
        }

        void remove(Key &key)
        {
                node *a = this->searchfrom(this->last_accessed, key, false);
                if (a)
                {
                        remove(a);
                        return;
                }

                node *b = this->searchfrom(this->last_accessed, key, true);
                if (b)
                {
                        remove(b);
                        return;
                }
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

        Value &operator[](Key key)
        {
                node *a = this->searchfrom(this->last_accessed, key, false);
                if (a)
                {
                        return a->value;
                }

                node *b = this->searchfrom(this->first, key, false);
                if (b)
                {
                        return b->value;
                }

                std::string msg = "ll: key not found" + std::to_string(key);
                throw std::out_of_range(msg);
        }

        void operator^(Key &key)
        {
                remove(key);
        }

        void operator()(const Value &value, Key &key)
        {
                append(value, key);
        }

        size_t length(void) const
        {
                return this->size;
        }

        const node *begin(void) {return this->first;}
        const node *end(void) {return this->end;}
};

#endif
