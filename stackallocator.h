#include <iterator>
#include <compare>
#include <memory>

template <size_t N>
struct StackStorage {
  char storage[N];
  StackStorage() = default;
  ~StackStorage() = default;
  StackStorage(const StackStorage& other) = delete;
  StackStorage operator=(const StackStorage<N>& other) = delete;
  size_t start = 0;
  char* GetReference(size_t pos) {
    return &storage[pos];
  }
};

template <typename T, size_t N>
struct StackAllocator {
  StackStorage<N>* storage = nullptr;
  size_t space = 0;
  using value_type = T;
  using pointer = value_type*;
  using reference = T&;
  StackAllocator() {
    if (N < this->storage->start) {
      throw std::bad_alloc();
    }
    this->space = N - this->storage->start;
  }
  StackAllocator(StackStorage<N>& other): storage(&other) {}
  template <typename U>
  StackAllocator(const StackAllocator<U, N>& other)
      : storage(other.storage), space(other.space) {}
  pointer allocate(size_t the_sz) {
    if (the_sz > N) {
      throw std::bad_alloc();
    }
    size_t sz = N - (storage->GetReference(storage->start) - storage->GetReference(0));
    char* ptr = storage->GetReference(storage->start);
    std::align(alignof(T), sizeof(T), reinterpret_cast<void*&>(ptr), sz);
    storage->start += ptr - storage->GetReference(storage->start);
    T* final_pos = reinterpret_cast<T*>(storage->GetReference(storage->start));
    storage->start += the_sz * sizeof(T);
    return final_pos;
  }
  void deallocate(T*, size_t) {}
  bool operator==(const StackAllocator& other) const {
    return storage == other.storage;
  }
  template <typename U>
  StackAllocator<T, N>& operator=(const StackAllocator<U, N>& other) {
    this->storage = other.storage;
    return *this;
  }
  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };
};

template <typename T, typename the_alloc = std::allocator<T>>
struct List {
 private:
  struct BaseNode;
  struct Node;
  using allocator_type = typename std::allocator_traits<the_alloc>::template rebind_alloc<Node>;
  using allocator_traits = typename std::allocator_traits<the_alloc>::template rebind_traits<Node>;
  void AddVertex(Node* pos);
  void AddVertexVal(Node* pos, const T& val);
  void DeleteVertex(Node* pos);
  void GetCleared();
  void GetSwapped(List&);
  size_t the_size = 0;
  BaseNode fakeNode = BaseNode();
  allocator_type alloc;
 public:
  List();
  List(size_t size);
  List(const List<T, the_alloc>& other);
  List(size_t size, const T& val);
  List(const the_alloc& allocator);
  List(size_t size, const the_alloc& allocator);
  List(size_t size, const T& val, const the_alloc& allocator);
  ~List();
  List<T, the_alloc>& operator=(const List<T, the_alloc>& list);
  allocator_type get_allocator() const {
    return this->alloc;
  }
  void push_back(const T& val);
  void push_front(const T& val);
  void pop_back();
  void pop_front();
  size_t size() const {
    return this->the_size;
  }
  template <bool IterType>
  class base_iterator;
  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  void erase(const_iterator it);
  void insert(const_iterator it, const T& value);
  iterator begin() {
    return iterator(fakeNode.next, 0);
  }
  iterator end() {
    return iterator(fakeNode.prev->next, this->the_size);
  }
  const_iterator cbegin() const {
    return const_iterator(fakeNode.next, 0);
  }
  const_iterator cend() const {
    return const_iterator(fakeNode.prev->next, this->the_size);
  }
  const_iterator begin() const {
    return const_iterator(fakeNode.next, 0);
  }
  const_iterator end() const {
    return cend();
  }
  reverse_iterator rend() {
    return std::make_reverse_iterator(this->begin());
  }
  reverse_iterator rbegin() {
    return std::make_reverse_iterator(this->end());
  }
  const_reverse_iterator rbegin() const {
    return std::make_reverse_iterator(this->cend());
  }
  const_reverse_iterator rend() const {
    return std::make_reverse_iterator(this->cbegin());
  }
  const_reverse_iterator crbegin() const {
    return std::make_reverse_iterator(this->cend());
  }
  const_reverse_iterator crend() const {
    return std::make_reverse_iterator(this->cbegin());
  }
};

template <typename T, typename the_alloc>
void List<T, the_alloc>::GetSwapped(List& other) {
  std::swap(alloc, other.alloc);
  std::swap(the_size, other.the_size);
  std::swap(fakeNode, other.fakeNode);
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::GetCleared() {
  while (the_size != 0) {
    pop_back();
  }
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::DeleteVertex(List<T, the_alloc>::Node* pos) {
  pos->next->prev = pos->prev;
  pos->prev->next = pos->next;
  --this->the_size;
  allocator_traits::destroy(alloc, &pos->value);
  allocator_traits::deallocate(alloc, pos, 1);
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::AddVertex(List::Node* pos) {
  Node* ptr = nullptr;
  try {
    ptr = alloc.allocate((size_t)1);
    allocator_traits::construct(alloc, &ptr->value);
  } catch (...) {
    throw;
  }
  pos->prev->next = ptr;
  ptr->next = pos;
  ptr->prev = pos->prev;
  pos->prev = ptr;
  ++this->the_size;
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::AddVertexVal(List::Node* pos, const T& val) {
  Node* ptr = nullptr;
  try {
    ptr = alloc.allocate((size_t)1);
    allocator_traits::construct(alloc, &ptr->value, val);
  } catch (...) {
    throw;
  }
  pos->prev->next = ptr;
  ptr->next = pos;
  ptr->prev = pos->prev;
  pos->prev = ptr;
  ++this->the_size;
}

template <typename T, typename the_alloc>
struct List<T, the_alloc>::BaseNode {
  Node* prev = nullptr;
  Node* next = nullptr;
  BaseNode() {
    this->prev = static_cast<Node*>(this);
    this->next = static_cast<Node*>(this);
  }
};

template <typename T, typename the_alloc>
struct List<T, the_alloc>::Node: BaseNode {
  T value;
  Node() = default;
};

template <typename T, typename the_alloc>
List<T, the_alloc>::List() = default;

template <typename T, typename the_alloc>
List<T, the_alloc>::List(size_t sz) {
  for (size_t tmp = 0; tmp < sz; ++tmp) {
    try {
      this->AddVertex(fakeNode.prev);
    } catch (...) {
      GetCleared();
      throw;
    }
  }
}

template <typename T, typename the_alloc>
List<T, the_alloc>::List(size_t sz, const the_alloc& allocator): alloc(allocator) {
  for (size_t tmp = 0; tmp < sz; ++tmp) {
    try {
      this->AddVertex(fakeNode.prev);
    } catch (...) {
      GetCleared();
      throw;
    }
  }
}

template <typename T, typename the_alloc>
List<T, the_alloc>::List(size_t sz, const T& val) {
  for (size_t tmp = 0; tmp < sz; ++tmp) {
    try {
      push_back(val);
    } catch (...) {
      GetCleared();
      throw;
    }
  }
}

template <typename T, typename the_alloc>
List<T, the_alloc>::List(const the_alloc& allocator): alloc(allocator) {}

template <typename T, typename the_alloc>
List<T, the_alloc>::List(const List& other):
List(allocator_traits::select_on_container_copy_construction(other.get_allocator())) {
  for (const auto& value : other) {
    try {
      push_back(value);
    } catch (...) {
      GetCleared();
      throw;
    }
  }
}

template <typename T, typename the_alloc>
List<T, the_alloc>::List(size_t size, const T& val, const the_alloc& allocator): alloc(allocator) {
  for (size_t tmp = 0; tmp < size; ++tmp) {
    try {
      push_back(val);
    } catch (...) {
      GetCleared();
      throw;
    }
  }
}

template <typename T, typename the_alloc>
List<T, the_alloc>::~List() {
  while(this->the_size != 0) {
    this->pop_back();
  }
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::push_back(const T& val) {
  try {
    this->AddVertexVal(static_cast<Node*>(&fakeNode), val);
  } catch (...) {
    throw;
  }
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::push_front(const T& val) {
  try {
    this->AddVertexVal(fakeNode.next, val);
  } catch (...) {
    throw;
  }
}

template <typename T, typename the_alloc>
List<T, the_alloc>& List<T, the_alloc>::operator=(const List<T, the_alloc>& other) {
  if (this == &other) {
    return *this;
  }
  size_t temp_sz = the_size;
  if (allocator_traits::propagate_on_container_copy_assignment::value) {
    this->alloc = other.alloc;
  }
  size_t pushed = 0;
  for (const auto& elem : other) {
    try {
      this->push_back(elem);
      ++pushed;
    } catch (...) {
      for (size_t tmp = 0; tmp < pushed; ++tmp) {
        pop_back();
      }
      throw;
    }
  }
  for (size_t tmp = 0; tmp < temp_sz; ++tmp) {
    pop_front();
  }
  return *this;
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::pop_back() {
  this->DeleteVertex(fakeNode.prev);
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::pop_front() {
  this->erase(cbegin());
}

template <typename T, typename the_alloc>
template <bool IterType>
class List<T, the_alloc>::base_iterator {
 public:
  using difference_type = int;
  using value_type = std::conditional_t<IterType, const T, T>;
  using pointer = value_type*;
  using reference = value_type&;
  using the_iter = base_iterator<IterType>;
  using iterator_category = std::bidirectional_iterator_tag;
  base_iterator(Node* ptr, difference_type pos): ptr(ptr), position(pos) {}
  Node* operator->() {
    return this->ptr;
  }
  reference operator*() {
    return this->ptr->value;
  }
  difference_type operator-(const base_iterator& other) const {
    return (this->position - other.position);
  }
  operator const_iterator() const {
    return const_iterator(this->ptr, this->position);
  }
  the_iter& operator+=(int delta) {
    if (!delta) {
      return *this;
    }
    if (delta > 0) {
      while (delta) {
        this->ptr = this->ptr->next;
        ++this->position;
        --delta;
      }
      return *this;
    }
    while (delta) {
      this->ptr = this->ptr->prev;
      --this->position;
      ++delta;
    }
    return *this;
  }
  the_iter& operator-=(int delta) {
    return *this += (-1) * delta;
  }
  the_iter& operator++() {
    return *this += 1;
  }
  the_iter operator++(int) {
    the_iter ans = *this;
    *this += 1;
    return ans;
  }
  the_iter& operator--() {
    return *this -= 1;
  }
  the_iter operator--(int) {
    the_iter ans = *this;
    *this -= 1;
    return ans;
  }
  bool operator==(const base_iterator& other) const {
    return this->ptr == other.ptr;
  }
  bool operator!=(const base_iterator& other) const {
    return !(*this == other);
  }
  bool operator<(const base_iterator& other) {
    return (*this - other) < 0;
  }
  bool operator>(const base_iterator& other) {
    return (*this - other) > 0;
  }
  bool operator>=(const base_iterator& other) {
    return !(*this < other);
  }
  bool operator<=(const base_iterator& other) {
    return !(*this > other);
  }
  Node* ptr;
  difference_type position;
};

template <typename T, typename the_alloc>
void List<T, the_alloc>::erase(const_iterator it) {
  this->DeleteVertex(it.ptr);
}

template <typename T, typename the_alloc>
void List<T, the_alloc>::insert(const_iterator iter, const T& val) {
  this->AddVertexVal(iter.operator->(), val);
}