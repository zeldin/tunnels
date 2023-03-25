#include "system.h"

#include "StdioFileBackend.h"
#include "Database.h"
#include "Utils.h"

#include <cstdio>

namespace Tunnels {

class StdioFileBackend::FileHandle {
private:
  FILE *f;
public:
  FileHandle(FILE *f_) : f(f_) {}
  FILE *operator*() const { return f; }
  bool operator!() const { return !f; }
};

class StdioFileBackend::ReadHandle : public File::ReadHandle {
private:
  FILE *f;
  StdioFileBackend &backend;
public:
  ReadHandle(FileHandle fh, StdioFileBackend &backend_)
    : f(*fh), backend(backend_) { }
  virtual unsigned readBytes(byte *ptr, unsigned cnt) override;
};

class StdioFileBackend::WriteHandle : public File::WriteHandle {
private:
  FILE *f;
  StdioFileBackend &backend;
public:
  WriteHandle(FileHandle fh, StdioFileBackend &backend_)
    : f(*fh), backend(backend_) { }
  virtual unsigned writeBytes(const byte *ptr, unsigned cnt) override;
};

unsigned StdioFileBackend::ReadHandle::readBytes(byte *ptr, unsigned cnt)
{
  size_t n = ::fread(ptr, sizeof(byte), cnt, f);
  if (::ferror(f) || n > cnt) {
    backend.error = File::ERROR_FILE_ERROR;
    return 0;
  }
  else if (n < cnt)
    backend.error = File::ERROR_PAST_EOF;
  return n;
}

unsigned StdioFileBackend::WriteHandle::writeBytes(const byte *ptr, unsigned cnt)
{
  size_t n = ::fwrite(ptr, sizeof(byte), cnt, f);
  if (::ferror(f) || n > cnt) {
    backend.error = File::ERROR_FILE_ERROR;
    return 0;
  }
  else if (n < cnt)
    backend.error = File::ERROR_PAST_EOF;
  return n;
}

StdioFileBackend::FileHandle StdioFileBackend::fopen(Utils::StringSpan filename, const char *mode)
{
  char namebuf[32];
  filename.removePrefix("DSK1.");
  filename.store(namebuf);
  FILE *f = ::fopen(namebuf, mode);
  if (!f) {
    error = File::ERROR_FILE_ERROR;
  }
  return f;
}

Database *StdioFileBackend::load(Utils::StringSpan filename,
				 DatabaseFactory &factory)
{
  error = File::ERROR_OK;
  FileHandle f = fopen(filename, "rb");
  if (!f)
    return nullptr;
  ReadHandle rh{f, *this};
  return factory.load(rh);
}

bool StdioFileBackend::save(const Database &database,
			    Utils::StringSpan filename,
			    DatabaseFactory &factory)
{
  error = File::ERROR_OK;
  FileHandle f = fopen(filename, "wb");
  if (!f)
    return false;
  WriteHandle wh{f, *this};
  return factory.save(wh, database);
}

}
