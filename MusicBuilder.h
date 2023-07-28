#ifndef TUNNELS_MUSICBUILDER_H_
#define TUNNELS_MUSICBUILDER_H_

namespace Tunnels {
namespace DCSG { namespace MusicBuilder {

template <typename T, typename U> struct music_meta_sequence
{
  static constexpr uint16 len = T::len + U::len;
  T init;
  U tail;
  void play(DCSG::Music &music, bool post=false, unsigned offs=0) const
  {
    music.play(reinterpret_cast<const byte*>(this), len, offs, post);
  }
};

namespace {

template <bool B, typename T> struct enable_if {};
template <typename T> struct enable_if<true,T>
{
  typedef T type;
};

template <uint16 n> struct music_sequence
{
  static constexpr uint16 len = n;
  music_sequence<n-1> init;
  byte tail;
};

template <> struct music_sequence<0>
{
  static constexpr uint16 len = 0;
  typedef music_sequence<0> metatype;
  byte nothing[0];
};

typedef music_sequence<0> music_nil;

template <typename T> struct music_index_list {
  byte nothing[0];
  template <int n> constexpr uint16 lookup() const { return T::template lookup_index<n>(0); }
};

template <typename T, typename U> struct index_lookup {
  template <int i> static constexpr uint16 lookup(uint16 base)
  {
    return U::template lookup_index<i>(T::len+base);
  }
};

template <typename T, typename U> struct meta_resolve {};

template <typename T, typename U> struct music_cons
{
  static constexpr uint16 len = T::len + U::len;
  typedef typename meta_resolve<T, U>::type metatype;
  T car;
  U cdr;
  template <int n> static constexpr uint16 lookup_index(uint16 base) {
    return index_lookup<T, U>::template lookup<n>(base);
  }
};

template <int n> struct music_jump {
  static constexpr uint16 len = 3;
};

template <int n> struct music_index {
  static constexpr uint16 len = 0;
};

template <int n, typename U> struct index_lookup<music_index<n>,U> {
  template <int i> static constexpr uint16 lookup(typename enable_if<i==n, uint16>::type base)
  {
    return base;
  }
  template <int i> static constexpr uint16 lookup(typename enable_if<i!=n, uint16>::type base)
  {
    return U::template lookup_index<i>(base);
  }
};

template <uint16 n, typename U> struct meta_resolve<music_sequence<n>,U> {
  typedef music_meta_sequence<music_sequence<n>, typename U::metatype> type;
};
template <int n, typename U> struct meta_resolve<music_jump<n>,U> {
  typedef music_meta_sequence<music_sequence<3>, typename U::metatype> type;
};
template <int n, typename U> struct meta_resolve<music_index<n>,U> {
  typedef typename U::metatype type;
};

template <typename ... T> struct music_cons_resolve;

template <>
struct music_cons_resolve<>
{
  typedef music_nil type;
};

template <typename T, typename ... U>
struct music_cons_resolve<T, U...>
{
  typedef music_cons_resolve<U...> rest_type;
  typedef music_cons<T, typename rest_type::type> type;
};

template <typename T> struct music_beat_resolve_one
{
  static constexpr uint16 len = 1;
};

template <uint16 n> struct music_beat_resolve_one<music_sequence<n> >
{
  static constexpr uint16 len = n;
};

template <typename ... T> struct music_beat_resolve;

template <>
struct music_beat_resolve<>
{
  typedef music_nil type;
};

template <typename T, typename ... U>
struct music_beat_resolve<T, U...>
{
  typedef music_beat_resolve_one<T> first_type;
  typedef music_beat_resolve<U...> rest_type;
  typedef music_sequence<first_type::len + rest_type::type::len> type;
};

template <unsigned n> struct index_sequence
{
  uint16 offs;
  index_sequence<n-1> rest;
};

template <> struct index_sequence<1>
{
  uint16 offs;
};

template <int lo, int hi, int def, typename T> struct offset_array {
  index_sequence<hi-lo+1> index;
  music_index_list<T> list;
  uint16 lookup(int n) const {
    return (n >= lo && n <= hi? reinterpret_cast<const uint16 *>(&index)[n-lo] :
	    list.template lookup<def>());
  }
};

template <typename T> struct indexed_music_sequence
{
  typedef byte as_bytes_type[T::metatype::len];
  typename T::metatype music;
  music_index_list<T> index;
  template <int i> void play(DCSG::Music &m, bool post=false) const
  {
    music.play(m, post, index.template lookup<i>());
  }
  template <int lo, int hi, int def, typename I>
  void play(DCSG::Music &m, const offset_array<lo, hi, def, I> &index,
	    int i, bool post=false) const
  {
    music.play(m, post, index.lookup(i));
  }
  void as_bytes(as_bytes_type &dst) const {
    const as_bytes_type &src(*reinterpret_cast<const as_bytes_type*>(this));
    for (unsigned i=0; i<T::metatype::len; i++)
      dst[i] = src[i];
  }
};

template <uint16 n> constexpr music_sequence<n+1>
append1(music_sequence<n> l, byte x) { return music_sequence<n+1>{l, x}; }

template <uint16 n, uint16 k>
constexpr music_sequence<n> appendi(typename enable_if<(k==0),music_sequence<n> >::type l, const char *p)
{
  return l;
}

template <uint16 n, uint16 k>
constexpr music_sequence<n+k> appendi(typename enable_if<(k>0),music_sequence<n> >::type l, const char *p)
{
  return appendi<n+1,k-1>(append1(l, *p), p+1);
}

template <uint16 n>
constexpr music_sequence<n> append(music_sequence<n> x1, music_sequence<0> x2)
{
  return x1;
}

template <uint16 n1, uint16 n2>
constexpr music_sequence<n1+n2> append(music_sequence<n1> x1, music_sequence<n2> x2)
{
  return append1(append(x1, x2.init), x2.tail);
}

constexpr music_sequence<1> lift(byte b) {
  return append1(music_nil(), b);
}

constexpr music_sequence<2> build_offs(uint16 offs) {
  return append1(lift(offs>>8), offs&0xff);
}

constexpr music_nil build_list() { return music_nil{}; }

template <typename T, typename ... U>
constexpr typename music_cons_resolve<T, U...>::type
build_list(T head, U ... rest) {
  return music_cons<T, typename music_cons_resolve<U...>::type>{head, build_list(rest...)};
}

template <typename I> constexpr music_sequence<0> build_music_sequence(music_index_list<I> index, music_nil l)
{
  return music_nil();
}

template <typename I, uint16 n, typename T>
constexpr music_meta_sequence<music_sequence<n>,typename T::metatype>
build_music_sequence(music_index_list<I> index, music_cons<music_sequence<n>,T> l)
{
  return music_meta_sequence<music_sequence<n>,typename T::metatype>
    {l.car, build_music_sequence(index, l.cdr)};
}

template <typename I, int n, typename T>
constexpr music_meta_sequence<music_sequence<3>,typename T::metatype>
build_music_sequence(music_index_list<I> index, music_cons<music_jump<n>,T> l)
{
  return music_meta_sequence<music_sequence<3>,typename T::metatype>
    {append(lift(0), build_offs(index.template lookup<n>())),
     build_music_sequence(index, l.cdr)};
}

template <typename I, int n, typename T>
constexpr typename T::metatype
build_music_sequence(music_index_list<I> index, music_cons<music_index<n>,T> l)
{
  return build_music_sequence(index, l.cdr);
}

constexpr music_sequence<0> build_beat()
{
  return music_nil();
}

template <typename ... U>
constexpr music_sequence<music_beat_resolve<byte, U...>::type::len>
build_beat(byte n, U ... rest) {
  return append(lift(n), build_beat(rest...));
}

template <uint16 n, typename ... U>
constexpr music_sequence<music_beat_resolve<music_sequence<n>, U...>::type::len>
build_beat(music_sequence<n> s, U ... rest) {
  return append(s, build_beat(rest...));
}

template <typename T>
constexpr indexed_music_sequence<T> build_indexed_music_sequence(T l)
{
  return indexed_music_sequence<T>
    {build_music_sequence(music_index_list<T>{}, l), music_index_list<T>{}};
}

template <int lo, int hi, typename T>
constexpr index_sequence<hi-lo+1>
build_index_array(typename enable_if<lo == hi,music_index_list<T>>::type index)
{
  return index_sequence<1>{index.template lookup<lo>()};
}

template <int lo, int hi, typename T>
constexpr index_sequence<hi-lo+1>
build_index_array(typename enable_if<lo != hi,music_index_list<T>>::type index)
{
  return index_sequence<hi-lo+1>{index.template lookup<lo>(),
    build_index_array<lo+1,hi,T>(index)};
}

}

template <typename ... U>
constexpr music_sequence<music_beat_resolve<U...>::type::len+2>
BEAT(unsigned time, U ... args)
{
  return append1(build_beat(byte(music_beat_resolve<U...>::type::len), args...), time);
}

template <int n>
constexpr music_jump<n> JUMP() { return music_jump<n>{}; }

template <int n>
constexpr music_index<n> INDEX() { return music_index<n>{}; }

template <typename ... U>
constexpr indexed_music_sequence<typename music_cons_resolve<U...>::type>
music(U ... args)
{
  return build_indexed_music_sequence(build_list(args...));
}

template <int lo, int hi, int def, typename T>
constexpr offset_array<lo, hi, def, T> index_array(music_index_list<T> index)
{
  return offset_array<lo, hi, def, T>{build_index_array<lo,hi,T>(index), index};
}

}}
}


#endif // TUNNELS_MUSICBUILDER_H_
