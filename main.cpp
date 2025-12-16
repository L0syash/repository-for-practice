#include <iostream>
#include <algorithm>
#include <stdexcept>
namespace top {

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

    struct IDraw {
        virtual ~IDraw() = default;
        virtual p_t begin() const = 0;
        virtual p_t next(p_t prev) const = 0;
    };
    p_t * extend (const p_t * pts, size_t s, p_t fill);
    void extend(p_t ** pts, size_t & s, p_t fill);
    struct Dot: IDraw {
    	explicit Dot(p_t dd);
    	p_t begin() const override;
    	p_t next(p_t prev) const override;
    	p_t d;
    };
    struct Vline : IDraw {
    	explicit Vline(p_t start, int lenght);
    	p_t begin() const override;
    	p_t next(p_t prev) const override;
    	p_t start_point;
    	int len;
    };
    struct Square : IDraw {
    	explicit Square(p_t start, int ed);
    	p_t begin() const override;
    	p_t next(p_t prev) const override;
    	p_t start_point;
    	int edge;
    };
    struct Rect : IDraw {
    	Rect(p_t pos, int w, int h);
    	Rect(p_t a, p_t b);
		p_t begin() const override;
		p_t next(p_t prev) const override;
    	f_t rect;
    };
    struct FRect : IDraw {
    	FRect(p_t pos, int w, int h);
    	FRect(p_t a, p_t b);
    	p_t begin() const override;
    	p_t next(p_t prev) const override;
    	f_t rect;
    };
    void append(const IDraw * sh, p_t ** ppts, size_t & s);
    f_t frame(const p_t * pts, size_t s);
    struct Layers;
    char * canvas(f_t fr, char fill);
    void paint(p_t p, char* cnv, f_t fr, char fill);
    void flush(std::ostream& os, const char * cnv, f_t fr);
    struct Layers {
    	Layers();
    	~Layers();
    	Layers(const Layers&) = delete;
    	Layers& operator=(const Layers&) = delete;
    	Layers(Layers&&) = delete;
    	Layers& operator=(Layers&&) = delete;
    	
    	void append(const IDraw & dr);
    	f_t frame() const {
    		return top::frame(pts_, points_);
    	}
    	size_t points() const{
    		return points_;
    	}
    	size_t layers() const{
    		return layers_;
    	}
    	p_t point(size_t i) const{
    		return pts_[i];
    	}
    	size_t start(size_t i) const{
    		return !i ? 0 : sizes_[i-1];
    	}
    	size_t end(size_t i) const{
    		return sizes_[i];
    	}
      	private:
    		size_t points_;
    		p_t * pts_;
    		size_t layers_;
    		size_t * sizes_;
    };
}

int main() {
	using namespace top;
	int err = 0;
    IDraw* shp[5] = {};
	Layers layers;
    try{
	    shp [0] = new Rect({-3, 4}, 4, 6);
	    shp [1] = new FRect({3, 4}, 6, 3);
	    shp [2] = new Dot({-5, -2});
	    shp [3] = new Vline({-2, 0}, 5);
	    shp [4] = new Square({1, 1}, 4);
		for (size_t i = 0; i < 5; ++i){
			layers.append(*(shp[i]));
		}
		f_t fr = layers.frame();
		char * cnv = canvas(fr, '.');
		const char * brush = "#0%$*";
		for (size_t k = 0; k < layers.layers(); ++k){
			size_t start = layers.start(k);
			size_t end = layers.end(k);
			for (size_t i = start; i < end; ++i){
				paint(layers.point(i), cnv, fr, brush[k]);
			}	
		}
	    flush (std::cout, cnv, fr);
	    delete[] cnv;
	} catch (...){
		std::cerr << "Error!\n";
		err = 1;
	}
    delete shp[1];
    delete shp[2];
    delete shp[0];
    delete shp[3];
    delete shp[4];
    return err;
}

void top::paint(p_t p, char * cnv, f_t fr, char fill)
{
	size_t dx = p.x - fr.aa.x;
	size_t dy = fr.bb.y - p.y;
	cnv[dy * cols(fr) + dx] = fill;
}

top::p_t * top::extend (const p_t * pts, size_t s, p_t fill)
{
	p_t * r = new p_t[s+1];
	for (size_t i = 0; i < s; i++){
		r[i] = pts[i];
	}
	r[s] = fill;
	return r;
}

void top::extend(p_t ** pts, size_t & s, p_t fill)
{
	p_t*r = extend(*pts, s, fill);
	delete[] *pts;
	++s;
	*pts = r;
}

void top::append(const IDraw * sh, p_t ** ppts, size_t & s)
{
	extend(ppts, s, sh -> begin());
	p_t b = sh -> begin();
	
	while (sh -> next(b) != sh -> begin()){
		b = sh -> next(b);
		extend(ppts, s, b);
	}
}

void top::flush(std::ostream& os, const char * cnv, f_t fr)
{
	for (size_t i = 0; i < rows(fr); ++i){
		for (size_t j = 0; j < cols(fr); ++j){
			os << cnv[i * cols(fr) + j];
		}
		os << '\n';
	}
}

char * top::canvas(f_t fr, char fill)
{
	size_t s = rows(fr) * cols(fr);
	char * c = new char [s];
	for (size_t i = 0; i < s; ++i){
		c[i] = fill;
	}
	return c;
}

top::f_t top::frame(const p_t * pts, size_t s)
{
	int minx = pts[0].x;
	int miny = pts[0].y;
	int maxx = pts[0].x;
	int maxy = pts[0].y;
	for (size_t i = 0; i < s; ++i){
		minx = std::min(minx, pts[i].x);
		miny = std::min(miny, pts[i].y);
		maxx = std::max(maxx, pts[i].x);
		maxy = std::max(maxy, pts[i].y);
	}
	p_t a{minx, miny};
	p_t b{maxx, maxy};
	return f_t{a, b};
}

top::Dot::Dot(p_t dd):
 IDraw(),
 d{dd}
{}

top::p_t top::Dot::begin() const {
	return d;
}

top::p_t top::Dot::next(p_t prev) const {
	if (prev != d){
		throw std::logic_error("bad prev");
	}
	return d;
}

bool top::operator==(p_t a, p_t b) {
    return a.x == b.x && a.y == b.y;
}

bool top::operator!=(p_t a, p_t b) {
    return !(a == b);
}

size_t top::rows(f_t fr)
{
	return fr.bb.y - fr.aa.y + 1;
}
size_t top::cols(f_t fr)
{
	return fr.bb.x - fr.aa.x + 1;
}

top::Vline::Vline(p_t start, int lenght):
  start_point(start),
  len(lenght) {
  	if (len <= 0){
  		throw std::logic_error("Lenght must be > 0");
  	}
}

top::p_t top::Vline::begin() const {
	return start_point;
}

top::p_t top::Vline::next(p_t prev) const {
    if (prev == start_point) {
        if (len > 1) {
            return {start_point.x, start_point.y + 1};
        }
        return start_point;
    }
    
    int current_index = prev.y - start_point.y;
    if (current_index < len - 1) {
        return {prev.x, prev.y + 1};
    }
    
    return start_point;
}

top::Square::Square(p_t start, int ed) :
	start_point(start),
	edge(ed)
{
	if (edge <= 0) {
		throw std::logic_error("Edge lenght must be > 0");
	}
}

top::p_t top::Square::begin() const {
    return start_point;
}
top::p_t top::Square::next(p_t prev) const {    
    if (edge == 1) {
        return start_point;
    }
    int dx = prev.x - start_point.x;
    int dy = prev.y - start_point.y;
    if (dy == 0 && dx < edge - 1) {
        return {prev.x + 1, prev.y};
    }
    if (dx == edge - 1 && dy == 0) {
        return {prev.x, prev.y + 1};
    }
    if (dx == edge - 1 && dy < edge - 1) {
        return {prev.x, prev.y + 1};
    }
    if (dx == edge - 1 && dy == edge - 1) {
        return {prev.x - 1, prev.y};
    }
    if (dy == edge - 1 && dx > 0) {
        return {prev.x - 1, prev.y};
    }
    if (dx == 0 && dy == edge - 1) {
        return {prev.x, prev.y - 1};
    }
    if (dx == 0 && dy > 1) {
        return {prev.x, prev.y - 1};
    }
    return start_point;
}

top::Rect::Rect(p_t pos, int w, int h) : rect{pos, {pos.x + w, pos.y + h}}, IDraw()
{
	if (w <= 0 or h <= 0){
		throw std::logic_error("bad rect");
	}
}

top::Rect::Rect(p_t a, p_t b): Rect(a, b.x - a.x, b.y - a.y)
{}

top::p_t top::Rect::begin() const {
	return rect.aa;
}

top::p_t top::Rect::next(p_t prev) const {
	if (prev.x == rect.aa.x and prev.y < rect.bb.y){
		return {prev.x, prev.y + 1};
	}
	if (prev.y == rect.bb.y && prev.x < rect.bb.x){
		return {prev.x + 1, prev.y};
	}
	if (prev.x == rect.bb.x && prev.y > rect.aa.y) {
		return {prev.x, prev.y - 1};
	}
	if (prev.y  == rect.aa.y && prev.x > rect.aa.x){
		return {prev.x - 1, prev.y};
	}
	throw std::logic_error("bad impl");
}

top::FRect::FRect(p_t pos, int w, int h) : rect{pos, {pos.x + w, pos.y + h}}, IDraw()
{
	if (w <= 0 or h <= 0){
		throw std::logic_error("bad filled rect");
	}
}

top::FRect::FRect(p_t a, p_t b) : FRect(a, b.x - a.x, b.y - a.y)
{}
top::p_t top::FRect::begin() const {
	return rect.aa;
}

top::p_t top::FRect::next(p_t prev) const
{
	if (prev.x < rect.bb.x){
		return {prev.x + 1, prev.y};
	}
	if (prev.x == rect.bb.x && prev.y < rect.bb.y){
		return {rect.aa.x, prev.y + 1};
	}
	if (prev == rect.bb){
		return rect.aa;
	}
	throw std::logic_error("bad filled impl");
}

top::Layers::Layers():
	points_{0},
	pts_(nullptr),
	layers_{0},
	sizes_{nullptr}
{}

top::Layers::~Layers()
{
	delete[] pts_;
	delete[] sizes_;
}

void top::Layers::append(const IDraw& dr){
	size_t * ext_sizes = new size_t[layers_ + 1];
	try {
		top::append(&dr, &pts_, points_);
	} catch (...) {
		delete [] ext_sizes;
		throw;
	}
	for (size_t i = 0; i < layers_; ++i){
		ext_sizes[i] = sizes_[i];
	}
	ext_sizes[layers_] = points_;
	delete[] sizes_;
	sizes_ = ext_sizes;
	++layers_;
}