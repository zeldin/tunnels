#ifndef TUNNELS_FMTBUILDER_H_
#define TUNNELS_FMTBUILDER_H_

namespace Tunnels {
namespace VDP { namespace FMTBuilder {

template <uint16 n> struct fmt_sequence
{
  static constexpr uint16 len = n;
  fmt_sequence<n-1> init;
  byte tail;
  void operator()(VDP::Screen &screen) const
  {
    VDP::FMT fmt{screen,
		 reinterpret_cast<const byte*>(this), len};
  }
};

template <> struct fmt_sequence<0>
{
  static constexpr uint16 len = 0;
  byte nothing[0];
};

namespace {

template <bool B, typename T> struct enable_if {};
template <typename T> struct enable_if<true,T>
{
  typedef T type;
};

template <typename T, typename U> struct fmt_cons
{
  static constexpr uint16 len = T::len + U::len;
  T car;
  U cdr;
};

typedef fmt_sequence<0> fmt_nil;

template <typename T>
struct fmt_rptb
{
  static constexpr uint16 len = T::len + 4;
  byte cnt;
  T body;
};

template <typename ... T> struct fmt_cons_resolve;

template <>
struct fmt_cons_resolve<>
{
  typedef fmt_nil type;
  static constexpr uint16 len = 0;
};

template <typename T, typename ... U>
struct fmt_cons_resolve<T, U...>
{
  typedef fmt_cons_resolve<U...> rest_type;
  typedef fmt_cons<T, typename rest_type::type> type;
};

template <uint16 n> constexpr fmt_sequence<n+1>
append1(fmt_sequence<n> l, byte x) { return fmt_sequence<n+1>{l, x}; }

template <uint16 n, uint16 k>
constexpr fmt_sequence<n> appendi(typename enable_if<(k==0),fmt_sequence<n> >::type l, const char *p)
{
  return l;
}

template <uint16 n, uint16 k>
constexpr fmt_sequence<n+k> appendi(typename enable_if<(k>0),fmt_sequence<n> >::type l, const char *p)
{
  return appendi<n+1,k-1>(append1(l, *p), p+1);
}

template <uint16 n>
constexpr fmt_sequence<n> append(fmt_sequence<n> x1, fmt_sequence<0> x2)
{
  return x1;
}

template <uint16 n1, uint16 n2>
constexpr fmt_sequence<n1+n2> append(fmt_sequence<n1> x1, fmt_sequence<n2> x2)
{
  return append1(append(x1, x2.init), x2.tail);
}

constexpr fmt_sequence<1> lift(byte b) {
  return append1(fmt_nil(), b);
}

constexpr fmt_sequence<1> op1(byte op, byte cnt)
{
  return lift((op<<5)|(cnt-1));
}

constexpr fmt_sequence<2> op1(byte op, byte cnt, byte v)
{
  return append1(op1(op, cnt), v);
}

template <uint16 n>
constexpr fmt_sequence<n>
op1(typename enable_if<(n>=2 && n<=33), byte>::type op, const char (&str)[n])
{
  return appendi<1,n-1>(op1(op, n-1), str);
}

constexpr fmt_sequence<2> op2(byte op, byte v)
{
  return append1(lift(op), v);
}

constexpr fmt_nil build_list() { return fmt_nil{}; }

template <typename T, typename ... U>
constexpr typename fmt_cons_resolve<T, U...>::type
build_list(T head, U ... rest) {
  return fmt_cons<T, typename fmt_cons_resolve<U...>::type>{head, build_list(rest...)};
}

constexpr fmt_sequence<0> build_fmt_sequence(uint16 offs, fmt_nil l)
{
  return fmt_nil();
}

template <uint16 n, typename T>
constexpr fmt_sequence<n+T::len>
build_fmt_sequence(uint16 offs, fmt_cons<fmt_sequence<n>,T> l)
{
  return append(l.car, build_fmt_sequence(offs+n, l.cdr));
}

template <typename U, typename T>
constexpr fmt_sequence<fmt_rptb<U>::len+T::len>
build_fmt_sequence(uint16 offs, fmt_cons<fmt_rptb<U>,T> l)
{
  return
    append(append1(append1(append1(append(op1(6, l.car.cnt),
					  build_fmt_sequence(offs+1,
							     l.car.body)),
				   0xfb), (offs+1)>>8), (offs+1)&0xff),
	   build_fmt_sequence(offs+fmt_rptb<U>::len, l.cdr));
}

}

template <uint16 n>
constexpr fmt_sequence<n> HSTR(const char (&str)[n]) { return op1(0, str); }
template <uint16 n>
constexpr fmt_sequence<n> VSTR(const char (&str)[n]) { return op1(1, str); }
constexpr fmt_sequence<2> HCHA(byte c, byte v) { return op1(2, c, v); }
constexpr fmt_sequence<2> VCHA(byte c, byte v) { return op1(3, c, v); }
constexpr fmt_sequence<1> RIGHT(byte c) { return op1(4, c); }
constexpr fmt_sequence<1> DOWN(byte c) { return op1(5, c); }
constexpr fmt_sequence<2> SCRO(byte v) { return op2(0xfc, v); }
constexpr fmt_sequence<2> ROW(byte v) { return op2(0xfe, v); }
constexpr fmt_sequence<2> COL(byte v) { return op2(0xff, v); }

template <typename ... U>
constexpr fmt_rptb<typename fmt_cons_resolve<U...>::type>
RPTB(byte cnt, U ... rest)
{
  return fmt_rptb<typename fmt_cons_resolve<U...>::type>{cnt, build_list(rest...)};
}

template <typename ... U>
constexpr fmt_sequence<fmt_cons_resolve<U...>::type::len+1>
fmt(U ... args)
{
  return append1(build_fmt_sequence(0, build_list(args...)), 0xfb);
}

}}
}

#endif // TUNNELS_FMTBUILDER_H_
