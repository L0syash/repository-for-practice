#include<iostream>
#include<stdexcept>

namespace topit {
  struct p_t {
    int x, y;
  };
  struct f_t {
    p_t aa, bb;
  };
  size_t rows(f_t fr);
  size_t cols(f_t fr);
  bool operator==(p_t a, p_t b);
  bool operator!=(p_t a, p_t b);
  struct IDraw
  {
    virtual p_t begin() const = 0;
    virtual p_t next(p_t prev) const = 0;
    virtual ~IDraw() = default;
  };
  struct Dot: IDraw
  {
    explicit Dot(p_t dd);
    p_t begin() const override;
    p_t next(p_t prev) const override;
    p_t d;
  };
  struct Vline: IDraw
  {
    explicit Vline(p_t start, int length);
    p_t begin() const override;
    p_t next(p_t prev) const override;
    p_t s;
    int l;
  };
  struct Hline: IDraw
  {
    explicit Hline(p_t start, int length);
    p_t begin() const override;
    p_t next(p_t prev) const override;
    p_t s;
    int l;
  };
  p_t* extend(const p_t* pts, size_t s, p_t fill);
  void extend(p_t** pts, size_t& s, p_t fill);
  void append (const IDraw* sh, p_t ** ppts, size_t& s);
  f_t frame(const p_t * pts, size_t s);
  char * canvas(f_t fr, char fill);
  void paint(p_t pl, char * cnv, f_t fr, char fill);
  void flush(std::ostream& os, const char* cnv, f_t fr);
}
int main() {
  using namespace topit;
  int err = 0;
  IDraw* shp[3] = {};
  p_t * pts = nullptr;
  size_t s = 0;
  try {
    shp[0] = new Dot({0, 0});
    shp[1] = new Hline({-5, -2}, -6);
    shp[2] = new Vline({2, 3}, -4); 
    for (size_t i = 0; i < 3; ++i) {
      append(shp[i], &pts, s);
    }
    f_t fr = frame(pts, s);
    char * cnv = canvas(fr, '.');
    for (size_t i = 0; i < s; ++i) {
      paint(pts[i], cnv, fr, '#');
    }
    flush(std::cout, cnv, fr);
    delete [] cnv;
  }
  catch(...){
    std::cerr << "Error!\n";
    err = 1;
  }
  delete shp[2];
  delete shp[1];
  delete shp[0];
  return err;
}
topit::p_t* topit::extend(const p_t* pts, size_t s, p_t fill) {
  p_t* r = new p_t[s + 1];
  for (size_t i = 0; i < s; i++) {
    r[i] = pts[i];
  }
  r[s] = fill;
  return r;
}
void topit::extend(p_t** pts, size_t& s, p_t fill){
  p_t* r = extend(*pts, s, fill);
  delete [] *pts;
  s++;
  *pts = r ;
}
void topit::append(const IDraw* sh, p_t ** ppts, size_t& s) {
  extend(ppts, s, sh->begin());
  p_t b = sh->begin();
  while (sh->next(b) != sh->begin()) {
    b = sh -> next(b);
    extend(ppts, s, b);
  }
}
void topit::paint(p_t p, char * cnv, f_t fr, char fill) {
  size_t dx = p.x - fr.aa.x;
  size_t dy = fr.bb.y - p.y;
  cnv[dy * cols(fr) + dx] = fill;
}
void topit::flush(std::ostream& os, const char* cnv, f_t fr) {
  for (size_t i = 0; i < rows(fr); ++i) {
    for (size_t j = 0; j < cols(fr); ++j) {
      os << cnv[i * cols(fr) + j];
    }
    os << "\n";
  }
}
char * topit::canvas(f_t fr, char fill) {
  size_t s = rows(fr) * cols(fr);
  char * c = new char[s];
  for (size_t i = 0; i < s; ++i) {
    c[i] = fill;
  }
  return c;
}
topit::f_t topit::frame(const p_t* pts, size_t s) {
  int minx = pts[0].x, miny = pts[0].y;
  int maxx = minx, maxy = miny;
  for (size_t i = 1; i < s; ++i) {
    minx = std::min(minx, pts[i].x);
    miny = std::min(miny, pts[i].y);
    maxx = std::max(maxx, pts[i].x);
    maxy = std::max(maxy, pts[i].y);
  }
  p_t a{minx, miny};;
  p_t b{maxx, maxy};
  return f_t{a, b};
}
topit::Dot::Dot(p_t dd):
 IDraw(),
 d{dd}
{}
topit::p_t topit::Dot::begin() const {
  return d;
}
topit::p_t topit::Dot::next(p_t prev) const {
  if (prev != d) {
    throw std::logic_error("bad prev");
  }
  return d;
}
topit::Vline::Vline(p_t start, int length):
 IDraw(),
 s{start},
 l{length}
{}
topit::p_t topit::Vline::begin() const {
  return s;
}
topit::p_t topit::Vline::next(p_t prev) const {
    if (l == 0) return s;
    if (prev == s) {
        if (std::abs(l) > 1) {
          return {s.x, s.y + (l > 0 ? 1 : -1)};
        }
      return s;
    }
    if (prev.x != s.x) {
      throw std::logic_error("bad prev");
    }
    int steps = std::abs(l);
    int dir = l > 0 ? 1 : -1;
    int current_step = (prev.y - s.y) / dir;
    if (current_step < 0 || current_step >= steps) {
      throw std::logic_error("bad prev");
    }
    if (current_step == steps - 1) {
      return s;
    }
  return {prev.x, prev.y + dir};
}
topit::Hline::Hline(p_t start, int length):
 IDraw(),
 s{start},
 l{length}
{}
topit::p_t topit::Hline::begin() const {
  return s;
}
topit::p_t topit::Hline::next(p_t prev) const {
    if (l == 0) return s;
    if (prev == s) {
        if (std::abs(l) > 1) {
          return {s.x + (l > 0 ? 1 : -1), s.y};
        }
      return s;
    }
    if (prev.y != s.y) {
      throw std::logic_error("bad prev");
    }
    int steps = std::abs(l);
    int dir = l > 0 ? 1 : -1;
    int current_step = (prev.x - s.x) / dir;
    if (current_step < 0 || current_step >= steps) {
      throw std::logic_error("bad prev");
    }
    if (current_step == steps - 1) {
      return s;
    }
  return {prev.x + dir, prev.y};
}
size_t topit::rows(f_t fr) {
  return fr.bb.y - fr.aa.y + 1;
}
size_t topit::cols(f_t fr) {
  return (fr.bb.x - fr.aa.x + 1);
}
bool topit::operator==(p_t a, p_t b) {
  return a.x == b.x && a.y == b.y;
}
bool topit::operator!=(p_t a, p_t b) {
  return !(a == b);
}
//
