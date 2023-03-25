#ifndef TUNNELS_DATABASE_H_
#define TUNNELS_DATABASE_H_

namespace Tunnels {

namespace Utils { class StringSpan; }
namespace File { class ReadHandle; class WriteHandle; }

class Database {
public:
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name) = 0;
  virtual ~Database() { }
};

class DatabaseFactory {
public:
  virtual Database *load(File::ReadHandle &handle) = 0;
  virtual bool save(File::WriteHandle &handle, const Database &db) = 0;
  virtual ~DatabaseFactory() { }
};

}

#endif // TUNNELS_DATABASE_H_
