#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>

#include "IEnumerator.h"

// Шаблонный класс односвязного списка
template <class T> class LinkedList {
private:
    // Внутренняя структура узла списка
    struct Node {
        T data;
        Node *next;
        
        Node(const T &value) {
            this->data = value;
            this->next = nullptr;
        }
    };

    Node *head; // Указатель на начало списка
    Node *tail; // Указатель на конец списка
    int size;   // Текущий размер списка

public:
    // Конструктор по умолчанию: создает пустой список
    LinkedList() : LinkedList(nullptr, 0) {}

    // Конструктор: создает список из массива элементов
    LinkedList(T *items, int count) {
        this->head = nullptr;
        this->tail = nullptr;
        this->size = 0;
        
        for (int index = 0; index < count; index++) {
            Node *new_node = new Node(items[index]);
            
            if (head == nullptr) {
                head = new_node;
                tail = new_node;
            } else {
                tail->next = new_node;
                tail = new_node;
            }
            size++;
        }
    }

    // Конструктор копирования
    LinkedList(const LinkedList<T> &list) : LinkedList() {
        Node *current = list.head;
        
        while (current != nullptr) {
            Node *new_node = new Node(current->data);
            
            if (head == nullptr) {
                head = new_node;
                tail = new_node;
            } else {
                tail->next = new_node;
                tail = new_node;
            }
            
            current = current->next;
        }
        
        this->size = list.size;
    }

    // Возвращает первый элемент списка
    const T &GetFirst() const {
        if (head == nullptr) {
            throw std::out_of_range("Первого элемента не существует");
        }
        
        return head->data;
    }

    // Возвращает последний элемент списка
    const T &GetLast() const {
        if (tail == nullptr) {
            throw std::out_of_range("Последнего элемента не существует");
        }
        
        return tail->data;
    }

    // Возвращает элемент по индексу
    const T &Get(int index) const {
        if (index >= size || index < 0) {
            throw std::out_of_range("Элемента с таким индексом не существует");
        }
            
        Node *current = head;

        for (int ind = 0; ind < index; ind++) {
            current = current->next;
        }
        
        return current->data;
    }

    // Возвращает подсписок по заданным индексам
    LinkedList<T> *GetSubList(int start_index, int end_index) {
        if (start_index < 0 || end_index < 0 || end_index >= size ||
            start_index >= size || end_index < start_index) {
            throw std::out_of_range("Индексы невалидны для данного списка");
        }

        LinkedList<T> *sub_list = new LinkedList<T>();
        Node *current = this->head;

        for (int index = 0; index < start_index; index++) {
            current = current->next;
        }

        Node **current_ptr = &(sub_list->head);

        for (int index = start_index; index <= end_index; index++) {
            Node *new_node = new Node(current->data);

            *current_ptr = new_node;
            current_ptr = &(new_node->next);

            sub_list->tail = new_node;
            sub_list->size++;

            current = current->next;
        }

        return sub_list;
    }

    // Возвращает текущую длину списка
    int GetLength() const { 
        return size; 
    }

    // Добавляет элемент в конец списка
    void Append(const T &item) {
        Node *new_node = new Node(item);
        
        if (head == nullptr) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
        
        size++;
    }

    // Добавляет элемент в начало списка
    void Prepend(const T &item) {
        Node *new_node = new Node(item);
        
        if (this->head == nullptr) {
            this->head = new_node;
            this->tail = new_node;
        } else {
            new_node->next = this->head;
            this->head = new_node;
        }
        
        size++;
    }

    // Вставляет элемент по заданному индексу
    void InsertAt(const T &item, int index) {
        if (index >= size || index < 0) {
            throw std::out_of_range("Индекс вне списка");
        }
            
        if (index == 0) {
            Prepend(item);
            return;
        }
        
        Node *new_node = new Node(item);
        Node *current = head;
        
        for (int ind = 0; ind < (index - 1); ind++) {
            current = current->next;
        }
        
        new_node->next = current->next;
        current->next = new_node;
        size++;
    }

    // Возвращает новый список, являющийся конкатенацией текущего и переданного
    LinkedList<T> *Concat(LinkedList<T> *list) {
        LinkedList<T> *sub_list = new LinkedList<T>(*this);
        Node *current = list->head;

        for (int index = 0; index < list->size; index++) {
            sub_list->Append(current->data);
            current = current->next;
        }
        
        return sub_list;
    }

    // Деструктор: освобождает память всех узлов
    ~LinkedList() {
        Node *current = this->head;
        
        while (current != nullptr) {
            Node *local_current = current->next;
            delete current;
            current = local_current;
        }
    }

    // Итератор для связного списка
    class LinkedListEnumerator : public IEnumerator<T> {
    private:
        Node *current;
        
    public:
        // Конструктор итератора
        LinkedListEnumerator(Node *head) { 
            current = head; 
        }
        
        // Возвращает текущий элемент
        const T &GetCurrent() const override { 
            return current->data; 
        }
        
        // Переходит к следующему элементу
        void MoveNext() override {
            if (current != nullptr) {
                current = current->next;
            }
        }
        
        // Проверяет, есть ли следующий элемент
        bool HasNext() const override { 
            return current != nullptr; 
        }
    };

    // Возвращает итератор для обхода списка
    IEnumerator<T> *GetEnumerator() const { 
        return new LinkedListEnumerator(this->head); 
    }
};

#endif // LINKEDLIST_H