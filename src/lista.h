#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T>
struct Node 
{
    T data;
    std::unique_ptr<Node<T>> next;
    Node<T>* prev;

    Node(T val) : data(val), next(nullptr), prev(nullptr) {}
};

template <typename T>
class LinkedList 
{
private:
    std::unique_ptr<Node<T>> head;
    Node<T>* tail;
    size_t size;

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), size(0) 
    {
        Node<T>* current = other.head.get();
        while (current) 
        {
            pushBack(current->data);
            current = current->next.get();
        }
    }

    LinkedList(LinkedList&& other) : head(std::move(other.head)), tail(other.tail), size(other.size) 
    {
        other.tail = nullptr;
        other.size = 0;
    }

    ~LinkedList() = default;

    void pushFront(T data) 
    {
        auto newNode = std::make_unique<Node<T>>(data);
        newNode->next = std::move(head);
        if (newNode->next) 
        {
            newNode->next->prev = newNode.get();
        } 
        else 
        {
            tail = newNode.get();
        }
        head = std::move(newNode);
        size++;
    }

    void pushBack(T data) {
        auto newNode = std::make_unique<Node<T>>(data);
        if (!tail) 
        {
            head = std::move(newNode);
            tail = head.get();
        } 
        else 
        {
            newNode->prev = tail;
            tail->next = std::move(newNode);
            tail = tail->next.get();
        }
        size++;
    }

    T& operator[](size_t index) 
    {
        if (index >= size) 
        {
            throw std::out_of_range("Index out of range");
        }

        Node<T>* current = head.get();
        for (size_t i = 0; i < index; ++i) 
        {
            current = current->next.get();
        }
        return current->data;
    }

    class Iterator 
    {
    private:
        Node<T>* current;

    public:
        Iterator(Node<T>* start) : current(start) {}

        T& operator*() { return current->data; }
        Iterator& operator++() 
        {
            if (current) current = current->next.get();
            return *this;
        }
        Iterator operator++(int) 
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        Iterator& operator--() 
        {
            if (current) current = current->prev;
            return *this;
        }
        Iterator operator--(int) 
        {
            Iterator temp = *this;
            --(*this);
            return temp;
        }
        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    Iterator begin() const { return Iterator(head.get()); }
    Iterator end() const { return Iterator(nullptr); }
};