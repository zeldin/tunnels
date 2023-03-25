#ifndef TUNNELS_DATABASE_H_
#define TUNNELS_DATABASE_H_

namespace Tunnels {

namespace Utils { class StringSpan; }
namespace File { class ReadHandle; class WriteHandle; }

class Database {
public:
  virtual byte getMaxPlayers() const = 0;
  virtual byte getMaxFloors() const = 0;
  virtual byte getUnknown1CEB() const = 0;
  virtual byte getMinFloors() const = 0;
  virtual byte getUnknown1CF4() const = 0;
  virtual void setUnknown1CF4(byte x) = 0;
  virtual void setNumFloors(byte num) = 0;
  virtual void setNumPlayers(byte num) = 0;
  virtual void setDifficulty(byte dif) = 0;
  virtual byte getUnknown1D01() const = 0;
  virtual Utils::StringSpan getDictionaryWord(byte n) const = 0;
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
