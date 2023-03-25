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
    byte unknown_0400[0x18da];
    byte maxPlayers;      // V@>1CDA
    byte unknown_1cdb[5];
    byte unknown_1ce0;    // V@>1CE0
    byte numClasses;      // V@>1CE1
    byte maxFloors;       // V@>1CE2
    byte unknown_1ce3[8]; // V@>1CE3
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
    byte unknown_1d02[0x158e];
    byte dictionary[36][12]; // V@3290
    byte unknown_3440[0x292];
    byte pab[10];         // V@>36D2
    byte dsrname[28];     // V@>37DC
    byte unknown_36f8[8];
  } data;
protected:
  DatabaseImpl() { }
public:
  virtual byte getMaxPlayers() const override { return data.maxPlayers; }
  virtual byte getMaxFloors() const override { return data.maxFloors; }
  virtual byte getUnknown1CEB() const override { return data.unknown_1ceb; }
  virtual byte getMinFloors() const override { return data.minFloors; }
  virtual byte getUnknown1CF4() const override { return data.unknown_1cf4; }
  virtual void setUnknown1CF4(byte x) override { data.unknown_1cf4 = x; }
  virtual void setNumFloors(byte num) override { data.numFloors = num; }
  virtual void setNumPlayers(byte num) override { data.numPlayers = num; }
  virtual void setDifficulty(byte dif) override { data.difficulty = dif; }
  virtual byte getUnknown1D01() const override { return data.unknown_1d01; }
  virtual Utils::StringSpan getDictionaryWord(byte n) const override;
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name)
    override;
};
  
}
}

#endif // TUNNELS_CLASSIC_DATABASEIMPL_H_
