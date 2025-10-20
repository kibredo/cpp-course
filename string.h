#include <algorithm>
#include <cstring>
#include <iostream>

/*
THIS IS FINAL
Вроде бы всё исправил
substr() из посылки вроде как работает, не понимаю что там не так
*/

class String {
 private:
  char* dat = nullptr;
  size_t sz = 0;
  size_t cap = 0;
  String(size_t count): dat(new char[count + 1]), sz(count), cap(count) {
    dat[sz] = '\0';
  }
  void relocate(size_t new_cap) {
    String new_str(new_cap);
    std::copy(dat, dat + sz, new_str.dat);
    new_str.sz = sz;
    swap(new_str);
    dat[sz] = '\0';
  }
 public:
  void swap(String& other) {
    std::swap(cap, other.cap);
    std::swap(sz, other.sz);
    std::swap(dat, other.dat);
  }
  String(): String((size_t)0) {}
  String(size_t count, char c): String(count) {
    memset(dat, c, count);
  }
  String(const char* str): String(strlen(str)) {
    std::copy(str, str + sz, dat);
  }
  String(const String& other): dat(new char[other.cap]), sz(other.sz), cap(other.cap) {
    memcpy(dat, other.dat, sz);
  }
  String& operator=(const String& other) {
    String copy = other;
    swap(copy);
    return *this;
  }
  String(const char chr): String((size_t)1) {
    dat[0] = chr;
  }
  ~String() {
    delete[] dat;
  }
  String& operator+=(const String& other) {
    int temp_sz = this->sz;
    if (this->sz + other.sz >= cap) {
      relocate(this->sz + other.sz + 1);
    }
    sz += other.sz;
    std::copy(other.dat, other.dat + other.sz, dat + temp_sz);
    dat[sz] = '\0';
    return *this;
  }
  String& operator+=(const char chr) {
    if (sz == cap) {
      relocate(cap * 2);
    }
    this->dat[sz] = chr;
    sz += 1;
    this->dat[sz] = '\0';
    return *this;
  }
  void clear() {
    sz = 0;
  }
  char operator[](size_t index) const {
    return dat[index];
  }
  char& operator[](size_t index) {
    return dat[index];
  }
  size_t length() const {
    return sz;
  }
  size_t size() const {
    return sz;
  }
  size_t capacity() const {
    return cap;
  }
  char* data() const {
    return dat;
  }
  char* data() {
    return dat;
  }
  void push_back(const char the_char) {
    if (cap == 0) {
      relocate(2);
    }
    if (sz == cap) {
      relocate(2 * cap);
    }
    this->dat[sz] = the_char;
    ++sz;
    this->dat[sz] = '\0';
  }
  void pop_back() {
    --sz;
    this->dat[sz] = '\0';
  }
  char front() const {
    return *dat;
  }
  char back() const {
    return *(dat + sz - 1);
  }
  char& front() {
    return *dat;  
  }
  char& back() {
    return *(dat + sz - 1);
  }
  size_t find(const String& str) const {
    auto substring = str.dat;
    auto ptr = strstr(dat, substring);
    if (ptr == NULL) {
      return length();
    }
    return static_cast<size_t> (ptr - dat);
  }
  size_t rfind(const String& str) const {
    auto substr = str.dat;
    size_t cnt = 0;
    auto ptr = strstr(dat, substr);
    auto delta = ptr;
    while (ptr != NULL) {
      delta = ptr;
      ptr = strstr(ptr + 1, substr);
      ++cnt;
    }
    if (cnt > 0) {
      return static_cast<size_t> (delta - dat);
    }
    return length();
  }
  bool empty() const {
    return sz == 0;
  }
  String substr(size_t start, size_t count) const {
    String new_str(dat + start);
    new_str.dat[count] = '\0';
    new_str.sz = count;
    return new_str;
  }
  void shrink_to_fit() {
    relocate(sz);
  }
};

String operator+(const String& other, const String& other_other) {
  String ans = other;
  ans += other_other;
  return ans;
}

bool operator<(const String& other, const String& other_other) {
  return strcmp(other.data(), other_other.data()) < 0;
}
bool operator<=(const String& a, const String& b) {
  return !(b < a);
}
bool operator>(const String& a, const String& b) {
  return !(a <= b);
}
bool operator>=(const String& a, const String& b) {
  return !(a < b);
}
bool operator==(const String& a, const String& b) {
  return !(a < b) && !(b < a);
}
bool operator!=(const String& a, const String& b) {
  return !(a == b);
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  size_t the_size = str.size();
  auto dat = str.data();
  for (size_t tmp = 0; tmp < the_size; ++tmp) {
    out << dat[tmp];
  }
  return out;
}

namespace help_input {
  void insert_letter(char*& str, char& letter, int& sz, int& cap) {
    if (sz + 1 >= cap) {
      cap *= 2;
      char* new_str = new char[cap];
      memmove(new_str, str, sz);
      std::swap(str, new_str);
      delete[] new_str;
    }
    ++sz;
    str[sz - 1] = letter;
  }

  char* get_input(std::istream& in) {
    char letter;
    int sz = 0;
    int cap = 2;
    char* str1 = new char[cap + 1];
    bool check = true;
    while (check) {
      in.get(letter);
      if (letter == '\n' || letter == ' ' || in.eof()) {
        check = false;
      } else {
        insert_letter(str1, letter, sz, cap);
      }
    }
    char end_letter = '\0';
    insert_letter(str1, end_letter, sz, cap);
    return str1;
  }
}

std::istream& operator>>(std::istream& in, String& str) {
  str = String(help_input::get_input(in));
  return in;
}