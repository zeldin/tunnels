#ifndef TUNNELS_CLASSIC_DATABASEIMPL_H_
#define TUNNELS_CLASSIC_DATABASEIMPL_H_

#include "Database.h"

namespace Tunnels {

namespace Classic {

class DatabaseFactory;

class DatabaseImpl : public Database {
  friend class Classic::DatabaseFactory;
private:
  struct {
    byte unknown_0400[0x200]; 
    byte description[384]; // V@>0600
    byte unknown_0780[0x80];
    byte patternTable[0x800]; // V@>0800
    struct {
      byte name[15];
      byte unknown1[0x14];
      byte classId;
      byte unknown2[0x18];
    } player[4];	  // V@>1000
    byte unknown_10f0[0x82];
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
    byte unknown_1ce3[7]; // V@>1CE3
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
    byte unknown_1d00;    // V@>1D00
    byte unknown_1d01;    // V@>1D01
    byte unknown_1d02[0x116e];
    byte playerColor[4];  // V@>2E70
    byte unknown_2e74[0x41c];
    byte dictionary[36][12]; // V@3290
    byte unknown_3440[0x292];
    byte pab[10];         // V@>36D2
    byte dsrname[28];     // V@>37DC
    byte unknown_36f8[8];
  } data;
protected:
  DatabaseImpl() { }
public:
  virtual Utils::StringSpan getDescription() const override;
  virtual Utils::StringSpan getPatternTable() const override;
  virtual void clearPlayerSheet(unsigned n) override;
  virtual Utils::StringSpan getPlayerName(unsigned n) const override;
  virtual void setPlayerName(unsigned n, Utils::StringSpan name) override;
  virtual byte getPlayerClass(unsigned n) const override { return data.player[n].classId >> 6; }
  virtual void setPlayerClass(unsigned n, unsigned c) override;
  virtual Utils::StringSpan getClassName(unsigned n) const override;
  virtual Utils::StringSpan getClassPatternTable(unsigned n) const override;
  virtual byte getMaxPlayers() const override { return data.maxPlayers; }
  virtual byte getNumClassChoices() const override;
  virtual byte getMaxFloors() const override { return data.maxFloors; }
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
  virtual byte getUnknown1D01() const override { return data.unknown_1d01; }
  virtual byte getPlayerColor(unsigned n) const override { return data.playerColor[n]; }
  virtual void setPlayerColor(unsigned n, unsigned c) override;
  virtual Utils::StringSpan getDictionaryWord(byte n) const override;
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name)
    override;
};
  
}
}

#endif // TUNNELS_CLASSIC_DATABASEIMPL_H_
