#ifndef TUNNELS_UTILS_H_
#define TUNNELS_UTILS_H_

namespace Tunnels { namespace Utils {

class StringSpan {
private:
  const byte *p;
  unsigned l;

public:
  StringSpan(const byte *ptr, unsigned len) : p(ptr), l(len) {}
  template<unsigned n> StringSpan(const byte (&str)[n]) : p(str), l(n) {}
  template<unsigned n> StringSpan(const char (&str)[n]) :
    p(reinterpret_cast<const byte *>(str)), l(n-1) {}

  const byte *ptr() const { return p; }
  unsigned len() const { return l; }
  unsigned center() {
    unsigned offs = 0;
    while (l>0 && p[l-1] == ' ') if ((--l & 1))	offs++;
    return offs;
  }
  void uncenter() {
    while (l>0 && *p == ' ') { p++; --l; }
  }
};

}}

#endif // TUNNELS_UTILS_H_
