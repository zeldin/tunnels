#ifndef TUNNELS_UTILS_H_
#define TUNNELS_UTILS_H_

namespace Tunnels { namespace Utils {

class StringSpan {
private:
  const byte *p;
  unsigned l;
  
  bool hasPrefix(const byte *pfx, unsigned len)
  {
    if (len > l)
      return false;
    while(len-- > 0)
      if (p[len] != pfx[len])
	return false;
    return true;
  }
public:
  StringSpan() : p(nullptr), l(0) {}
  StringSpan(const byte *ptr, unsigned len) : p(ptr), l(len) {}
  template<unsigned n> StringSpan(const byte (&str)[n]) : p(str), l(n) {}
  template<unsigned n> StringSpan(const char (&str)[n]) :
    p(reinterpret_cast<const byte *>(str)), l(n-1) {}

  const byte *ptr() const { return p; }
  unsigned len() const { return l; }
  unsigned prefixLen() const {
    for (unsigned i=0; i<l; i++)
      if (p[i] == ' ')
	return i;
    return l;
  }
  unsigned center() {
    unsigned offs = 0;
    while (l>0 && p[l-1] == ' ') if (!(--l & 1)) offs++;
    return offs;
  }
  void uncenter() {
    while (l>0 && *p == ' ') { p++; --l; }
  }
  void subspan(unsigned start, unsigned len) {
    if (start < l) {
      p += start;
      l -= start;
      if (l > len)
	l = len;
    } else
      l = 0;
  }
  operator bool() const { return l != 0; }
  const byte& operator[](unsigned i) const
  {
    static const byte nul = 0;
    return (i < l? p[i] : nul);
  }
  template <unsigned n> void store(byte (&dst)[n], unsigned offset=0)
  {
    unsigned len = (offset >= n? 0 : (n-offset>l? l : n-offset));
    for (unsigned i=0; i<len; i++)
      dst[i+offset] = p[i];
  }
  template <unsigned n> void store(char (&dst)[n])
  {
    unsigned len = (n>l? l : n-1);
    for (unsigned i=0; i<len; i++)
      dst[i] = p[i];
    dst[len] = 0;
  }
  template <unsigned n> bool hasPrefix(const byte (&pfx)[n])
  { return hasPrefix(&pfx[0], n); }
  template <unsigned n> bool hasPrefix(const char (&pfx)[n])
  { return hasPrefix(reinterpret_cast<const byte *>(&pfx[0]), n-1); }
  template <unsigned n> void removePrefix(const byte (&pfx)[n])
  { if (hasPrefix(pfx)) { p+=n; l-=n; } }
  template <unsigned n> void removePrefix(const char (&pfx)[n])
  { if (hasPrefix(pfx)) { p+=n-1; l-=n-1; } }
};

template <unsigned n, typename T> void clearArray(T (&dst)[n], T value=0)
{
  for (unsigned i=0; i<n; i++)
    dst[i] = value;
}

template <unsigned n, typename T> void clearArray(T (&dst)[n], unsigned offs, unsigned count, T value=0)
{
  if (offs >= n)
    return;
  if (count > n-offs)
    count = n-offs;
  for (unsigned i=0; i<count; i++)
    dst[offs+i] = value;
}

}}

#endif // TUNNELS_UTILS_H_
