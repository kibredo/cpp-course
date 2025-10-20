#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

// Всё исправил

namespace K {
  const double kDelta = 1e-6;
}

struct Vector;
struct Line;
struct Shape;

struct Point {
  Point() = default;
  double x = 0;
  double y = 0;
  double Distance(const Point& other) const {
    return (double)std::sqrt((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y));
  }
  Point(double a, double b): x(a), y(b) {}
  bool operator==(const Point& other) const {
    return this->x - other.x < K::kDelta && this->y - other.y < K::kDelta;
  }
  bool operator!=(const Point& other) const {
    return !(*this == other);
  }
  void rotate(const Point& center, double angle) {
    angle /= (double)180;
    angle *= M_PI;
    if (std::abs(angle) < K::kDelta) {
      return;
    }
    auto old_x = this->x;
    auto old_y = this->y;
    this->x = ((old_x - center.x) * std::cos(angle) - (old_y - center.y) * std::sin(angle) + center.x);
    this->y = ((old_x - center.x) * std::sin(angle) + (old_y - center.y) * std::cos(angle) + center.y);
  }
  void reflect(const Line& axis);
  void reflect(const Point& center) {
    this->rotate(center, 180);
  }
  static double cross_product(const Point& A, const Point& B, const Point& C, const Point& D) {
    return (B.x - A.x) * (D.y - C.y) - (D.x - C.x) * (B.y - A.y);
  }
  static double signed_angle(const Point& A, const Point& B, const Point& C, const Point& D) {
    return atan2(cross_product(A, B, C, D), (B.x - A.x) * (D.x - C.x) + (B.y - A.y) * (D.y - C.y));
  }
  static double angle(const Point& A, const Point& B, const Point& C, const Point& D) {
    return fabs(signed_angle(A, B, C, D));
  }
  Point point_scale(const Point& center, const Point& p, double coefficient) const {
    return Point(center.x + (p.x - center.x) * coefficient, center.y + (p.y - center.y) * coefficient);
  }
  void scale(const Point& center, double coefficient) {
    *this = point_scale(center, *this, coefficient);
  }
  Point operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
  }
  Point operator*(double k) const {
    return Point(x * k, y * k);
  }
  
};

struct Vector {
  double x = 0;
  double y = 0;
  Vector() = default;
  Vector(double x, double y): x(x), y(y) {}
  Vector(const Vector& other) = default;
  Vector(const Point& a, const Point& b) {
    this->x = b.x - a.x;
    this->y = b.y - a.y;
  }
  Vector operator/(double k) const {
    return *this * (1 / k);
  }
  Vector operator*(double k) const {
    return Vector(this->x * k, this->y * k);
  }
  Vector operator+(const Vector& other) const {
    return Vector(Point(0, 0), Point(x + other.x, y + other.y));
  }
};

Point operator+(const Point& p, const Vector& other) {
  return Point(p.x + other.x, p.y + other.y);
}
double operator*(const Vector& a, const Vector& b) {
  return a.x * b.y - a.y * b.x;
}

struct Shape {
  virtual double perimeter() const = 0;
  virtual double area() const  = 0;
  virtual bool operator==(const Shape& other) const = 0;
  virtual bool isCongruentTo(const Shape& other) const = 0;
  virtual bool isSimilarTo(const Shape& other) const = 0;
  virtual bool containsPoint(const Point& point) const = 0;
  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void scale(const Point& center, double k) = 0;
  virtual ~Shape() = default;
};

struct Line {
  double eq_b = 0;
  bool isVertical = false;
  Point first = Point(0,0); // y(0)
  Point second = Point(1,1); // y(1)
  double kf = 0; // строим прямую черещ нашу точку и k = -1/kf
  void set_eq() {
    this->eq_b = this->first.y;
  }
  Line(const Point& a, const Point& b) {
    if (b.x - a.x < K::kDelta) {
        isVertical = true;
    } else {
        isVertical = false;
      kf = (double)((b.y - a.y) / (b.x - a.x));
    }
    this->first.x = 0;
    this->first.y = (a.y - kf * a.x);
    this->second.x = 1;
    this->second.y = this->first.y + kf;
    this->set_eq();
  }
  Line(double nk, double delta) {
    this->kf = nk;
    isVertical = false;
    this->first.x = 0;
    this->first.y = -1 * delta * nk;
    this->second.x = 1;
    this->second.y = -1 * (delta - 1) * nk;
    if (this->second.y < K::kDelta) {
      this->second.y = 0;
    }
    if (this->first.y < K::kDelta) {
      this->first.y = 0;
    }
    this->set_eq();
  }
  Line(const Point& a, double k) {
    Point b(a.x + 1, a.y + k);
    *this = Line(a, b);
    this->set_eq();
  }
  bool operator==(const Line& other) const {
    return first == other.first && second == other.second;
  }
  bool operator!=(const Line& other) const {
    return !(*this == other);
  }
  void test() {
    std::cout << this->first.x << " " << this->first.y << '\n';
    std::cout << this->second.x << " " << this->second.y;
    std::cout << " " << this->kf;
  }
};
void Point::reflect(const Line& axis) {
  Point center(0, 0);
  Line help(*this, (-1) / axis.kf);
  center.x = (-1) * (axis.eq_b - help.eq_b) / (axis.kf + 1 / axis.kf);
  center.y = (-1) * (help.kf * axis.eq_b - axis.kf * help.eq_b) / (axis.kf + 1 / axis.kf);
  this->rotate(center, 180);
}
Vector operator-(const Point& a, const Point& b) {
  return Vector(a.x - b.x, a.y - b.y);
}
double vec_sub(const Vector& a, const Vector& b) {
  return a.x * b.y - a.y * b.x;
}
struct Polygon : Shape {
  std::vector<Point> vertex;
  int size = 0;
  virtual double perimeter() const override {
    Point current = vertex[0];
    double ans = 0;
    for (int tmp = 1; tmp <= this->size; ++tmp) {
      ans += current.Distance(this->vertex[tmp % size]);
      current = this->vertex[tmp % size];
    }
    return ans;
  }
  double area() const override {
    double ans = 0;
    for (size_t tmp = 1; tmp < vertex.size() - 1; ++tmp) {
      ans += vec_sub(vertex[tmp] - vertex[0], vertex[tmp + 1] - vertex[0]);
    }
    return std::abs(ans) / 2.0;
  }
  Polygon(std::vector<Point> v): vertex(v), size((int)v.size()) {}
  template <typename ...Args>
  Polygon(Args ...g) {
    std::vector<Point> tmp{g...};
    this->vertex = tmp;
    this->size = (int)this->vertex.size();
  }
  std::vector<Point> getVertices() const {
    return vertex;
  }
  int vertexCount() const {
    return size;
  }
  void scale(const Point& center, double k) override {
    for (int tmp = 0; tmp < size; ++tmp) {
      this->vertex[tmp].scale(center, k);
    }
  }
  bool do_the_left_thing(const Polygon& p1, const Polygon& p2) const {
    int size = p1.size;
    for (int v = 0; v < size; ++v) {
      bool is_equal = true;
      for (int i = 0; i < size; ++i) {
        Point next_first = p1.vertex[(i + 1) % size];
        Point double_next_1 = p1.vertex[(i + 2) % size];
        Point next_second = p2.vertex[(i + v + 1) % size];
        Point double_next_2 = p2.vertex[(i + v + 2) % size];
        if (p1.vertex[i].Distance(next_first)- p2.vertex[(i + v) % size].Distance(next_second) >= K::kDelta) {
          is_equal = false;
          break;
        }
        if (Point::angle(p1.vertex[i], next_first, next_first, double_next_1) - Point::angle(p2.vertex[(i + v) % size], next_second, next_second, double_next_2) >= K::kDelta) {
          is_equal = false;
          break;
        }
      }
      if (is_equal) {
        return true;
      }
    }
    return false;
  }
  bool do_the_right_thing(const Polygon& p1, const Polygon& p2) const {
    int size = p1.size;
    for (int v = 0; v < size; ++v) {
      bool is_equal = true;
      for (int i = 0; i < size; ++i) {
        Point next_first = p1.vertex[(i + 1) % size];
        Point double_next_1 = p1.vertex[(i + 2) % size];
        Point next_second = p2.vertex[(-i + 2 * size + v - 1) % size];
        Point double_next_2 = p2.vertex[(-i + 2 * size + v - 2) % size];
        if (p1.vertex[i].Distance(next_first)- p2.vertex[(-i + size + v) % size].Distance(next_second) >= K::kDelta) {
          is_equal = false;
          break;
        }
        if (Point::angle(p1.vertex[i], next_first, next_first, double_next_1) - Point::angle(p2.vertex[(-i + size + v) % size], next_second, next_second, double_next_2) >= K::kDelta) {
          is_equal = false;
          break;
        }
      }
      if (is_equal) {
        return true;
      }
    }
    return false;
  }
  bool isCongruentTo(const Shape& other) const override {
    const Polygon* ptr = dynamic_cast<const Polygon*>(&other);
    if (ptr == nullptr) {
      return false;
    }
    if (this->size != ptr->size) {
      return false;
    }
    return (do_the_left_thing(*this, *ptr) || do_the_right_thing(*this, *ptr));
  }
  bool isSimilarTo(const Shape& other) const override {
    const Polygon* ptr = dynamic_cast<const Polygon*>(&other);
    if (ptr == nullptr) {
      return false;
    }
    if (this->size != ptr->size) {
      return false;
    }
    Polygon scaled(ptr->vertex);
    scaled.scale(Point(0, 0), this->perimeter() / ptr->perimeter());
    return this->isCongruentTo(scaled);
  }
  static double CrossProduct(const std::vector<Point>& a) {
     double x1 = a[1].x -  a[0].x;
     double x2 = a[2].x - a[0].x;
     double y1 = a[1].y - a[0].y;
     double y2 = a[2].y - a[0].y;
     return (x1 * y2 - y1 * x2);
  }
  bool isConvex() const {
    int N = (int)vertex.size();
    double prev = 0;
    double curr = 0;
    for (int tmp = 0; tmp < N; tmp++) {
      std::vector<Point> temp  = { vertex[tmp], vertex[(tmp + 1) % N], vertex[(tmp + 2) % N] };
      curr = Polygon::CrossProduct(temp);
      if (curr > K::kDelta || curr < (-1) * K::kDelta) {
        if (curr * prev < 0) {
          return false; 
        }
        else {
          prev = curr;
        }   
      }  
    }   
    return true;
  }
  virtual void reflect(const Point& center) override {
    this->rotate(center, 180);
  }
  virtual bool operator==(const Shape& other) const override {
    auto ptr = dynamic_cast<const Polygon*>(&other);
    if (ptr == nullptr) {
      return false;
    }
    if (this->size != ptr->size) {
      return false;
    }
    for (int index = 0; index < size; ++index) {
      for (int tmp = 0; tmp < size; ++tmp) {
        if (this->vertex[tmp] != ptr->vertex[(tmp + index) % this->size]) {
          break;
        } else {
          if (tmp == size - 1) {
            return true;
          }
        }
      }
    }
    for (int index = 0; index < size; ++index) {
      for (int tmp = 0; tmp < size; ++tmp) {
        if (this->vertex[tmp] != ptr->vertex[(size - tmp + index) % this->size]) {
          break;
        } else {
          if (tmp == size - 1) {
            return true;
          }
        }
      }
    }
    return false;
  }
  virtual bool operator!=(const Shape& other) {
    return !(*this == other);
  }
  virtual void reflect(const Line& axis) override {
    for (int tmp = 0; tmp < this->size; ++tmp) {
      this->vertex[tmp].reflect(axis);
    }
  }
  virtual void rotate(const Point& center, double angle) override {
    for (int tmp = 0; tmp < this->size; ++tmp) {
      this->vertex[tmp].rotate(center, angle);
    }
  }
  bool containsPoint(const Point& p) const override {
    double ans = 0;
    for (int tmp = 0; tmp < (int)this->vertex.size(); ++tmp) {
      Point p1 = this->vertex[(tmp + 1) % (int)this->vertex.size()];
      Point p2 = this->vertex[tmp];
      ans += signed_angle(p, p2, p, p1);
    }
    return std::abs(std::abs(ans) - 2 * M_PI) < K::kDelta;
  }
 private:
  static double signed_angle(Point p1, Point p2, Point p3, Point p4) {
    return atan2(Point::cross_product(p1, p2, p3, p4), (p2.x - p1.x) * (p4.x - p3.x) + (p2.y - p1.y) * (p4.y - p3.y));
  }
};
struct Ellipse : Shape {
  Point Fa;
  Point Fb;
  double length = 0;
  double a = 0;
  double b = 0;
  double e = 0;
  Ellipse(const Point& x, const Point& y, double d): Fa(x), Fb(y), length(d), a(d / 2) {
    b = std::sqrt(d * d / 4 + Fa.Distance(Fb) * Fa.Distance(Fb) / 4);
    this->RushE();
  }
  virtual void reflect(const Point& center) override {
    this->rotate(center, 180);
  }
  void RushE() {
    this->e = this->Fa.Distance(this->Fb) / this->length;
  }
  double eccentricity() const {
    return this->e;
  }
  std::pair<Point, Point> focuses() const {
    return std::make_pair(Fa, Fb);
  }
  std::pair<Line, Line> directrices() const {
    return {Line(a / e, 0), Line((-1) * a / e, 1)};
  }
  virtual void rotate(const Point& center, double angle) override {
    this->Fa.rotate(center, angle);
    this->Fb.rotate(center, angle);
  }
  virtual double area() const override {
    return (double)M_PI * this->a * this->a * std::sqrt(1 - this->e * this->e);
  }
  virtual double perimeter() const override {
    return 4 * this->a; //* std::comp_ellint_2(this->e);
  }
  virtual void reflect(const Line& axis) override {
    this->Fa.reflect(axis);
    this->Fb.reflect(axis);
  }
  virtual bool isCongruentTo(const Shape& other) const override {
    auto ptr = &other;
    auto tempe = dynamic_cast<const Ellipse*>(ptr);
    if (tempe != nullptr) {
      return std::abs(this->a - tempe->a) < K::kDelta && std::abs(this->b - tempe->b) < K::kDelta;
    }
    return false;
  }
  bool operator==(const Shape& other) const override {
    const Ellipse* ptr = dynamic_cast<const Ellipse*>(&other);
    if (ptr == nullptr) {
      return false;
    }
    return this->Fa == ptr->Fa && this->Fb == ptr->Fb && this->length == ptr->length;
  }
  virtual bool isSimilarTo(const Shape& other) const override {
    auto ptr = &other;
    auto tempe = dynamic_cast<const Ellipse*>(ptr);
    if (tempe != nullptr) {
      return std::abs(this->a * tempe->b - this->b * tempe->a) < K::kDelta;
    }
    return false;
  }
  virtual void scale(const Point& center, double k) override {
    this->Fa.scale(center, k);
    this->Fb.scale(center, k);
    this->length *= k;
  }
  virtual bool containsPoint(const Point& p) const override {
    return p.Distance(Fa) + p.Distance(Fb) - length < K::kDelta;
  }
};
  struct Circle : public Ellipse {
    bool operator==(const Circle& other) const {
      return this->radius() == other.radius() && this->center() == other.center();
    }
    Circle(Point center, double radius): Ellipse(center, center, radius * 2) {
      this->Fa = center;
      this->Fb = center;
      this->length = radius * 2;
      this->a = this->b = this->length;
    }
    double radius() const {
      return this->length / 2;
    }
    Point center() const {
      return this->Fa;
    }
   };
  struct Rectangle : Polygon {
    double frac = 0;
    Point center() const {
      Point a = vertex[0];
      Point b = vertex[2];
      return Point(0.5 * a.x + 0.5 * b.x, 0.5 * a.y + 0.5 * b.y);
    }
    Rectangle(const Point& a, const Point& b, double frac): frac(frac) {
      this->size = 4;
      this->vertex.resize(4);
      frac = std::max(frac, 1 / frac);
      Point delta = Point(a.x - b.x, a.y - b.y);
      double r = a.Distance(b) / std::sqrt(frac * frac + 1);
      double angle = atan2(delta.y, delta.x) + atan(frac);
      angle += M_PI / 2.0;
      double k1 = r * cos(angle);
      double k2 = r * sin(angle);
      this->vertex[1] = Point(a.x + k1, a.y + k2);
      this->vertex[0] = a;
      this->vertex[2] = b;
      Point c = this->vertex[1];
      c.reflect(Point(a.x * 0.5 + b.x * 0.5, a.y * 0.5 + b.y * 0.5));
      this->vertex[3] = c;
    }
    std::pair<Line, Line> diagonals() const {
      std::pair<Line, Line> ans = {Line(vertex[0], vertex[2]), Line(vertex[1], vertex[3])};
      return ans;
    }
  };
  struct Square : Rectangle {
    Square(const Point& a, const Point& b): Rectangle(a, b, 1.0) {}
    Circle circumscribedCircle() const {
      return Circle(center(), 0.5 * vertex[0].Distance(vertex[2]));
    }
    Circle inscribedCircle() const {
      return Circle(center(), 0.5 * vertex[0].Distance(vertex[2]) / std::sqrt(2));
    }
  };
    struct Triangle : Polygon {
      Triangle(const Point& A, const Point& B, const Point& C) {
        std::vector<Point> ans({A, B, C});
        this-> vertex = ans;
      }
      Circle circumscribedCircle() const {
        Point A = vertex[0];
        Point B = vertex[1];
        Point C = vertex[2];
        double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
        double Ux = (A.x * A.x + A.y * A.y) * (B.y - C.y);
        Ux += (B.x * B.x + B.y * B.y) * (C.y - A.y);
        Ux += (C.x * C.x + C.y * C.y) * (A.y - B.y);
        Ux /= D;
        double Uy = (A.x * A.x + A.y * A.y) * (B.x - C.x);
        Uy += (B.x * B.x + B.y * B.y) * (C.x - A.x);
        Uy += (C.x * C.x + C.y * C.y) * (A.x - B.x);
        Uy /= D;
        Point the_center(Ux, (-1) * Uy);
        return Circle(the_center, the_center.Distance(A));
      }
      Circle inscribedCircle() const {
        Point A = vertex[0];
        Point B = vertex[1];
        Point C = vertex[2];
        double radius = 2* this->area() / (A.Distance(B) + B.Distance(C) + C.Distance(A));
        Point O(0, 0);
        Vector OA(O, A);
        Vector OB(O, B);
        Vector OC(O, C);
        Point center = O + (OA * B.Distance(C) + OB * C.Distance(A) + OC * A.Distance(B)) / (A.Distance(B) + B.Distance(C) + C.Distance(A));
        return Circle(center, radius);
      }
      Point centroid() const {
        Point ans = vertex[0] + vertex[1] + vertex[2];
        ans.x /= 3;
        ans.y /= 3;
        return ans;
      }
      Point orthocenter() const {
        Point A = vertex[0];
        Point B = vertex[1];
        Point C = vertex[2];
        double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
        double Ux = (A.x * A.x + A.y * A.y) * (B.y - C.y);
        Ux += (B.x * B.x + B.y * B.y) * (C.y - A.y);
        Ux += (C.x * C.x + C.y * C.y) * (A.y - B.y);
        Ux /= D;
        double Uy = (A.x * A.x + A.y * A.y) * (B.x - C.x);
        Uy += (B.x * B.x + B.y * B.y) * (C.x - A.x);
        Uy += (C.x * C.x + C.y * C.y) * (A.x - B.x);
        Uy /= D;
        Point O(Ux, Uy);
        Vector va(O, A);
        Vector vb(O, B);
        Vector vc(O, C);
        Vector oh(va + vb + vc);
        return O + oh;
      }
      Line EulerLine() const {
        Point A = vertex[0];
        Point B = vertex[1];
        Point C = vertex[2];
        double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
        double Ux = (A.x * A.x + A.y * A.y) * (B.y - C.y);
        Ux += (B.x * B.x + B.y * B.y) * (C.y - A.y);
        Ux += (C.x * C.x + C.y * C.y) * (A.y - B.y);
        Ux /= D;
        double Uy = (A.x * A.x + A.y * A.y) * (B.x - C.x);
        Uy += (B.x * B.x + B.y * B.y) * (C.x - A.x);
        Uy += (C.x * C.x + C.y * C.y) * (A.x - B.x);
        Uy /= D;
        Point O(Ux, Uy);
        return Line(O, this->orthocenter());
      }
      Circle ninePointsCircle() const {
        auto H = this->orthocenter();
        auto omega = this->circumscribedCircle();
        omega.scale(H, 0.5);
        return omega;
      }
    };