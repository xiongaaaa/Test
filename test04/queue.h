#ifndef GTEST_SAMPLES_SAMPLE3_INL_H_
#define GTEST_SAMPLES_SAMPLE3_INL_H_

#include <stddef.h>

template <typename E>  // E is the element type
class Queue;

template <typename E>  // E is the element type
class QueueNode {
 friend class Queue<E>;

public:
 // Gets the element in this node.
 const E& element() const { return element_; }

 // Gets the next node in the queue.
 QueueNode* next() { return next_; }
 const QueueNode* next() const { return next_; }

private:
 // Creates a node with a given element value.  The next pointer is
 // set to NULL.
 explicit QueueNode(const E& an_element)
     : element_(an_element), next_(nullptr) {}

 // We disable the default assignment operator and copy c'tor.
 const QueueNode& operator = (const QueueNode&);
 QueueNode(const QueueNode&);

 E element_;
 QueueNode* next_;
};

template <typename E>  // E is the element type.
class Queue {
public:
 // Creates an empty queue.
 Queue() : head_(nullptr), last_(nullptr), size_(0) {}

 // D'tor.  Clears the queue.
 ~Queue() { Clear(); }

 // Clears the queue.
 void Clear() {
   if (size_ > 0) {
     // 1. Deletes every node.
     QueueNode<E>* node = head_;
     QueueNode<E>* next = node->next();
     for (; ;) {
       delete node;
       node = next;
       if (node == nullptr) break;
       next = node->next();
     }

     // 2. Resets the member variables.
     head_ = last_ = nullptr;
     size_ = 0;
   }
 }

 size_t Size() const { return size_; }

 QueueNode<E>* Head() { return head_; }
 const QueueNode<E>* Head() const { return head_; }

 QueueNode<E>* Last() { return last_; }
 const QueueNode<E>* Last() const { return last_; }

 void Enqueue(const E& element) {
   QueueNode<E>* new_node = new QueueNode<E>(element);

   if (size_ == 0) {
     head_ = last_ = new_node;
     size_ = 1;
   } else {
     last_->next_ = new_node;
     last_ = new_node;
     size_++;
   }
 }

 E* Dequeue() {
   if (size_ == 0) {
     return nullptr;
   }

   const QueueNode<E>* const old_head = head_;
   head_ = head_->next_;
   size_--;
   if (size_ == 0) {
     last_ = nullptr;
   }

   E* element = new E(old_head->element());
   delete old_head;

   return element;
 }

 template <typename F>
 Queue* Map(F function) const {
   Queue* new_queue = new Queue();
   for (const QueueNode<E>* node = head_; node != nullptr;
        node = node->next_) {
     new_queue->Enqueue(function(node->element()));
   }

   return new_queue;
 }

private:
 QueueNode<E>* head_;  // The first node of the queue.
 QueueNode<E>* last_;  // The last node of the queue.
 size_t size_;  // The number of elements in the queue.

 // We disallow copying a queue.
 Queue(const Queue&);
 const Queue& operator = (const Queue&);
};

#endif  // GTEST_SAMPLES_SAMPLE3_INL_H_

