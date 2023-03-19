#ifndef TUNNELS_CLASSIC_SEQUENCEPACK_H_
#define TUNNELS_CLASSIC_SEQUENCEPACK_H_

namespace Tunnels {

namespace Utils { class StringSpan; }

namespace Classic {

namespace SequencePack {

template <uint16 n, typename T> struct ArraySequence
{
  static constexpr uint16 len = n;
  typedef T type;
  T data[n];
  constexpr const T *ptr() const { return data; }
};

template <uint16 n, typename T> struct UniformSequence
{
  static constexpr uint16 len = n;
  typedef T type;
  T head;
  UniformSequence<n-1, T> tail;
  constexpr const T *ptr() const { return &head; }
};

template <typename T> struct UniformSequence<0, T>
{
  static constexpr uint16 len = 0;
  byte nothing[0];
};

template <uint16 n, typename H, typename T> struct Sequence
{
  static constexpr uint16 len = n;
  typedef H headType;
  H head;
  T tail;
};

template <> struct Sequence<0, void, void>
{
  static constexpr uint16 len = 0;
  byte nothing[0];
};

typedef Sequence<0, void, void> EmptySequence;

template <typename ... T> struct SequenceType;

template <> struct SequenceType<>
{
  static constexpr uint16 len = 0;
  typedef EmptySequence type;
};

template <typename T, typename ... U> struct SequenceType<T, U...>
{
  typedef typename SequenceType<U...>::type tailType;
  static constexpr uint16 len = 1 + tailType::len;
  typedef Sequence<len, T, tailType> type;
};

template <typename ... T> struct StringSequenceType;

template <> struct StringSequenceType<>
{
  static constexpr uint16 len = 0;
  typedef EmptySequence type;
};

template <int n, typename ... U> struct StringSequenceType<const char[n], U...>
{
  typedef typename StringSequenceType<U...>::type tailType;
  static constexpr uint16 len = 1 + tailType::len;
  typedef Sequence<len, UniformSequence<n-1, byte>, tailType> type;
};

constexpr EmptySequence sequence() { return EmptySequence{}; }

template <typename T, typename ... U>
constexpr typename SequenceType<T, U...>::type sequence(T head, U ... rest) {
  return typename SequenceType<T, U...>::type{head, sequence(rest...)};
}

template <int n> constexpr UniformSequence<n, byte> buildString(const char *p)
{
  return UniformSequence<n, byte>{*p, buildString<n-1>(p+1)};
}

template <> constexpr UniformSequence<0, byte> buildString<0>(const char *p)
{
  return UniformSequence<0, byte>{};
}

constexpr EmptySequence stringSequence() { return EmptySequence{}; }

template <int n, typename ... U>
constexpr typename StringSequenceType<const char[n], U ...>::type
stringSequence(const char (&head)[n], U& ... rest) {
  return typename StringSequenceType<const char[n], U ...>::type{
    buildString<n-1>(head), stringSequence(rest...)};
}

constexpr UniformSequence<1, uint16> indexSequence(EmptySequence s, uint16 offs=0)
{
  return UniformSequence<1, uint16>{offs, UniformSequence<0, uint16>{}};
}

template <uint16 n, typename T, typename U> constexpr
UniformSequence<n+1, uint16> indexSequence(Sequence<n, T, U> s, uint16 offs=0) {
  return UniformSequence<n+1, uint16>{offs, indexSequence(s.tail, offs+T::len)};
}

template <typename T> class SpanType { };
template <> class SpanType<byte> { typedef Utils::StringSpan type; };

template <typename S> class IndexedSequence
{
private:
  S seq;
  UniformSequence<S::len+1, uint16> idx;
public:
  constexpr IndexedSequence(S seq) : seq(seq), idx(indexSequence(seq)) {}
  constexpr const typename S::headType::type *entry(uint16 n) const
  { return n < S::len? seq.head.ptr()+idx.ptr()[n] : nullptr; }
  constexpr uint16 len(uint16 n) const
  { return n < S::len? idx.ptr()[n+1] - idx.ptr()[n] : 0; }
  template<typename T=typename SpanType<typename S::headType::type>::type>
  constexpr T span(uint16 n) const
  { return n < S::len?
      T{seq.head.ptr()+idx.ptr()[n],
	unsigned(idx.ptr()[n+1]) - unsigned(idx.ptr()[n])} : T{}; }
};

template <typename S> constexpr IndexedSequence<S> index(S seq)
{
  return IndexedSequence<S>(seq);
}


}
}
}

#endif // TUNNELS_CLASSIC_SEQUENCEPACK_H_
