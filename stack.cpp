#include <cstring>
#include <iostream>

void ClearStack(char**& stack, int size_of_stack, int capacity) {
  if (size_of_stack > 0) {
    capacity = size_of_stack;
    for (int tmp = 0; tmp < capacity; ++tmp) {
      delete[] stack[tmp];
    }
  }
  delete[] stack;
}

void CopyChars(char**& stack, char**& new_stack, int& size) {
  for (int tmp = 0; tmp < size; ++tmp) {
    int length = strlen(stack[tmp]);
    new_stack[tmp] = new char[length + 1];
    memcpy(new_stack[tmp], stack[tmp], length);
    new_stack[tmp][length] = '\0';
  }
}

void PushElement(char**& stack, char*& str, int& capacity, int& size) {
  std::cout << "ok" << '\n';
  if (size == capacity) {
    capacity *= 2;
    char** new_stack = new char*[capacity + 1];
    memcpy(new_stack, stack, size);
    CopyChars(stack, new_stack, size);
    std::swap(stack, new_stack);
    ClearStack(new_stack, size, size);
  }
  *(stack + size) = str;
  ++size;
}

void PrintCharArray(char*& arr) {
  for (int tmp = 0; tmp < (int)strlen(arr); ++tmp) {
    std::cout << *(arr + tmp);
  }
  std::cout << '\n';
}

void PopStack(char**& stack, int& capacity, int& size) {
  if (size == 0) {
    std::cout << "error" << '\n';
    return;
  }
  if (4 * size <= capacity) {
    capacity /= 2;
    char** new_stack = new char*[capacity];
    memcpy(new_stack, stack, size * sizeof(char*));
    CopyChars(stack, new_stack, size);
    std::swap(stack, new_stack);
    ClearStack(new_stack, size, size);
  }
  std::cout << stack[size - 1];
  std::cout << '\n';
  delete[] stack[size - 1];
  --size;
}

void GetBack(char**& stack, int& size_of_stack) {
  if (size_of_stack == 0) {
    std::cout << "error" << '\n';
  } else {
    PrintCharArray(*(stack + size_of_stack - 1));
  }
}

void GetSize(int size_of_stack) { std::cout << size_of_stack << '\n'; }

void InsertLetter(char*& str, char& letter, int& size, int& capacity) {
  if (size == capacity) {
    capacity *= 2;
    char* new_str = new char[capacity];
    memmove(new_str, str, size);
    std::swap(str, new_str);
    delete[] new_str;
  }
  ++size;
  str[size - 1] = letter;
}

char* GetInput() {
  char letter;
  int size = 0;
  int capacity = 2;
  char* str1 = new char[capacity + 1];
  std::cin.get(letter);
  bool check = true;
  while (check) {
    std::cin.get(letter);
    if (letter == '\n') {
      check = false;
    } else {
      InsertLetter(str1, letter, size, capacity);
    }
  }
  char end_letter = '\0';
  InsertLetter(str1, end_letter, size, capacity);
  return str1;
}

void Exit(char**& stack, int& size, int& capacity) {
  ClearStack(stack, size, capacity);
  std::cout << "bye";
  exit(0);
}

int main() {
  char** stack = new char*[2];
  int capacity = 2;
  int size_of_stack = 0;
  char* input = new char[7];
  while (true) {
    std::cin >> input;
    if (strcmp(input, "push") == 0) {
      auto tmp_str = GetInput();
      PushElement(stack, tmp_str, capacity, size_of_stack);
    } else if (strcmp(input, "exit") == 0) {
      delete[] input;
      Exit(stack, size_of_stack, capacity);
    } else if (strcmp(input, "back") == 0) {
      GetBack(stack, size_of_stack);
    } else if (strcmp(input, "size") == 0) {
      GetSize(size_of_stack);
    } else if (strcmp(input, "clear") == 0) {
      ClearStack(stack, size_of_stack, capacity);
      stack = new char*[2];
      capacity = 2;
      size_of_stack = 0;
      std::cout << "ok" << '\n';
    } else {
      PopStack(stack, capacity, size_of_stack);
    }
  }
  ClearStack(stack, size_of_stack, capacity);
}
