#ifndef TUNNELS_STDIOFILEBACKEND_H_
#define TUNNELS_STDIOFILEBACKEND_H_

#include "File.h"

namespace Tunnels {

class StdioFileBackend : public File::Backend {
private:
  File::Error error;
  class FileHandle;
  class ReadHandle;
  class WriteHandle;

public:
  StdioFileBackend() : error(File::ERROR_OK) { }
  virtual Database *load(Utils::StringSpan filename,
			 DatabaseFactory &factory) override;
  virtual bool save(const Database &database, Utils::StringSpan filename,
		    DatabaseFactory &factory) override;
  virtual File::Error getError() override { return error; }
private:
  FileHandle fopen(Utils::StringSpan filename, const char *mode);
};

}

#endif // TUNNELS_STDIOFILEBACKEND_H_
