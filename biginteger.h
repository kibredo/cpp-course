#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

enum Sign : int {
  Plus = 1,
  Minus = -1,
  Zero = 0
};

class BigInt {
 private:
  static const long long kGlobalMax = 999999999;
  std::vector<long long> data;
  void div_positive(const BigInt& other, BigInt& remnant) {
    BigInt help_ans;
    int first_ind = (int)this->data.size() - (int)other.data.size();
    remnant.data.resize(other.data.size());
    for (int tmp = first_ind; tmp < (int)data.size(); ++tmp) {
      remnant.data[tmp - first_ind] = this->data[tmp];
    }
      for (int tmp = first_ind; tmp >= 0; --tmp) {
      if (tmp != first_ind) {
        int len = remnant.data.size();
        remnant.data.resize(remnant.data.size() + 1, 0);
        for (int tmp = 0; tmp < len; ++tmp) {
          remnant.data[remnant.data.size() - tmp - 1] = remnant.data[len - tmp - 1];
        }
        remnant.data[0] = 0;
        while (remnant.data.size() > 1 && remnant.data.back() == 0) {
          remnant.data.pop_back();
        }
        remnant.data[0] = this->data[tmp];
      }
      long long left_border = 0;
      long long right_border = BigInt::kGlobalMax + 1;
      while (right_border - left_border > 1) {
        long long middle = (left_border + right_border) / 2;
        help_ans.data = other.data;
        help_ans *= (middle);
        if (IsLower(remnant, help_ans)) {
          right_border = middle;
        } else {
          left_border = middle;
        }
      }
      help_ans.data = other.data;
      help_ans *= (left_border);
      get_left_sub(remnant, help_ans);
      this->data[tmp] = left_border;
    }
    while (first_ind < (int)this->data.size() - 1) {
      this->data.pop_back();
    }
  }
  void get_the_result(const BigInt& other, long long& tmprent, int& other_tmp, std::vector<long long>& new_data, int tmp) {
    if (other_tmp < (int)other.data.size()) {
      tmprent += this->data[tmp] * other.data[other_tmp];
    }
    if (tmp + other_tmp < (int)new_data.size()) {
      tmprent += new_data[tmp + other_tmp];
      new_data[tmp + other_tmp] = tmprent % (BigInt::kGlobalMax + 1);
    } else {
      new_data.push_back(tmprent % (BigInt::kGlobalMax + 1));
    }
    tmprent /= (BigInt::kGlobalMax + 1);
    ++other_tmp;
  }
  static bool IsLower(const BigInt& first, const BigInt& second) {
    if (first.data.size() != second.data.size()) {
      return first.data.size() < second.data.size();
    }
    for (int tmp = 0; tmp < (int)first.data.size(); ++tmp) {
      if (first.data[first.data.size() - tmp - 1] != second.data[first.data.size() - tmp - 1]) {
        return first.data[first.data.size() - tmp - 1] < second.data[first.data.size() - tmp - 1];
      }
    }
    return false;
  }
  void get_positive_summed(BigInt& first, const BigInt& other) {
    long long reminder = 0;
	  long long tmprent = 0;
	  if (first.size() >= other.size()) {
  	  for (int tmp = 0; tmp < (int)first.data.size(); ++tmp) {
		    if (tmp < (int)other.size()) {
		      long long add = first.data[tmp] + other.data[tmp] + reminder;
		      reminder = 0;
		      if (add > BigInt::kGlobalMax) {
	  	      get_division(add, tmprent, reminder);
			      first.data[tmp] = tmprent;
		      } else {
			      first.data[tmp] = add;
		      }
		    } else if (reminder != 0) {
		      long long add = first.data[tmp] + reminder;
		      if (add > BigInt::kGlobalMax) {
		        get_division(add, tmprent, reminder);
		        first.data[tmp] = tmprent;
		      } else {
			      first.data[tmp] = add;
			      reminder = 0;
		      }
	      }
	    }
	    if (reminder != 0) {
		    first.data.push_back(reminder);
	    }
	  } else {
	    auto copy = first;
	    first.data = other.data;
	    first += copy;
    }
  }
  static void get_left_sub(BigInt& left, const BigInt& right) {
    unsigned long long sub = 0;
    for (int tmp = 0; tmp < (int)left.data.size(); ++tmp) {
      unsigned long long help_tmp = sub;
      unsigned long long reduced = left.data[tmp];
      sub = 0;
      if (tmp < (int)right.data.size()) {
        help_tmp += right.data[tmp];
      }
      if (reduced < help_tmp) {
        sub = 1;
        reduced += BigInt::kGlobalMax + 1;
      }
      left.data[tmp] = reduced - help_tmp;
    }
  }
  static void get_right_sub(const BigInt& left, BigInt& right) {
    unsigned long long sub = 0;
    for (int tmp = 0; tmp < (int)left.data.size(); ++tmp) {
      unsigned long long help_tmp = sub;
      unsigned long long reduced = left.data[tmp];
      sub = 0;
      if (tmp < (int)right.data.size()) {
        help_tmp += right.data[tmp];
      }
      if (reduced < help_tmp) {
        sub = 1;
        reduced += BigInt::kGlobalMax + 1;
      }
      if (tmp < (int)right.data.size()) {
        right.data[tmp] = reduced - help_tmp;
      } else {
        right.data.push_back(reduced - help_tmp);
      }
    }
  }
  static void get_reversed(long long& ans) {
    long long copy = 0;
    while (ans) {
    copy *= 10;
    copy += ans % 10;
    ans /= 10;
    }
    ans = copy;
  }
  static long long get_int(std::string str) {
    bool check = false;
    long long ans = 0;
    for (auto el : str) {
      if (!check && el == '0') {
        continue;
      }
      if (!check && el != '0') {
        check = true;
      }
      ans *= 10;
      ans += el - '0';
    }
    return ans;
  }
  static void get_division(const long long& num, long long& ans, long long& var) {
    var = num;
    ans = 0;
    int cnt = 0;
    bool was_digit = false;
    for (int tmp = 1; tmp <= 9; ++tmp) {
      ans *= 10;
    if (var % 10 == 0 && !was_digit) {
      ++cnt;
    } else {
      was_digit = true;
    }
    ans += var % 10;
    var /= 10;
    }
    while (ans % 10 == 0 && ans) {
    ans /= 10;
    }
    get_reversed(ans);
    while (cnt) {
      ans *= 10;
      --cnt;
    }
  }
 public:
  Sign sign = Zero;
  Sign get_the_sign() const {
    return this->sign;
  }
  bool is_nothing() const {
    return this->sign == Zero;
  }
  explicit operator bool() const {
    for (auto elem : this->data) {
      if (elem != 0) {
        return true;
      }
    }
    return false;
  }
  std::string toString() const {
    if (this->sign == Zero) {
      return "0";
    }
    int size = 0;
    std::string str = "";
    std::string help = "";
    if (this->sign == Minus) {
      str += "-";
    }
    for (int tmp = (int)this->data.size() - 1; tmp >= 0; --tmp) {
      if (tmp != (int)this->data.size() - 1) {
        size = 9 - std::to_string(this->data[tmp]).length();
      }
      for (int u = 0; u < size; ++u) {
        str += '0';
      }
      help = std::to_string(this->data[tmp]);
      str += help;
    }
    return str;
  }
  void get_auto_fixed() {
    while (data.back() == 0 && data.size()) {
      data.pop_back();
    }
    if (data.empty()) {
      sign = Zero;
      data.push_back(0);
    }
  }
  bool zero_check() const {
    for (auto elem : data) {
      if (elem != 0) {
        return false;
      }
    }
    return true;
  }
  BigInt(const BigInt& bint) = default;
  BigInt() = default;
  BigInt(const std::string& str): BigInt() {
    std::string temp_str = "";
    int size = (int)str.size();
    sign = Plus;
      for (int tmp = (int)str.size() - 1; tmp >= 0; --tmp) {
        if (!isdigit(str[tmp])) {
          sign = Minus;
        } else {
          temp_str += str[tmp];
          if ((size - tmp) % 9 == 0 && tmp != size - 1) {
            std::reverse(temp_str.begin(), temp_str.end());
            long long ans = get_int(temp_str);
            data.push_back(ans);
            temp_str = "";
          }
        }
      }
    if (!temp_str.empty()) {
        std::reverse(temp_str.begin(), temp_str.end());
      long long ans = get_int(temp_str);
      data.push_back(ans);
      temp_str = "";
    }
  }
  BigInt(int val) {
    BigInt copy = std::to_string(val);
    *this = copy;
  }
  std::vector<long long> get_data() const {
	  return this->data;
  }
  BigInt& operator=(const BigInt& other) = default;
  int size() const {
	  return (int)data.size();
  }
  long long first_sign_digit() const {
    if (this->zero_check()) {
      return 0ll;
    }
    return data[(int)data.size() - 1];
  }
  BigInt& operator+=(const BigInt& other) {
    this->get_auto_fixed();
    if (this->sign == Zero) {
      *this = other;
      return *this;
    }
    if (this->sign == other.sign) {
      get_positive_summed(*this, other);
      return *this;
    } else if (IsLower(*this, other)) {
      get_right_sub(other, *this);
      this->sign = other.sign;
    } else {
      get_left_sub(*this, other);
    }
    this->get_auto_fixed();
    return *this;
  }
  BigInt& operator*=(const long long& other) {
    this->get_auto_fixed();
    long long reminder = 0;
	  long long tmprent = 0;
	  for (int tmp = 0; tmp < (int)this->data.size(); ++tmp) {
		  long long add = this->data[tmp] * other + reminder;
      if (add > BigInt::kGlobalMax) {
        get_division(add, tmprent, reminder);
      } else {
        tmprent = add;
        reminder = 0;
      }
      this->data[tmp] = tmprent;
	  }
    if (reminder != 0) {
      this->data.push_back(reminder);
    }
    return *this;
  }
  BigInt& operator*=(const BigInt& other) {
    this->get_auto_fixed();
    std::vector<long long> new_data;
    for (int tmp = 0; tmp < (int)data.size(); ++tmp) {
      long long tmprent = 0;
      int other_tmp = 0;
      this->get_the_result(other, tmprent, other_tmp, new_data, tmp);
      while (tmprent > 0 || other_tmp < (int)other.data.size()) {
        this->get_the_result(other, tmprent, other_tmp, new_data, tmp);
      }
    }
    this->data = new_data;
    if (other.sign == Minus) {
      if (this->sign == Plus) {
        this->sign = Minus;
      } else if (this->sign == Minus) {
        this->sign = Plus;
      }
    }
    if (other.sign == Zero) {
      this->sign = Zero;
    }
    return *this;
  }
  BigInt& operator-=(const BigInt& other) {
    BigInt copy = other;
    BigInt the_one = -1;
    copy *= the_one;
    *this += copy;
    return *this;
  }
  BigInt& operator/=(const BigInt& other) {
    this->get_auto_fixed();
    if (this->sign == Zero || IsLower(*this, other)) {
      *this = 0;
      return *this;
    }
    Sign final_sign = Zero;
    if (this->sign == other.sign) {
      final_sign = Plus;
    } else {
      final_sign = Minus;
    }
    BigInt remnant;
    div_positive(other, remnant);
    this->sign = final_sign;
    this->get_auto_fixed();
    return *this;
  }
  BigInt& operator++() {
    return *this += 1;
  }
  BigInt& operator--() {
    return *this -= 1;
  }
  BigInt operator++(int) {
    BigInt result = *this;
    operator++();
    return result;
  }
  BigInt operator--(int) {
    BigInt result = *this;
    operator--();
    return result;
  }
  bool is_negative() const {
    return sign == Minus;
  }
  void set_data(std::vector<long long> new_data) {
    std::swap(this->data, new_data);
  }
  void set_sign(Sign sgn) {
    this->sign = sgn;
  }
  BigInt operator-() const {
    BigInt result = *this;
    if (result.sign == Plus) {
      result.sign = Minus;
    } else if (result.sign == Minus) {
      result.sign = Plus;
    }
    result.get_auto_fixed();
    return result;
  }
  BigInt operator%=(const BigInt& other) {
    this->get_auto_fixed();
    if (IsLower(*this, other)) {
      return *this;
    }
    BigInt remnant;
    div_positive(other, remnant);
    this->data = remnant.data;
    this->get_auto_fixed();
    return *this;
  }
  static BigInt GCD(BigInt a, BigInt b);
  static BigInt LCD(const BigInt& a, const BigInt& b);
  static std::pair<std::string, int> GetTheString(std::istream& in);
};

BigInt operator ""_bi(unsigned long long bint) {
  return BigInt(bint);
}
BigInt operator/(const BigInt& a, const BigInt& b) {
  BigInt result = a;
  result /= b;
  result.get_auto_fixed();
  return result;
}
BigInt operator*(const BigInt& a, long long c) {
  BigInt result = a;
  result *= c;
  result.get_auto_fixed();
  return result;
}
BigInt operator%(const BigInt& a, const BigInt& b) {
  BigInt result = a;
  result %= b;
  result.get_auto_fixed();
  return result;
}
BigInt operator*(const BigInt& a, const BigInt& b) {
  BigInt result = a;
  result *= b;
  result.get_auto_fixed();
  return result;
}
BigInt operator-(const BigInt& a, const BigInt& b) {
  BigInt result = a;
  result -= b;
  result.get_auto_fixed();
  return result;
}
BigInt operator+(const BigInt& a, const BigInt& b) {
  BigInt result = a;
  result += b;
  result.get_auto_fixed();
  return result;
}

bool operator<(const BigInt& a, const BigInt& b) {
  if (a.get_the_sign() < b.get_the_sign()) {
    return true;
  }
  if (a.get_the_sign() > b.get_the_sign()) {
    return false;
  }
  auto veca = a.size();
  auto vecb = b.size();
  if (a.get_the_sign() == Zero) {
    return false;
  }
  if (a.get_the_sign() == Plus) {
    if (veca < vecb) {
	    return true;
    }
    if (veca == vecb) {
	    return a.first_sign_digit() < b.first_sign_digit();
    }
    return false;
  }
  if (veca < vecb) {
	    return false;
    }
    if (veca == vecb) {
	    return a.first_sign_digit() > b.first_sign_digit();
    }
    return true;
}
bool operator==(const BigInt& a, const BigInt& b) {
  return !(a < b) && !(b < a);
}
bool operator!=(const BigInt& a, const BigInt& b) {
	return !(a == b);
}
bool operator<=(const BigInt&a, const BigInt&b) {
  return a < b || a == b;
}
bool operator>(const BigInt&a, const BigInt&b) {
  return !(a <= b);
}
bool operator>=(const BigInt&a, const BigInt&b) {
  return !(a < b);
}

  BigInt BigInt::GCD(BigInt a, BigInt b) {
    if (b.is_nothing()) {
      return a;
    }
    a.set_sign(Plus);
    b.set_sign(Plus);
    return GCD(b, a % b);
  }
  BigInt BigInt::LCD(const BigInt& a, const BigInt& b) {
    return (a * b) / GCD(a,b);
  }

std::ostream& operator<<(std::ostream& out, const BigInt& bint) {
  if (bint.is_negative()) {
    out << "-";
  }
  bool was_first_significant_digit = false;
  auto data = bint.get_data();
  bool check = false;
  for (auto el : data) {
    if (el != 0) {
      check = true;
      break;
    }
  }
  if (!check) {
    out << 0;
    return out;
  }
  for (int tmp = (int)data.size() - 1; tmp >= 0; --tmp) {
  	if (was_first_significant_digit) {
  	  int len = std::to_string(data[tmp]).length();
        for (int xtmp = 0; xtmp < 9 - len; ++xtmp) {
  	    out << '0';
  	  }
  	  out << data[tmp];
	  } else if (data[tmp] != 0) {
	  	out << data[tmp];
	  	was_first_significant_digit = true;
	  }
  }
  return out;
}
std::pair<std::string, int> BigInt::GetTheString(std::istream& in) {
  char letter;
  Sign sign = Plus;
  std::string ans = "";
  while (!in.eof() && in.get(letter)) {
    if (std::isspace(letter) == 0) {
      break;
    }
  }
  if (letter == '-') {
    sign = Minus;
  } else if (!std::isdigit(letter) && letter != '+') {
    return {ans, sign};
  }
  if (std::isdigit(letter)) {
    ans += letter;
  }
  while (!in.eof() && in.get(letter)) {
    if (std::isdigit(letter) == 0) {
      break;
    }
    ans += letter;
  }
  return {ans, sign};
}
std::istream& operator>>(std::istream& in, BigInt& bint) {
  std::pair<std::string, int> in_pair = BigInt::GetTheString(in);
  std::string input = in_pair.first;
  Sign the_sign = in_pair.second == -1 ? Minus : Plus;
  BigInt result(input);
  result.set_sign(the_sign);
  bint = result;
  bint.get_auto_fixed();
  return in;
}

class Rational {
 private:
  BigInt upper = 0;
  BigInt down = 1;
  void get_self_reduced() {
    if (this->upper.is_nothing()) {
      this->down = 1;
      return;
    }
    BigInt divider = BigInt::GCD(this->upper, this->down);
    this->upper /= divider;
    this->down /= divider;
  }
 public:
  void set_frac_sign(Sign sign) {
    this->upper.set_sign(sign);
    this->down.set_sign(Plus);
  }
  Sign get_sign() const {
    return this->upper.get_the_sign();
  }
  Rational operator-() const {
    Rational ans = *this;
    ans.upper.set_sign(this->upper.get_the_sign());
    if (ans.upper.sign == Plus) {
      ans.upper.sign = Minus;
    } else if (ans.upper.sign == Minus) {
      ans.upper.sign = Plus;
    }
    return ans;
  }
  std::string toString() const {
    std::string ans = "";
      ans += this->upper.toString();
      if (this->down == 1) {
        return ans;
      }
      ans += '/';
      std::string extra_help = "";
      std::string help_ans = this->down.toString();
      for (auto elem : help_ans) {
        if (elem != '-') {
          extra_help += elem;
        }
      }
      ans += extra_help;
      return ans;
    }
  Rational(): upper(0), down(1) {}
  Rational(int32_t val) : upper(val), down(1) {
    this->get_self_reduced();
  }
  Rational(const BigInt& bint) : upper(bint), down(1) {
    this->get_self_reduced();
  }
  Rational(const Rational&) = default;
  Rational& operator=(const Rational&) = default;
  Rational& operator=(const int32_t bint) {
    this->upper = bint;
    this->down = 1;
    return *this;
  }
  Rational& operator=(const BigInt& bint) {
    this->upper = bint;
    this->down = 1;
    return *this;
  }
  Rational& operator*=(const Rational& other) {
    this->get_self_reduced();
    this->down *= other.down;
    this->upper *= other.upper;
    this->get_self_reduced();
    return *this;
  }
  Rational& operator+=(const Rational& other) {
    this->get_self_reduced();
    BigInt down_gcd = BigInt::GCD(this->down, other.down);
    BigInt down_lcd = BigInt::LCD(this->down, other.down);
    this->upper *= other.down;
    this->upper /= down_gcd;
    this->upper += (other.upper * this->down) / down_gcd;
    this->down = down_lcd;
    this->get_self_reduced();
    return *this;
  }
  Rational& operator-=(const Rational& other) {
    Rational copy = other;
    copy *= -1;
    *this += copy;
    return *this; 
  }
  Rational& operator/=(const Rational& other) {
    this->get_self_reduced();
    Sign sign = this->upper.get_the_sign();
    sign = other.upper.get_the_sign() == Minus ? (sign == Plus ? Minus : Plus) : sign;
    sign = other.upper.get_the_sign() == Zero ? Zero : sign; 
    this->upper *= other.down;
    this->down *= other.upper;
    this->set_frac_sign(sign);
    this->get_self_reduced();
    return *this;
  }
  BigInt get_upper() const {
    return this->upper;
  }
  BigInt get_down() const {
    return this->down;
  }
  std::string asDecimal(int number_of_digids = 0) const {
    std::string ans = "";
    Rational copy = *this;
    if (this->upper.is_negative()) {
      ans = "-";
      copy.upper.set_sign(Plus);
    }
    ans += (copy.upper / copy.down).toString();
    BigInt remnant = copy.upper % copy.down;
    int zeros = number_of_digids / 9;
    auto data = remnant.get_data();
    int size = (int)data.size();
    data.resize(data.size() + zeros, 0);
    for (int tmp = 0; tmp < size; ++tmp) {
      data[data.size() - tmp - 1] = data[size - tmp - 1];
    }
    for (int tmp = 0; tmp < zeros; ++tmp) {
      data[tmp] = 0;
    }
    remnant.set_data(data);
    remnant.get_auto_fixed();
    int arg = 1;
    for (int tmp = 0; tmp < number_of_digids % 9; ++tmp) {
      arg *= 10;
    }
    remnant *= arg;
    if (remnant.is_negative()) {
      remnant.set_sign(Plus);
    }
    if (number_of_digids > 0) {
      ans += '.';
      std::string extra_help = "";
      std::string help_ans = (remnant / this->down).toString();
      for (auto elem : help_ans) {
        if (elem != '-') {
          extra_help += elem;
        }
      }
      std::swap(extra_help, help_ans);
      if ((int)help_ans.size() < number_of_digids) {
        ans += std::string(number_of_digids - help_ans.size(), '0');
      }
      ans += help_ans;
    }
    return ans;
  }
  explicit operator double() const {
    double result;
    result = std::stod(this->asDecimal(310));
    return result;
  }
};

Rational operator*(const Rational& a, const Rational& b) {
  Rational result = a;
  result *= b;
  return result;
}
Rational operator/(const Rational& a, const Rational& b) {
  Rational result = a;
  result /= b;
  return result;
}
Rational operator+(const Rational& a, const Rational& b) {
  Rational result = a;
  result += b;
  return result;
}
Rational operator-(const Rational& a, const Rational& b) {
  Rational result = a;
  result -= b;
  return result;
}

bool operator<(const Rational& a, const Rational& b) {
  if (a.get_sign() != b.get_sign()) {
    return a.get_sign() < b.get_sign();
  }
  return a.get_upper() * b.get_down() < a.get_down() * b.get_upper();
}
bool operator<=(const Rational& a, const Rational& b) {
  return !(b < a);
}
bool operator>(const Rational& a, const Rational& b) {
  return !(a <= b);
}
bool operator>=(const Rational& a, const Rational& b) {
  return !(a < b);
}
bool operator==(const Rational& a, const Rational& b) {
  return !(a < b) && !(b < a);
}
bool operator!=(const Rational& a, const Rational& b) {
  return !(a == b);
}