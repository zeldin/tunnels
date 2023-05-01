#ifndef TUNNELS_CLASSIC_DATABASEIMPL_H_
#define TUNNELS_CLASSIC_DATABASEIMPL_H_

#include "Database.h"

namespace Tunnels {

namespace Classic {

class DatabaseFactory;

class DatabaseImpl : public Database {
  friend class Classic::DatabaseFactory;
private:
  struct msb16 {
    byte hi, lo;
    constexpr operator uint16 () const { return (((uint16)hi)<<8)|lo; }
    const uint16& operator=(const uint16& n) {
      hi = n>>8;
      lo = n;
      return n;
    }
  };
  class PosWord {
  private:
    uint16 v;
  public:
    constexpr PosWord(uint16 pos) : v(pos) {}
    constexpr PosWord(MapPosition pos) : v((pos.y << 5) | pos.x) {}
    constexpr operator uint16() { return v; }
    constexpr operator MapPosition()
    { return MapPosition{byte(v&0x1f), byte(v>>5)}; }
  };

  struct {
    byte unknown_0400[0x200]; 
    byte description[384]; // V@>0600
    byte unknown_0780[0x80];
    byte patternTable[0x800]; // V@>0800
    struct {
      byte name[15];
      byte unknown1[0x14];
      byte classId;
      byte unknown2[0x2];
      byte row;
      byte column;
      byte unknown3[0x14];
    } player[4];	  // V@>1000
    byte unknown_10f0[0x08];
    msb16 mapPosition;    // V@>10F8
    byte unknown_10fa;    // V@>10FA
    byte unknown_10fb;    // V@>10FB
    msb16 partyGold;      // V@>10FC
    byte numMappedFloors; // V@>10FE
    byte unknown_10ff[0x1d];
    byte unknown_111c;    // V@>111C
    byte unknown_111d[0x55];
    struct {
      byte name[10];
      byte unknown[0xc];
    } classes[4];         // V@>1172
    byte classPatternTable[4][0x40]; // V@>11CA
    byte unknown_12ca[0xa10];
    byte maxPlayers;      // V@>1CDA
    byte unknown_1cdb[5];
    byte unknown_1ce0;    // V@>1CE0
    byte numClasses;      // V@>1CE1
    byte maxFloors;       // V@>1CE2
    byte unknown_1ce3[5]; // V@>1CE3
    byte unknown_1ce8;    // V@>1CE8
    byte unknown_1ce9;    // V@>1CE9
    byte numConfPlayers;  // V@>1CEA
    byte unknown_1ceb;    // V@>1CEB
    byte unknown_1cec;    // V@>1CEC
    byte minFloors;       // V@>1CED
    byte unknown_1cee[6]; // V@>1CEE
    byte unknown_1cf4;    // V@>1CF4
    byte numFloors;       // V@>1CF5
    byte numPlayers;      // V@>1CF6
    byte difficulty;      // V@>1CF7
    byte currentFloor;    // V@>1CF8
    byte unknown_1cf9;    // V@>1CF9
    byte currentPlayer;   // V@>1CFA
    byte unknown_1cfb[5];
    byte currentLocation; // V@>1D00
    byte activeHighPatternTable; // V@>1D01
    byte unknown_1d02;    // V@>1D02
    byte unknown_1d03;    // V@>1D03
    byte unknown_1d04[0x8ec];
    byte savedDirection;  // V@>25F0
    byte unknown_25f1[0xc];
    byte savedProgression; // V@>25FD
    byte unknown_25fe[0x78];
    byte playerOrder[4];  // V@>2676
    byte unknown_267a[0x7f6];
    byte patternColors[32];  // V@>2E70
    byte patternTable2[0x380]; // V@2E90
    byte unknown_3210[0x80];
    byte dictionary[36][12]; // V@3290
    byte unknown_3440[0x78];
    byte floorMap[17*32-6]; // V@>34B8
    byte pab[10];         // V@>36D2
    byte dsrname[28];     // V@>37DC
    byte unknown_36f8[8];
  } data;
protected:
  DatabaseImpl() { }
public:
  virtual Utils::StringSpan getDescription() const override;
  virtual Utils::StringSpan getPatternTable() const override;
  virtual Utils::StringSpan getHighPatternTable(bool alternate) const override;
  virtual bool alternateHighPatternsActive() const override { return data.activeHighPatternTable != 1; }
  virtual void clearPlayerSheet(unsigned n) override;
  virtual Utils::StringSpan getPlayerName(unsigned n) const override;
  virtual void setPlayerName(unsigned n, Utils::StringSpan name) override;
  virtual byte getPlayerClass(unsigned n) const override { return data.player[n].classId >> 6; }
  virtual void setPlayerClass(unsigned n, unsigned c) override;
  virtual byte getPlayerRow(unsigned n) const override { return data.player[n].row; }
  virtual void setPlayerRow(unsigned n, byte row) override { data.player[n].row = row; }
  virtual byte getPlayerColumn(unsigned n) const override { return data.player[n].column; }
  virtual void setPlayerColumn(unsigned n, byte column) override { data.player[n].column = column; }
  virtual void setPlayerStartPosition(unsigned n, StartPosition pos, Direction dir) override;
  virtual Utils::StringSpan getClassName(unsigned n) const override;
  virtual Utils::StringSpan getClassPatternTable(unsigned n) const override;
  virtual byte getMaxPlayers() const override { return data.maxPlayers; }
  virtual byte getNumClassChoices() const override;
  virtual byte getMaxFloors() const override { return data.maxFloors; }
  virtual bool hasHiddenMap() const override { return data.unknown_1ce8 != 0; }
  virtual byte getNumConfiguredPlayers() const override { return data.numConfPlayers; }
  virtual void setNumConfiguredPlayers(byte num) override { data.numConfPlayers = num; }
  virtual byte getUnknown1CEB() const override { return data.unknown_1ceb; }
  virtual void setUnknown1CEB(byte x) override { data.unknown_1ceb = x; }
  virtual byte getMinFloors() const override { return data.minFloors; }
  virtual byte getUnknown1CF4() const override { return data.unknown_1cf4; }
  virtual void setUnknown1CF4(byte x) override { data.unknown_1cf4 = x; }
  virtual void setNumFloors(byte num) override { data.numFloors = num; }
  virtual byte getNumPlayers() const override { return data.numPlayers; }
  virtual void setNumPlayers(byte num) override { data.numPlayers = num; }
  virtual void setDifficulty(byte dif) override { data.difficulty = dif; }
  virtual byte getCurrentFloor() const override { return data.currentFloor; }
  virtual int getCurrentPlayer() const override { return data.currentPlayer-1; }
  virtual void setCurrentPlayer(int n) override { data.currentPlayer = n+1; }
  virtual bool nextPlayerInOrder() override;
  virtual Location getCurrentLocation() const override { return static_cast<Location>(data.currentLocation); }
  virtual Direction getSavedDirection() const override { return static_cast<Direction>(data.savedDirection & 3); }
  virtual void setSavedDirection(Direction direction) override { data.savedDirection = direction; }
  virtual byte getSavedProgression() const override { return data.savedProgression; }
  virtual void setSavedProgression(byte progression) override { data.savedProgression = progression; }
  virtual byte getPlayerColor(unsigned n) const override { return data.patternColors[n]; }
  virtual void setPlayerColor(unsigned n, unsigned c) override;
  virtual Utils::StringSpan getColorTable() const override;
  virtual Utils::StringSpan getDictionaryWord(byte n) const override;
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name)
    override;
  virtual MapPosition getMapPosition() const override { return PosWord(data.mapPosition); }
  virtual void setMapPosition(MapPosition pos) { data.mapPosition = PosWord(pos); }
  virtual uint16 getPartyGold() const override { return data.partyGold; }
  virtual byte getMappedFloors() const override { return data.numMappedFloors; }
  virtual Utils::StringSpan getFloorMap() const override;
  virtual void setMapVisited(MapPosition pos, bool visited) override;
  virtual bool inCombat() const override { return data.unknown_1d03 != 0; }
  virtual Location mapLocation(MapPosition pos) const override;
  virtual bool canMove(MapPosition pos, Direction dir, Location &dest) const override;
  virtual bool blockedForward(MapPosition pos, Direction dir) const override;
  virtual bool getSecretDoorsRevealed() const override { return data.unknown_111c & 0x8; }
};
  
}
}

#endif // TUNNELS_CLASSIC_DATABASEIMPL_H_
