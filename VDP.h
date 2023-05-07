#ifndef TUNNELS_VDP_H_
#define TUNNELS_VDP_H_

namespace Tunnels {

namespace Utils { class StringSpan; }

namespace VDP {

namespace FMTBuilder { template <uint16 n> struct fmt_sequence; }

constexpr unsigned COLUMNS = 32;
constexpr unsigned ROWS = 24;
constexpr unsigned PATTERN_W = 8;
constexpr unsigned PATTERN_H = 8;
constexpr unsigned PIXELS_W = COLUMNS * PATTERN_W;
constexpr unsigned PIXELS_H = ROWS * PATTERN_W;
constexpr unsigned PATTERNS = 256;
constexpr unsigned COLORS = 16;
constexpr unsigned COLOR_TABLE_ENTRIES = PATTERNS / 8;

struct RGB {
  byte r;
  byte g;
  byte b;
  constexpr RGB(byte r_, byte g_, byte b_) : r(r_), g(g_), b(b_) {}
};

template<typename T> class Palette
{
  template <typename U> friend class Palette;
private:
  template <unsigned i, typename U> struct Convert {
    typedef const U (&Src)[COLORS];
    Src src;
    constexpr Convert(Src src_) : src(src_) { }
    constexpr T next() { return src[COLORS-i]; }
  };

  T values[COLORS];

public:
  template <typename ... U> constexpr Palette(U ... vals) : values{vals...} { }
  template <typename U> constexpr Palette(const U (&src)[COLORS]) : Palette{Convert<COLORS,U>(src)} { }
  template <typename U> constexpr Palette(const Palette<U> &src) : Palette{src.values} { }

  T& operator[](unsigned idx) { return values[idx]; }
  const T& operator[](unsigned idx) const { return values[idx]; }

private:
  template <typename U, typename ... W>
  constexpr Palette(Convert<0,U> conv, W ... vals) : Palette{vals...} { }
  template <unsigned i, typename U, typename ... W>
  constexpr Palette(Convert<i,U> conv, W ... vals) :
    Palette{Convert<i-1,U>(conv.src), vals..., conv.next()} { }
};

enum {
  TRANSPARENT,
  BLACK,
  MEDIUM_GREEN,
  LIGHT_GREEN,
  DARK_BLUE,
  LIGHT_BLUE,
  DARK_RED,
  CYAN,
  MEDIUM_RED,
  LIGHT_RED,
  DARK_YELLOW,
  LIGHT_YELLOW,
  DARK_GREEN,
  MAGENTA,
  GRAY,
  WHITE
};

constexpr Palette<RGB> palette{
  RGB{ 0, 0, 0, },
  RGB{ 0, 0, 0, },
  RGB{ 33, 200, 66, },
  RGB{ 94, 220, 120, },
  RGB{ 84, 85, 237, },
  RGB{ 125, 118, 252, },
  RGB{ 212, 82, 77, },
  RGB{ 66, 235, 245, },
  RGB{ 252, 85, 84, },
  RGB{ 255, 121, 120, },
  RGB{ 212, 193, 84, },
  RGB{ 230, 206, 128, },
  RGB{ 33, 176, 59, },
  RGB{ 201, 91, 186, },
  RGB{ 204, 204, 204, },
  RGB{ 255, 255, 255 },
};

class Renderer {
  friend class ScopedRender;
protected:
  virtual void drawPattern(unsigned row, unsigned col,
			   const byte *pat, byte colors) = 0;
  virtual void drawSprite(byte y, byte x, const byte *pat, byte color) = 0;
  virtual void removeSprite() = 0;
  virtual void drawBorder(byte color) { };
  virtual void endRender() { };
};

class BytemapRenderer : public Renderer {
private:
  byte *data;
  int pitch;
  unsigned border_w;
  unsigned border_h;
  unsigned sprite_su_y, sprite_su_x, sprite_su_h, sprite_su_w;
  byte sprite_save_under[2*PATTERN_H][2*PATTERN_W];
protected:
  BytemapRenderer(byte *data_ = nullptr, int pitch_ = 0,
		  unsigned border_w_ = 0, unsigned border_h_ = 0) :
    data(data_), pitch(pitch_), border_w(border_w_), border_h(border_h_),
    sprite_su_h(0), sprite_su_w(0) { }
  void set(byte *data_ = nullptr, int pitch_ = 0, unsigned border_w_ = 0,
	   unsigned border_h_ = 0)
  { data = data_; pitch = pitch_; border_w = border_w_; border_h = border_h_; }
  virtual void drawPattern(unsigned row, unsigned col, const byte *pat,
			   byte colors) override;
  virtual void drawSprite(byte y, byte x, const byte *pat, byte color) override;
  virtual void removeSprite() override;
  virtual void drawBorder(byte color) override;
};

class ScopedRender {
private:
  Renderer *target;
public:
  ScopedRender(Renderer *target_) : target(target_) { }
  ~ScopedRender() { target->endRender(); }
  void drawPattern(unsigned row, unsigned col, const byte *pat, byte colors) {
    target->drawPattern(row, col, pat, colors);
  }
  void drawSprite(byte y, byte x, const byte *pat, byte color) {
    target->drawSprite(y, x, pat, color);
  }
  void removeSprite() {
    target->removeSprite();
  }
  void drawBorder(byte color) {
    target->drawBorder(color);
  }
  ScopedRender & operator=(const ScopedRender&) = delete;
  ScopedRender(const ScopedRender&) = delete;
  ScopedRender() = delete;
  ScopedRender(ScopedRender&& other) : target(other.target)
  { other.target = nullptr; }
};

class Backend {
 public:
  virtual ScopedRender startRender() = 0;
  virtual ~Backend() { }
};

class FMT;

class Screen
{
 private:
  byte pattern_generator[PATTERNS][PATTERN_H];
  byte color_table[COLOR_TABLE_ENTRIES];
  byte name_table[ROWS][COLUMNS];
  byte sprite_pattern_generator[PATTERNS][PATTERN_H];
  bool pattern_generator_dirty[PATTERNS];
  bool name_table_dirty[ROWS][COLUMNS];
  bool any_pattern_generator_dirty;
  bool any_name_table_dirty;
  bool border_dirty;
  bool sprite_dirty;
  bool sprite_displayed;
  bool sprite_active;

  byte sprite_y, sprite_x, sprite_name, sprite_color;
  byte background;
  byte xpt;
  byte ypt;
  
 private:
  void loadColorTable(unsigned idx, const byte *p, unsigned n);
  void loadPatterns7(unsigned idx, const byte *p, unsigned n);
  void loadPatterns8(unsigned idx, const byte *p, unsigned n);
  void loadSpritePatterns(unsigned idx, const byte *p, unsigned n);
  void loadPatterns(unsigned idx, const byte (&a)[7], unsigned n) {
    loadPatterns7(idx, &a[0], n);
  }
  void loadPatterns(unsigned idx, const byte (&a)[8], unsigned n) {
    loadPatterns8(idx, &a[0], n);
  }
  
 public:
  Screen();

  void loadPatterns(unsigned idx, Utils::StringSpan p);
  void loadSpritePatterns(unsigned idx, Utils::StringSpan p);
  template <unsigned n, unsigned m>
  void loadPatterns(unsigned idx, const byte (&a)[n][m]) {
    loadPatterns(idx, a[0], n);
  }
  template <unsigned n>
  void loadSpritePatterns(unsigned idx, const byte (&a)[n][8]) {
    loadSpritePatterns(idx, a[0], n);
  }
  void loadColorTable(unsigned idx, Utils::StringSpan p);
  template <unsigned n> void loadColorTable(unsigned idx, const byte (&a)[n])
  {
    loadColorTable(idx, &a[0], n);
  }

  void loadRomFont();
  byte gchar(unsigned row, unsigned col)
  { return row < ROWS && col < COLUMNS? name_table[row][col] : 0; }
  void hchar(unsigned row, unsigned col, byte name, unsigned cnt = 1);
  void vchar(unsigned row, unsigned col, byte name, unsigned cnt = 1);
  void hstr(unsigned row, unsigned col, Utils::StringSpan span,
	    byte offset = 0);
  void vstr(unsigned row, unsigned col, Utils::StringSpan span,
	    byte offset = 0);
  Utils::StringSpan gstr(unsigned row, unsigned col, unsigned len);
  void all(byte name);
  void setBackground(byte color);
  void setSprite(byte y, byte x, byte name, byte color);
  void clearSprite();
  void refresh(Backend &backend);

  byte getXpt() const { return xpt; }
  byte getYpt() const { return ypt; }
  void setXpt(byte x) { if (x < COLUMNS) xpt = x; }
  void setYpt(byte y) { if (y < ROWS) ypt = y; }

  template <unsigned n> void fmt(const byte (&a)[n]) { FMT fmt{*this, a}; }
};

class FMT {
  template <uint16 n> friend class FMTBuilder::fmt_sequence;
 private:
  Screen &screen;
  const byte *data;
  unsigned length;
  byte offset;
 public:
  template <unsigned n> FMT(Screen &screen, const byte (&a)[n]) :
  FMT(screen, &a[0], n) {}
 private:
  FMT(Screen &screen, const byte *data, unsigned length);
  unsigned run(unsigned offs);
};
    
}}

#endif // TUNNELS_VDP_H_
