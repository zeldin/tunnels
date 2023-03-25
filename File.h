#ifndef TUNNELS_FILE_H_
#define TUNNELS_FILE_H_

namespace Tunnels {

namespace Utils { class StringSpan; }

class Database;
class DatabaseFactory;

namespace File {

enum Error {
  ERROR_OK = 0,
  ERROR_WRITE_PROTECTION = 1,
  ERROR_BAD_ATTRIBUTE = 2,
  ERROR_ILLEGAL_OPCODE = 3,
  ERROR_MEMORY_FULL = 4,
  ERROR_PAST_EOF = 5,
  ERROR_DEVICE_ERROR = 6,
  ERROR_FILE_ERROR = 7
};

class ReadHandle {
protected:
  virtual unsigned readBytes(byte *ptr, unsigned cnt) = 0;
public:
  template<typename T> unsigned readBytes(T &buffer)
  { return readBytes(reinterpret_cast<byte *>(&buffer), sizeof(T)); }
  virtual ~ReadHandle() { }
};

class WriteHandle {
protected:
  virtual unsigned writeBytes(const byte *ptr, unsigned cnt) = 0;
public:
  template<typename T> unsigned writeBytes(const T &buffer)
  { return writeBytes(reinterpret_cast<const byte *>(&buffer), sizeof(T)); }
  virtual ~WriteHandle() { }
};

class Backend {
public:
  virtual Database *load(Utils::StringSpan filename,
			 DatabaseFactory &factory) = 0;
  virtual bool save(const Database &database, Utils::StringSpan filename,
		    DatabaseFactory &factory) = 0;
  virtual Error getError() = 0;
  virtual ~Backend() { }
};

}}

#endif // TUNNELS_FILE_H_
