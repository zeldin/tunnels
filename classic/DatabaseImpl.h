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
  struct RoomDescriptor {
    msb16 mapPosition;
    byte specialRoomType;
    byte monsterInfo;
    byte roomFlags;
    byte goldAmount;
    byte lootInfo;
    byte lootId[3];
  };
  enum {
    ROOM_FLAG_CHEST = 0x01,
    ROOM_FLAG_TRAP = 0x02,
    ROOM_FLAG_VISITED = 0x08,
    ROOM_FLAG_LIVING_STATUE = 0x10,
    ROOM_FLAG_FOUNTAIN = 0x20
  };

  struct {
    byte spritePatternTable[0x200]; // V@>0400
    byte description[384]; // V@>0600
    byte unknown_0780[0x80];
    byte patternTable[0x800]; // V@>0800
    struct {
      byte name[15];
      byte HP;
      byte WD;
      byte armorId;
      int8 armorProtection;
      byte shieldId;
      int8 shieldProtection;
      byte primaryWeaponId;
      byte primaryWeaponDamage;
      byte primaryWeaponAmmo;
      byte secondaryWeaponId;
      byte secondaryWeaponDamage;
      byte secondaryWeaponAmmo;
      int8 baseProtection;
      int8 weaponBonus;
      byte unknown2;
      msb16 exp;
      byte unknown3[0x2];
      byte level;
      byte flags;
      byte unknown4[0x2];
      byte row;
      byte column;
      struct {
	byte id;
	byte remainingUses;
      } magicItems[10];
    } player[4];	  // V@>1000
    byte unknown_10f0[0x08];
    msb16 mapPosition;    // V@>10F8
    byte unknown_10fa;    // V@>10FA
    byte unknown_10fb;    // V@>10FB
    msb16 partyGold;      // V@>10FC
    byte numMappedFloors; // V@>10FE
    byte unknown_10ff;    // V@>10FF
    byte foundQuestObjects; // V@>1100
    byte remainingQuestObjects; // V@>1101
    msb16 turnsLeft[8];   // V@>1102
    byte unknown_1112;    // V@>1112
    byte rations;         // V@>1113
    byte unknown_1114[8];
    byte unknown_111c;    // V@>111C
    byte unknown_111d[9];
    byte monsterName[12]; // V@>1126
    byte monsterHPNumD6;  // V@>1132
    byte monsterDefense;  // V@>1133
    byte monsterAttack;   // V@>1134
    byte monsterMaxDamage; // V@>1135
    byte monsterSpecialAttackChance; // V@>1136
    byte monsterSpecialAttackId; // V@>1137
    byte unknown_1138;   // V@>1138
    byte monsterSound;   // V@>1139
    byte monsterPatternNumber; // V@>113A
    byte monsterNegotiation; // V@>113B
    byte monsterMobility;   // V@>113C
    byte monsterResistance; // V@>113D
    byte unknown_113e;      // V@>113E
    byte monsterSpeed;      // V@>113F
    byte monsterHP[8];
    struct {
      byte row;
      byte column;
    } monsterPosition[7]; // V@>1148
    byte unknown_1156;  // V@>1156
    byte unknown_1157;  // V@>1157
    byte unknown_1158[6];
    byte unknown_115e;  // V@>115E
    byte unknown_115f[0x13];
    struct {
      byte name[10];
      byte unknown[0xc];
    } classes[4];         // V@>1172
    byte classPatternTable[4][0x40]; // V@>11CA
    struct {
      byte name[12];
      byte hpNumD6;
      byte defense;
      byte attack;
      byte maxDamage;
      byte specialAttackChance;
      byte specialAttackId;
      byte unknown1;
      byte unknown2;
      byte unknown3;
      byte unknown4;
    } monsters[56];       // V@>12CA
    struct {
      byte name[15];
      byte effect;
    } specialAttacks[20]; // V@>179A
    byte monsterPatternTable[16][0x40]; // V@>18DA
    byte maxPlayers;      // V@>1CDA
    byte unknown_1cdb[5];
    byte unknown_1ce0;    // V@>1CE0
    byte numClasses;      // V@>1CE1
    byte maxFloors;       // V@>1CE2
    byte unknown_1ce3[2]; // V@>1CE3
    byte unknown_1ce5;    // V@>1CE5
    byte numFountainsPerFloor; // V@>1CE6
    byte numStairsPerFloor;    // V@>1CE7
    byte unknown_1ce8;    // V@>1CE8
    byte unknown_1ce9;    // V@>1CE9
    byte numConfPlayers;  // V@>1CEA
    byte unknown_1ceb;    // V@>1CEB
    byte numRoomsPerFloor;// V@>1CEC
    byte minFloors;       // V@>1CED
    byte unknown_1cee[6]; // V@>1CEE
    byte unknown_1cf4;    // V@>1CF4
    byte numFloors;       // V@>1CF5
    byte numPlayers;      // V@>1CF6
    byte difficulty;      // V@>1CF7
    byte currentFloor;    // V@>1CF8
    byte unknown_1cf9;    // V@>1CF9
    byte currentPlayer;   // V@>1CFA
    byte unknown_1cfb;    // V@>1CFB
    byte unknown_1cfc[4];
    byte currentLocation; // V@>1D00
    byte activeHighPatternTable; // V@>1D01
    byte unknown_1d02;    // V@>1D02
    byte monsterInfo;     // V@>1D03
    msb16 descriptorBytesPerFloor; // V@>1D04
    msb16 ascendingStairsDescriptorOffset; // V@>1D06
    msb16 descendingStairsDescriptorOffset; // V@>1D08
    msb16 fountainDescriptorOffset; // V@>1D0A
    byte floorDescriptors[0x85c]; // V@>1D0C
    byte unknown_2568[0x3c];
    byte monsterSoundTable[16]; // V@>25A4
    msb16 monsterSoundAddress;  // V@>25B4
    byte unknown_25b6[0x39];
    byte unknown_25ef;    // V@>25EF
    byte savedDirection;  // V@>25F0
    byte unknown_25f1[0x3];
    msb16 savedFloorDescriptorAddr; // V@>25F4
    msb16 savedRoomDescriptorAddr; // V@>25F6
    byte unknown_25f8[0x4];
    byte savedActionKey;   // V@>25FC
    byte savedProgression; // V@>25FD
    byte unknown_25fe[0x78];
    byte playerOrder[4];  // V@>2676
    byte unknown_267a[0xa];
    struct {
      byte row;
      byte column;
    } roomItemPosition[6]; // V@>2684
    byte unknown_2690[0x68];
    struct {
      byte name[15];
      byte damage;
      byte unknown[2];
    } weapons[8]; // V@>26F8
    struct {
      byte name[15];
      byte damage;
      byte unknown1[2];
      int8 defaultAmmo;
      int8 ammoType;
      byte unknown2;
      byte ammoName[13];
    } rangedWeapons[8]; // V@>2788
    struct {
      byte name[15];
      byte protection;
      byte unknown[2];
    } armors[8]; // V@>2898
    struct {
      byte name[15];
      byte protection;
      byte unknown[2];
    } shields[6]; // V@>2928
    struct {
      byte name[11];
      byte tiles[4];
      byte unknown;
    } magicItemCategories[8]; // V@>2994
    struct {
      byte name[15];
      byte effect;
      byte usesLimit;
      byte unknown;
    } magicItems[40]; // V@>2a14
    byte unknown_2ce4[0x32];
    struct {
      byte name[11];
      byte unknown1[2];
      byte tiles[4];
      byte unknown2[2];
    } questObjects[8];     // V@>2D16
    byte unknown_2dae[0x90];
    byte corridorColors[5][10]; // V@>2E3E
    byte patternColors[32];  // V@>2E70
    byte patternTable2[0x380]; // V@2E90
    byte objectTiles[9][4]; // V@3210
    byte unknown_3234[0x9];
    byte keymap[11];         // V@323D
    byte unknown_3248[0x8];
    byte extDictionary[4][16]; // V@3250
    byte dictionary[36][12]; // V@3290
    byte magicEffect[32][3]; // V@3440
    byte roomFeatureName[2][12]; // V@34A0
    byte floorMap[17*32-6]; // V@>34B8
    byte pab[10];         // V@>36D2
    byte dsrname[28];     // V@>36DC
    byte itemCategory; // V@>36F8
    byte itemId;       // V@>36F9
    byte itemStat;     // V@>36FA
    byte itemAmmo;     // V@>36FB
    byte unknown_36fc[4];
  } data;
protected:
  DatabaseImpl() { }
public:
  virtual Utils::StringSpan getSpritePatternTable() const override;
  virtual Utils::StringSpan getDescription() const override;
  virtual Utils::StringSpan getPatternTable() const override;
  virtual Utils::StringSpan getHighPatternTable(bool alternate) const override;
  virtual void clearRoomFixtures() override;
  virtual void setRoomFixture(RoomFixture fixture) override;
  virtual bool alternateHighPatternsActive() const override { return data.activeHighPatternTable != 1; }
  virtual void clearPlayerSheet(unsigned n) override;
  virtual Utils::StringSpan getPlayerName(unsigned n) const override;
  virtual void setPlayerName(unsigned n, Utils::StringSpan name) override;
  virtual byte getPlayerHP(unsigned n) const override { return data.player[n].HP; }
  virtual byte getPlayerWD(unsigned n) const override { return data.player[n].WD; }
  virtual byte getPlayerArmorId(unsigned n) const override { return data.player[n].armorId; }
  virtual void setPlayerArmor(unsigned n, byte item) override;
  virtual int8 getPlayerArmorProtection(unsigned n) const override { return data.player[n].armorProtection; }
  virtual void setPlayerArmorProtection(unsigned n, int8 prot) override { data.player[n].armorProtection = prot; }
  virtual byte getPlayerShieldId(unsigned n) const override { return data.player[n].shieldId; }
  virtual void setPlayerShield(unsigned n, byte item) override;
  virtual int8 getPlayerShieldProtection(unsigned n) const override { return data.player[n].shieldProtection; }
  virtual void setPlayerShieldProtection(unsigned n, int8 prot) override { data.player[n].shieldProtection = prot; }
  virtual byte getPlayerWeapon(unsigned n, bool secondary, ItemCategory &cat) const override;
  virtual void setPlayerWeapon(unsigned n, bool secondary, ItemCategory cat, byte item) override;
  virtual byte getPlayerWeaponDamage(unsigned n, bool secondary) const override { return (secondary? data.player[n].secondaryWeaponDamage : data.player[n].primaryWeaponDamage); }
  virtual void setPlayerWeaponDamage(unsigned n, bool secondary, byte dmg) override { if(secondary)  data.player[n].secondaryWeaponDamage = dmg; else data.player[n].primaryWeaponDamage = dmg; }
  virtual byte getPlayerWeaponAmmo(unsigned n, bool secondary) const override { return (secondary? data.player[n].secondaryWeaponAmmo : data.player[n].primaryWeaponAmmo); }
  virtual int8 getPlayerBaseProtection(unsigned n) const override { return data.player[n].baseProtection; }
  virtual byte getPlayerWeaponBonus(unsigned n) const override { return data.player[n].weaponBonus; }
  virtual uint16 getPlayerExp(unsigned n) const override { return data.player[n].exp; }
  virtual byte getPlayerLevel(unsigned n) const override { return data.player[n].level; }
  virtual byte getPlayerClass(unsigned n) const override { return data.player[n].flags >> 6; }
  virtual void setPlayerClass(unsigned n, unsigned c) override;
  virtual bool isPlayerWeaponSwapped(unsigned n) const override { return (data.player[n].flags >> 5) & 1; }
  virtual void swapPlayerWeapon(unsigned n) override { data.player[n].flags ^= 0x20; }
  virtual byte getPlayerRow(unsigned n) const override { return data.player[n].row; }
  virtual byte getPlayerColumn(unsigned n) const override { return data.player[n].column; }
  virtual void placePlayer(unsigned n, unsigned y, unsigned x) override;
  virtual void setPlayerStartPosition(unsigned n, StartPosition pos, Direction dir) override;
  virtual byte getPlayerMagicItemId(unsigned n, unsigned m) const override { return data.player[n].magicItems[m].id; }
  virtual void setPlayerMagicItemId(unsigned n, unsigned m, byte id) override { data.player[n].magicItems[m].id = id; }
  virtual byte getPlayerMagicItemRemainingUses(unsigned n, unsigned m) const override { return data.player[n].magicItems[m].remainingUses; }
  virtual void setPlayerMagicItemRemainingUses(unsigned n, unsigned m, byte uses) override { data.player[n].magicItems[m].remainingUses = uses; }
  virtual void compactPlayerMagicItems(unsigned n) override;
  virtual void revealAllMagicItems() override;
  virtual bool isQuestObjectFound(unsigned n) const override { return (data.foundQuestObjects >> n)&1; }
  virtual bool isQuestObjectRemaining(unsigned n) const override { return (data.remainingQuestObjects >> n)&1; }
  virtual bool isAnyQuestObjectRemaining() const override { return data.remainingQuestObjects != 0; }
  virtual bool tryAchieveQuestObject(unsigned n) override;
  virtual void clearRemainingQuestObjects() override { data.remainingQuestObjects = 0; }
  virtual uint16 getTurnsLeft(unsigned n) const override { return data.turnsLeft[n]; }
  virtual byte getRations() const override { return data.rations; }
  virtual Utils::StringSpan getMonsterName() const override;
  virtual byte getMonsterHPNumD6() const override { return data.monsterHPNumD6; }
  virtual byte getMonsterDefense() const override { return data.monsterDefense; }
  virtual byte getMonsterAttack() const override { return data.monsterAttack; }
  virtual byte getMonsterMaxDamage() const override { return data.monsterMaxDamage; }
  virtual byte getMonsterSpecialAttackChance() const override { return data.monsterSpecialAttackChance; }
  virtual byte getMonsterSpecialAttackId() const override { return data.monsterSpecialAttackId; }
  virtual int getMonsterSound() override;
  virtual byte getMonsterNegotiation() const override { return data.monsterNegotiation * 25; }
  virtual byte getMonsterMobility() const override { return data.monsterMobility * 25; }
  virtual byte getMonsterResistance() const override { return data.monsterResistance * 10; }
  virtual byte getMonsterSpeed() const override { return data.monsterSpeed; }
  virtual byte getMonsterHP(unsigned n) const override { return data.monsterHP[n]; }
  virtual void setMonsterHP(unsigned n, byte hp) override { data.monsterHP[n] = hp; }
  virtual void placeMonster(unsigned n, unsigned y, unsigned x) override;
  virtual byte getMonsterRow(unsigned n) const override { return data.monsterPosition[n].row; }
  virtual byte getMonsterColumn(unsigned n) const override { return data.monsterPosition[n].column; }
  virtual bool isMonsterPlaced(unsigned n) const override { return data.monsterPosition[n].row != 0 || data.monsterPosition[n].column != 0; }
  virtual Utils::StringSpan getClassName(unsigned n) const override;
  virtual Utils::StringSpan getClassPatternTable(unsigned n) const override;
  virtual Utils::StringSpan getSpecialAttackName(unsigned n) const override;
  virtual byte getSpecialAttackEffect(unsigned n) const override { return data.specialAttacks[n-1].effect; }
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
  virtual void setCurrentFloor(byte floor) override { data.currentFloor = floor; }
  virtual int getCurrentPlayer() const override { return data.currentPlayer-1; }
  virtual void setCurrentPlayer(int n) override { data.currentPlayer = n+1; }
  virtual bool nextPlayerInOrder() override;
  virtual Location getCurrentLocation() const override { return static_cast<Location>(data.currentLocation); }
  virtual void setCurrentLocation(Location loc) override { data.currentLocation = loc; }
  virtual byte getUnknown25EF() const override { return data.unknown_25ef; }
  virtual Direction getSavedDirection() const override { return static_cast<Direction>(data.savedDirection & 3); }
  virtual void setSavedDirection(Direction direction) override { data.savedDirection = direction; }
  virtual DescriptorHandle getSavedRoomAddress() const override;
  virtual void setSavedRoomAddress(DescriptorHandle room) override;
  virtual byte getSavedActionKey() const override { return data.savedActionKey; }
  virtual void setSavedActionKey(byte actionKey) override { data.savedActionKey = actionKey; }
  virtual byte getSavedProgression() const override { return data.savedProgression; }
  virtual void setSavedProgression(byte progression) override { data.savedProgression = progression; }
  virtual void clearRoomItemPosition(RoomItem n) override;
  virtual void placeRoomItem(RoomItem n, byte y, byte x) override;
  virtual void placeRoomItemCenter(RoomItem n) override;
  virtual void copyRoomItemPosition(RoomItem n, RoomItem m) override;
  virtual byte getRoomItemRow(RoomItem n) const override { return data.roomItemPosition[n].row; }
  virtual byte getRoomItemColumn(RoomItem n) const override { return data.roomItemPosition[n].column; }
  virtual bool isRoomItemPlaced(RoomItem n) const override { return data.roomItemPosition[n].row != 0 || data.roomItemPosition[n].column != 0; }
  virtual Utils::StringSpan getItemName(ItemCategory cat, byte id) const override;
  virtual Utils::StringSpan getItemTiles(ItemCategory cat, byte id) const override;
  virtual int8 getRangedWeaponAmmoType(unsigned id) const override;
  virtual Utils::StringSpan getRangedWeaponAmmoName(unsigned id) const override;
  virtual byte getMagicItemEffect(byte n) const override { n = abs(n)-1; return (n < 40? data.magicItems[n].effect : 0); }
  virtual byte getMagicItemInitialUses(byte n) const override;
  virtual byte getPlayerColor(unsigned n) const override { return data.patternColors[n]; }
  virtual void setPlayerColor(unsigned n, unsigned c) override;
  virtual bool isQuestObjectValid(unsigned n) const override { return n<8 && data.questObjects[n].name[0] != ' '; }
  virtual Utils::StringSpan getFloorColorTable(unsigned floor) const override;
  virtual Utils::StringSpan getColorTable() const override;
  virtual Utils::StringSpan getLivingStatueTiles() const override;
  virtual Utils::StringSpan getFountainTiles() const override;
  virtual Utils::StringSpan getChestTiles() const override;
  virtual Utils::StringSpan getMoneyTiles() const override;
  virtual byte getKeymapEntry(KeyMapping k) const override { return data.keymap[k]; }
  virtual Utils::StringSpan getExtDictionaryWord(byte n) const override;
  virtual Utils::StringSpan getDictionaryWord(byte n) const override;
  virtual void getMagicEffectDescriptor(byte b, Base36Number (&effect)[3]) const override;
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name)
    override;
  virtual MapPosition getMapPosition() const override { return PosWord(data.mapPosition); }
  virtual void setMapPosition(MapPosition pos) { data.mapPosition = PosWord(pos); }
  virtual byte getUnknown10FB() const override { return data.unknown_10fb; }
  virtual uint16 getPartyGold() const override { return data.partyGold; }
  virtual void setPartyGold(uint16 n) override { data.partyGold = n; }
  virtual byte getMappedFloors() const override { return data.numMappedFloors; }
  virtual void setMappedFloors(byte n) override { data.numMappedFloors = n; }
  virtual DescriptorHandle getRoomDescriptor(MapPosition pos) const override { return findDescriptor(PosWord(pos)); }
  virtual byte getRoomSpecialType(DescriptorHandle room) const override { return roomDescriptor(room)->specialRoomType; }
  virtual bool roomHasEnemies(DescriptorHandle room) const override { return (roomDescriptor(room)->monsterInfo & 0xe0) != 0; }
  virtual void startCombat(DescriptorHandle room) override { data.monsterInfo = roomDescriptor(room)->monsterInfo; }
  virtual void clearRoomEnemies() override;
  virtual void prepareRoomEnemies(DescriptorHandle room) override;
  virtual bool roomHasUnopenedChest(DescriptorHandle room) const override { return (roomDescriptor(room)->roomFlags & ROOM_FLAG_CHEST) != 0; }
  virtual bool roomHasLivingStatue(DescriptorHandle room) const override { return (roomDescriptor(room)->roomFlags & ROOM_FLAG_LIVING_STATUE) != 0; }
  virtual bool roomHasFountain(DescriptorHandle room)  const override { return (roomDescriptor(room)->roomFlags & ROOM_FLAG_FOUNTAIN) != 0; }
  virtual void setRoomVisited(DescriptorHandle room) override { roomDescriptor(room)->roomFlags |= ROOM_FLAG_VISITED; }
  virtual byte getRoomMoneyAmount(DescriptorHandle room) const override { return roomDescriptor(room)->goldAmount; }
  virtual void setRoomMoneyAmount(DescriptorHandle room, byte n) override { roomDescriptor(room)->goldAmount = n; }
  virtual int getRoomNextLootSlot(DescriptorHandle room, unsigned &iterPos) const override;
  virtual void clearRoomLootSlot(DescriptorHandle room, unsigned iterPos, unsigned n) override;
  virtual bool dropItemInRoom(DescriptorHandle room, ItemCategory cat, byte id, byte itemStat, byte itemAmmo) override;
  virtual byte getRoomLootItem(DescriptorHandle room, unsigned n, ItemCategory &cat, byte &itemStat, byte &itemAmmo) override;
  virtual Utils::StringSpan getFloorMap() const override;
  virtual void setMapVisited(MapPosition pos, bool visited) override;
  virtual void prepareFloorMap(unsigned floor) override;
  virtual void restoreFloorVisitedMarkers() override;
  virtual bool inCombat() const override { return data.monsterInfo != 0; }
  virtual byte getNumEnemies() const override { return data.monsterInfo >> 5; }
  virtual void clearCombat() override { data.monsterInfo = 0; }
  virtual Location mapLocation(MapPosition pos) const override;
  virtual bool canMove(MapPosition pos, Direction dir, Location &dest) const override;
  virtual bool blockedForward(MapPosition pos, Direction dir) const override;
  virtual bool getSecretDoorsRevealed() const override { return data.unknown_111c & 0x8; }
  virtual void setSecretDoorsRevealed(bool value) override
  { if(value) data.unknown_111c |= 0x8; else data.unknown_111c &= ~0x8; }

private:
  static inline constexpr byte abs(byte n) {
    return ((n & 0x80)? byte(~n)+1 : n);
  }

  void clearMap(unsigned mode);
  unsigned findDescriptor(uint16 pos, unsigned offs, unsigned cnt, unsigned delta) const;
  unsigned findDescriptor(uint16 pos) const { return findDescriptor(pos, 0, data.numRoomsPerFloor, 10); }
  void addMapFeatures(unsigned offs, unsigned cnt, byte code, unsigned delta);
  unsigned countBlankNeighbors(unsigned offs);
  void addMapVerticalCorridors();
  void addMapHorizontalCorridors();
  void fixupConnections();
  RoomDescriptor *roomDescriptor(DescriptorHandle h) { return reinterpret_cast<RoomDescriptor *>(data.floorDescriptors + (h > sizeof(data.floorDescriptors)/sizeof(data.floorDescriptors[0])-10? 0 : h)); }
  const RoomDescriptor *roomDescriptor(DescriptorHandle h) const { return reinterpret_cast<const RoomDescriptor *>(data.floorDescriptors + (h > sizeof(data.floorDescriptors)/sizeof(data.floorDescriptors[0])-10? 0 : h)); }
};
  
}
}

#endif // TUNNELS_CLASSIC_DATABASEIMPL_H_
