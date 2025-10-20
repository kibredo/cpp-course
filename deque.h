//  Вроде бы всё исправил

template <typename T>
struct Deque {
 private:
    template <bool IterType>
    class base_iterator;
    std::vector<T*> chum_bucket;
    size_t cap;
    size_t d_size;
    size_t start;
    size_t first_bucket;
    static std::vector<T*> allocate(size_t capacity_new);
    void deallocate();
    void relocate();
 public:
  static const size_t kBucketSize = 256;
  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  void erase(iterator it);
  void insert(iterator it, const T& the_T);
  size_t size() const {
    return d_size;
  }
  iterator begin() {
    auto tmp = chum_bucket.begin() + first_bucket;
    return iterator(tmp, start);
  }
  iterator end() {
    auto tmp = chum_bucket.begin() + first_bucket +
    (d_size + start) / kBucketSize;
    return iterator(tmp, (start + d_size) % kBucketSize);
  }
  const_iterator begin() const {
    return cbegin();
  }
  const_iterator end() const {
    auto tmp = chum_bucket.begin() + first_bucket +
    (d_size + start) / kBucketSize;
    return const_iterator(tmp, (start + d_size) % kBucketSize);
  }
  const_iterator cbegin() const {
    auto tmp = chum_bucket.begin() + first_bucket;
    return const_iterator(tmp, start);
  }
  const_iterator cend() const {
    auto tmp = chum_bucket.begin() + first_bucket +
    (d_size + start) / kBucketSize;
    return const_iterator(tmp, (start + d_size) % kBucketSize);
  }
  reverse_iterator rend() {
    return std::make_reverse_iterator(begin());
  }
  reverse_iterator rbegin() {
    return std::make_reverse_iterator(end());
  }
  const_reverse_iterator rbegin() const {
    return std::make_reverse_iterator(cend());
  }
  const_reverse_iterator rend() const {
    return std::make_reverse_iterator(cbegin());
  }
  const_reverse_iterator crbegin() const {
    return std::make_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const {
    return std::make_reverse_iterator(cbegin());
  }

  explicit Deque(size_t sz): cap(0), d_size(0), start(0), first_bucket(0) {
    *this = Deque();
    try {
      for (size_t tmp = 0; tmp < sz; ++tmp) {
        push_back(T());
      }
    } catch(...) {
      deallocate();
      throw;
    }
  }

  ~Deque() {
    deallocate();
  }
  Deque(const Deque& other): cap(other.cap), d_size(other.d_size),
        start(other.start), first_bucket(other.first_bucket) {
    size_t size = static_cast<size_t>(other.chum_bucket.size());
    chum_bucket.resize(size);
    try {
      for (int tmp = 0; tmp < static_cast<int>(chum_bucket.size()); ++tmp) {
        chum_bucket[tmp] = reinterpret_cast<T*>(new char[kBucketSize * sizeof(T)]);
        std::copy(other.chum_bucket[tmp], other.chum_bucket[tmp] + kBucketSize, chum_bucket[tmp]);
      }
    } catch(...) {
      deallocate();
      throw;
    }
  }

  Deque& operator=(const Deque& other) {
    if (this != &other) {
      deallocate();
      chum_bucket = allocate(4);
      d_size = 0;
      cap = 4;
      start = 0;
      first_bucket = 0;
      try {
        for (auto it = other.begin(); it != other.end(); ++it) {
          push_back(*it);
        }
      } catch(...) {
        deallocate();
        throw;
      }
    }
    return *this;
  }

  Deque(): cap(0), d_size(0), start(0) {
    chum_bucket = allocate(4);
    cap = chum_bucket.size();
    first_bucket = 0;
  }

  void push_back(const T& the_T) {
    size_t end_ind = (start + d_size) % kBucketSize;
    size_t end_bucket = (first_bucket + (start + d_size) / kBucketSize);
    if (start + d_size + first_bucket * kBucketSize == cap * kBucketSize) {
      relocate();
      end_bucket = (first_bucket + d_size / kBucketSize);
      end_ind = (start + d_size) % kBucketSize;
    }
    new (chum_bucket[end_bucket] + end_ind) T(the_T);
    ++d_size;
  }

  Deque(size_t sz, const T& the_T): cap(0), d_size(0), start(0) {
    chum_bucket = allocate(4);
    cap = chum_bucket.size();
    first_bucket = 0;
    for (size_t tmp = 0; tmp < sz; ++tmp) {
      push_back(the_T);
    }
  }
  void push_front(const T& the_T) {
    if (start != 0) {
      ++d_size;
      --start;
      new (chum_bucket[first_bucket] + start) T(the_T);
      return;
    }
    if (first_bucket == 0) {
      relocate();
    }
    --first_bucket;
    start = kBucketSize - 1;
    ++d_size;
    new (chum_bucket[first_bucket] + start) T(the_T);
  }
  void pop_back() {
    if (d_size == 0) {
      throw std::out_of_range("Nope");
    }
    (chum_bucket[first_bucket + d_size / kBucketSize] + (start + d_size) % kBucketSize)->~T();
    --d_size;
  }
  void pop_front() {
    if (d_size == 0) {
      throw std::out_of_range("Nope");
      return;
    }
    if (start == kBucketSize - 1) {
      ++first_bucket;
    }
    (chum_bucket[first_bucket] + start)->~T();
    ++start;
    start %= kBucketSize;
    --d_size;
  }

  T& operator[](int pos) {
    return *(chum_bucket[first_bucket + pos / kBucketSize] + (start + pos) % kBucketSize);
  }
  const T& operator[](int pos) const {
    return *(chum_bucket[first_bucket + pos / kBucketSize] + (start + pos) % kBucketSize);
  }
  T& at(size_t pos) {
    if (pos >= d_size) {
      throw std::out_of_range("Nope");
    }
    return (*this)[pos];
  }
  const T& at(size_t pos) const {
    if (pos >= d_size) {
      throw std::out_of_range("Nope");
    }
    return (*this)[pos];
  }
};

template <typename T>
void Deque<T>::relocate() {
  std::vector<T*> new_bucket = chum_bucket;
  chum_bucket.resize(0);
  chum_bucket = allocate(cap * 3);
  std::copy(new_bucket.begin(), new_bucket.end(), chum_bucket.begin() + cap);
  first_bucket = cap + first_bucket;
  cap = 3 * cap;
}

template <typename T>
std::vector<T*> Deque<T>::allocate(size_t new_cap) {
  std::vector<T*> new_bucket;
  try {
    for (size_t tmp = 0; tmp < new_cap; ++tmp) {
      new_bucket.push_back(reinterpret_cast<T*>(new char[Deque::kBucketSize * sizeof(T)]));
    }
  }
  catch (...) {
    for (auto& bucket : new_bucket) {
      delete[] reinterpret_cast<char*>(bucket);
    }
    throw;
  }
  return new_bucket;
}

template <typename T>
void Deque<T>::deallocate() {
  T* bucket;
  for (size_t tmp = start + Deque::kBucketSize * first_bucket; tmp < Deque::kBucketSize * first_bucket + d_size + start; ++tmp) {
    if (tmp % Deque::kBucketSize == 0 || tmp == start + Deque::kBucketSize * first_bucket) {
      bucket = chum_bucket[tmp / Deque::kBucketSize];
    }
    (bucket + tmp % Deque::kBucketSize)->~T();
  }
  for (auto& buck : chum_bucket) {
    delete[] reinterpret_cast<char*>(buck);
  }
  d_size = 0;
  cap = 0;
  start = 0;
  first_bucket = 0;
  chum_bucket.resize(0);
}

template<typename T>
template <bool IterType>
class Deque<T>::base_iterator {
 public:
  using value_type = std::conditional_t<IterType, const T, T>;
  using difference_type = int;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::random_access_iterator_tag;
  using the_iter = base_iterator<IterType>;
  using ptr_t = std::conditional_t<IterType, typename std::vector<T*>::const_iterator,
                                   typename std::vector<T*>::iterator>;

  explicit base_iterator(const ptr_t& bucket, size_t start);
  base_iterator();
  pointer operator->() {
    return *the_bucket + pos;
  }
  int operator-(const base_iterator& other) const {
    return (the_bucket - other.the_bucket) * kBucketSize + pos - other.pos;
  }

  value_type& operator*() {
    return *(*the_bucket + pos);
  }
  operator const_iterator() const {
    return const_iterator(the_bucket, pos);
  }

  the_iter& operator+=(int delta) {
    if (delta < 0) {
      return *this -= -delta;
    }
    the_bucket += (delta + pos) / kBucketSize;
    pos = (delta + pos) % kBucketSize;
    return *this;
  }

  the_iter& operator-=(const int delta) {
    if (delta < 0) {
      return *this += -delta;
    }
    int tmp = (pos - delta) / kBucketSize;
    the_bucket += tmp;
    pos = ((pos - delta) % kBucketSize + kBucketSize) % kBucketSize;
    return *this;
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
  the_iter operator+(int a) const {
    auto ans = *this;
    ans += a;
    return ans;
  }
  the_iter operator-(int a) const {
    auto ans = *this;
    ans -= a;
    return ans;
  }

  bool operator==(const base_iterator& other) const = default;
  bool operator!=(const base_iterator& other) const {
    return !(*this == other);
  }
  bool operator<(const base_iterator& other) {
    return (*this - other) < 0;
  }
  bool operator>(const base_iterator& other) {
    return !(*this < other || *this == other);
  }
  bool operator>=(const base_iterator& other) {
    return !(*this < other);
  }
  bool operator<=(const base_iterator& other) {
    return !(*this > other);
  }

 private:
  ptr_t the_bucket;
  size_t pos;
  static const ptrdiff_t kBucketSize = Deque<T>::kBucketSize;
};

template <typename T>
void Deque<T>::erase(iterator it) {
  if (it == begin()) {
    pop_front();
    return;
  }
  if (it == end()) {
    pop_back();
    return;
  }
  (it)->~T();
  ++it;
  while (it != end()) {
    *(it - 1) = *it;
    ++it;
  }
  pop_back();
}

template <typename T>
void Deque<T>::insert(iterator it, const T& the_T) {
  auto the_temp = the_T;
  while (it != end()) {
    std::swap(*it, the_temp);
    ++it;
  }
  push_back(the_temp);
}

template <typename T>
template <bool IterType>
Deque<T>::base_iterator<IterType>::base_iterator():
                                   the_bucket(nullptr), pos(0) {}
template <typename T>
template <bool IterType>
Deque<T>::base_iterator<IterType>::base_iterator(const ptr_t& bucket, size_t start):
                                   the_bucket(bucket), pos(start) {}
