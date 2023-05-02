#ifndef TUNNELS_DATABASE_H_
#define TUNNELS_DATABASE_H_

#include "GameTypes.h"

namespace Tunnels {

namespace Utils { class StringSpan; }
namespace File { class ReadHandle; class WriteHandle; }

class Database {
public:
  virtual Utils::StringSpan getDescription() const = 0;
  virtual Utils::StringSpan getPatternTable() const = 0;
  virtual Utils::StringSpan getHighPatternTable(bool alternate) const = 0;
  virtual bool alternateHighPatternsActive() const { return false; };
  virtual void clearPlayerSheet(unsigned n) = 0;
  virtual Utils::StringSpan getPlayerName(unsigned n) const = 0;
  virtual void setPlayerName(unsigned n, Utils::StringSpan name) = 0;
  virtual byte getPlayerHP(unsigned n) const = 0;
  virtual byte getPlayerWD(unsigned n) const = 0;
  virtual byte getPlayerClass(unsigned n) const = 0;
  virtual void setPlayerClass(unsigned n, unsigned c) = 0;
  virtual byte getPlayerRow(unsigned n) const = 0;
  virtual void setPlayerRow(unsigned n, byte row) = 0;
  virtual byte getPlayerColumn(unsigned n) const = 0;
  virtual void setPlayerColumn(unsigned n, byte column) = 0;
  virtual void setPlayerStartPosition(unsigned n, StartPosition pos, Direction dir) = 0;
  virtual bool isQuestObjectFound(unsigned n) const = 0;
  virtual bool isQuestObjectIntact(unsigned n) const = 0;
  virtual uint16 getTurnsLeft(unsigned n) const = 0;
  virtual byte getRations() const = 0;
  virtual Utils::StringSpan getClassName(unsigned n) const = 0;
  virtual Utils::StringSpan getClassPatternTable(unsigned n) const = 0;
  virtual byte getMaxPlayers() const = 0;
  virtual byte getNumClassChoices() const = 0;
  virtual byte getMaxFloors() const = 0;
  virtual bool hasHiddenMap() const = 0;
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
  virtual byte getCurrentFloor() const = 0;
  virtual int getCurrentPlayer() const = 0;
  virtual void setCurrentPlayer(int n) = 0;
  virtual bool nextPlayerInOrder() = 0;
  virtual Location getCurrentLocation() const = 0;
  virtual Direction getSavedDirection() const = 0;
  virtual void setSavedDirection(Direction direction) = 0;
  virtual byte getSavedProgression() const = 0;
  virtual void setSavedProgression(byte progression) = 0;
  virtual Utils::StringSpan questObjectName(unsigned n) const = 0;
  virtual byte getPlayerColor(unsigned n) const = 0;
  virtual void setPlayerColor(unsigned n, unsigned c) = 0;
  virtual Utils::StringSpan getColorTable() const = 0;
  virtual byte getKeymapEntry(KeyMapping k) const = 0;
  virtual Utils::StringSpan getExtDictionaryWord(byte n) const = 0;
  virtual Utils::StringSpan getDictionaryWord(byte n) const = 0;
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name) = 0;
  virtual MapPosition getMapPosition() const = 0;
  virtual void setMapPosition(MapPosition pos) = 0;
  virtual uint16 getPartyGold() const = 0;
  virtual byte getMappedFloors() const = 0;
  virtual Utils::StringSpan getFloorMap() const = 0;
  virtual void setMapVisited(MapPosition pos, bool visited) = 0;
  virtual bool inCombat() const = 0;
  virtual Location mapLocation(MapPosition pos) const = 0;
  virtual bool canMove(MapPosition pos, Direction dir, Location &dest) const = 0;
  bool canMove(MapPosition pos, Direction dir) const
  { Location dest; return canMove(pos, dir, dest); }
  virtual bool blockedForward(MapPosition pos, Direction dir) const = 0;
  bool blockedBackward(MapPosition pos, Direction dir) const
  { return blockedForward(pos, reverse(dir)); }
  virtual bool getSecretDoorsRevealed() const = 0;
  virtual void setSecretDoorsRevealed(bool value) = 0;
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
