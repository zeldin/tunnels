#ifndef TUNNELS_DATABASE_H_
#define TUNNELS_DATABASE_H_

namespace Tunnels {

namespace Utils { class StringSpan; }
namespace File { class ReadHandle; class WriteHandle; }

class Database {
public:
  virtual Utils::StringSpan getDescription() const = 0;
  virtual Utils::StringSpan getPatternTable() const = 0;
  virtual void clearPlayerSheet(unsigned n) = 0;
  virtual Utils::StringSpan getPlayerName(unsigned n) const = 0;
  virtual void setPlayerName(unsigned n, Utils::StringSpan name) = 0;
  virtual byte getPlayerClass(unsigned n) const = 0;
  virtual void setPlayerClass(unsigned n, unsigned c) = 0;
  virtual Utils::StringSpan getClassName(unsigned n) const = 0;
  virtual Utils::StringSpan getClassPatternTable(unsigned n) const = 0;
  virtual byte getMaxPlayers() const = 0;
  virtual byte getNumClassChoices() const = 0;
  virtual byte getMaxFloors() const = 0;
  virtual byte getNumConfiguredPlayers() const = 0;
  virtual void setNumConfiguredPlayers(byte num) = 0;
  virtual byte getUnknown1CEB() const = 0;
  virtual void setUnknown1CEB(byte x) = 0;
  virtual byte getMinFloors() const = 0;
  virtual byte getUnknown1CF4() const = 0;
  virtual void setUnknown1CF4(byte x) = 0;
  virtual void setNumFloors(byte num) = 0;
  virtual byte getNumPlayers() const = 0;
  virtual void setNumPlayers(byte num) = 0;
  virtual void setDifficulty(byte dif) = 0;
  virtual byte getUnknown1D01() const = 0;
  virtual byte getPlayerColor(unsigned n) const = 0;
  virtual void setPlayerColor(unsigned n, unsigned c) = 0;
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
