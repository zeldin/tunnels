#ifndef TUNNELS_DATABASE_H_
#define TUNNELS_DATABASE_H_

#include "GameTypes.h"

namespace Tunnels {

namespace Utils { class StringSpan; }
namespace File { class ReadHandle; class WriteHandle; }

class RandomSource;

class Database {
public:
  virtual Utils::StringSpan getSpritePatternTable() const = 0;
  virtual Utils::StringSpan getDescription() const = 0;
  virtual Utils::StringSpan getPatternTable() const = 0;
  virtual Utils::StringSpan getHighPatternTable(bool alternate) const = 0;
  virtual void clearRoomFixtures() = 0;
  virtual void setRoomFixture(RoomFixture fixture) = 0;
  virtual bool alternateHighPatternsActive() const { return false; };
  virtual void clearPlayerSheet(unsigned n) = 0;
  virtual Utils::StringSpan getPlayerName(unsigned n) const = 0;
  virtual void setPlayerName(unsigned n, Utils::StringSpan name) = 0;
  virtual byte getPlayerHP(unsigned n) const = 0;
  virtual byte getPlayerWD(unsigned n) const = 0;
  virtual byte getPlayerArmorId(unsigned n) const = 0;
  virtual void setPlayerArmor(unsigned n, byte item) = 0;
  virtual int8 getPlayerArmorProtection(unsigned n) const = 0;
  virtual void setPlayerArmorProtection(unsigned n, int8 prot) = 0;
  virtual byte getPlayerShieldId(unsigned n) const = 0;
  virtual void setPlayerShield(unsigned n, byte item) = 0;
  virtual int8 getPlayerShieldProtection(unsigned n) const = 0;
  virtual void setPlayerShieldProtection(unsigned n, int8 prot) = 0;
  virtual byte getPlayerWeapon(unsigned n, bool secondary, ItemCategory &cat) const = 0;
  virtual void setPlayerWeapon(unsigned n, bool secondary, ItemCategory cat, byte item) = 0;
  virtual byte getPlayerWeaponDamage(unsigned n, bool secondary) const = 0;
  virtual void setPlayerWeaponDamage(unsigned n, bool secondary, byte dmg) = 0;
  virtual byte getPlayerWeaponAmmo(unsigned n, bool secondary) const = 0;
  virtual int8 getPlayerBaseProtection(unsigned n) const = 0;
  virtual byte getPlayerWeaponBonus(unsigned n) const = 0;
  virtual uint16 getPlayerExp(unsigned n) const = 0;
  virtual byte getPlayerLevel(unsigned n) const = 0;
  virtual byte getPlayerClass(unsigned n) const = 0;
  virtual void setPlayerClass(unsigned n, unsigned c) = 0;
  virtual bool isPlayerWeaponSwapped(unsigned n) const = 0;
  virtual void swapPlayerWeapon(unsigned n) = 0;
  virtual byte getPlayerRow(unsigned n) const = 0;
  virtual byte getPlayerColumn(unsigned n) const = 0;
  virtual void placePlayer(unsigned n, unsigned y, unsigned x) = 0;
  virtual void setPlayerStartPosition(unsigned n, StartPosition pos, Direction dir) = 0;
  virtual byte getPlayerMagicItemId(unsigned n, unsigned m) const = 0;
  virtual void setPlayerMagicItemId(unsigned n, unsigned m, byte id) = 0;
  virtual byte getPlayerMagicItemRemainingUses(unsigned n, unsigned m) const = 0;
  virtual void setPlayerMagicItemRemainingUses(unsigned n, unsigned m, byte uses) = 0;
  virtual void compactPlayerMagicItems(unsigned n) = 0;
  virtual void revealAllMagicItems() = 0;
  virtual bool isQuestObjectFound(unsigned n) const = 0;
  virtual bool isQuestObjectRemaining(unsigned n) const = 0;
  virtual bool isAnyQuestObjectRemaining() const = 0;
  virtual bool tryAchieveQuestObject(unsigned n) = 0;
  virtual void clearRemainingQuestObjects() = 0;
  virtual uint16 getTurnsLeft(unsigned n) const = 0;
  virtual byte getRations() const = 0;
  virtual Utils::StringSpan getMonsterName() const = 0;
  virtual byte getMonsterHPNumD6() const = 0;
  virtual byte getMonsterDefense() const = 0;
  virtual byte getMonsterAttack() const = 0;
  virtual byte getMonsterMaxDamage() const = 0;
  virtual byte getMonsterSpecialAttackChance() const = 0;
  virtual byte getMonsterSpecialAttackId() const = 0;
  virtual int getMonsterSound() = 0;
  virtual byte getMonsterNegotiation() const = 0;
  virtual byte getMonsterMobility() const = 0;
  virtual byte getMonsterResistance() const = 0;
  virtual byte getMonsterSpeed() const = 0;
  virtual byte getMonsterHP(unsigned n) const = 0;
  virtual void setMonsterHP(unsigned n, byte hp) = 0;
  virtual void placeMonster(unsigned n, unsigned y, unsigned x) = 0;
  virtual byte getMonsterRow(unsigned n) const = 0;
  virtual byte getMonsterColumn(unsigned n) const = 0;
  virtual bool isMonsterPlaced(unsigned n) const = 0;
  virtual Utils::StringSpan getClassName(unsigned n) const = 0;
  virtual Utils::StringSpan getClassPatternTable(unsigned n) const = 0;
  virtual Utils::StringSpan getSpecialAttackName(unsigned n) const = 0;
  virtual byte getSpecialAttackEffect(unsigned n) const = 0;
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
  virtual void setCurrentFloor(byte floor) = 0;
  virtual int getCurrentPlayer() const = 0;
  virtual void setCurrentPlayer(int n) = 0;
  virtual int getPlayerOrder(unsigned n) const = 0;
  virtual void exchangePlayerOrder(unsigned n, unsigned m) = 0;
  virtual bool nextPlayerInOrder() = 0;
  virtual Location getCurrentLocation() const = 0;
  virtual void setCurrentLocation(Location loc) = 0;
  virtual byte getUnknown25EF() const = 0;
  virtual Direction getSavedDirection() const = 0;
  virtual void setSavedDirection(Direction direction) = 0;
  virtual DescriptorHandle getSavedRoomAddress() const = 0;
  virtual void setSavedRoomAddress(DescriptorHandle room) = 0;
  virtual byte getSavedActionKey() const = 0;
  virtual void setSavedActionKey(byte actionKey) = 0;
  virtual byte getSavedProgression() const = 0;
  virtual void setSavedProgression(byte progression) = 0;
  virtual void clearRoomItemPosition(RoomItem n) = 0;
  virtual void placeRoomItem(RoomItem n, byte y, byte x) = 0;
  virtual void placeRoomItemCenter(RoomItem n) = 0;
  virtual void copyRoomItemPosition(RoomItem n, RoomItem m) = 0;
  virtual byte getRoomItemRow(RoomItem n) const = 0;
  virtual byte getRoomItemColumn(RoomItem n) const = 0;
  virtual bool isRoomItemPlaced(RoomItem n) const = 0;
  virtual Utils::StringSpan getItemName(ItemCategory cat, byte id) const = 0;
  virtual Utils::StringSpan getItemTiles(ItemCategory cat, byte id) const = 0;
  virtual int8 getRangedWeaponAmmoType(unsigned id) const = 0;
  virtual Utils::StringSpan getRangedWeaponAmmoName(unsigned id) const = 0;
  virtual byte getMagicItemEffect(byte n) const = 0;
  virtual byte getMagicItemInitialUses(byte n) const = 0;
  virtual byte getPlayerColor(unsigned n) const = 0;
  virtual void setPlayerColor(unsigned n, unsigned c) = 0;
  virtual bool isQuestObjectValid(unsigned n) const = 0;
  virtual Utils::StringSpan getFloorColorTable(unsigned floor) const = 0;
  virtual Utils::StringSpan getColorTable() const = 0;
  virtual Utils::StringSpan getLivingStatueTiles() const = 0;
  virtual Utils::StringSpan getFountainTiles() const = 0;
  virtual Utils::StringSpan getChestTiles() const = 0;
  virtual Utils::StringSpan getMoneyTiles() const = 0;
  virtual byte getKeymapEntry(KeyMapping k) const = 0;
  virtual Utils::StringSpan getExtDictionaryWord(ExtDictionaryWord n) const = 0;
  virtual Utils::StringSpan getDictionaryWord(byte n) const = 0;
  virtual void getMagicEffectDescriptor(byte b, Base36Number (&effect)[3]) const = 0;
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name) = 0;
  virtual MapPosition getMapPosition() const = 0;
  virtual void setMapPosition(MapPosition pos) = 0;
  virtual byte getUnknown10FB() const = 0;
  virtual uint16 getPartyGold() const = 0;
  virtual void setPartyGold(uint16 n) = 0;
  virtual byte getMappedFloors() const = 0;
  virtual void setMappedFloors(byte n) = 0;
  virtual DescriptorHandle getRoomDescriptor(MapPosition pos) const = 0;
  virtual byte getRoomSpecialType(DescriptorHandle room) const = 0;
  virtual bool roomHasEnemies(DescriptorHandle room) const = 0;
  virtual void startCombat(DescriptorHandle room) = 0;
  virtual void clearRoomEnemies() = 0;
  virtual void prepareRoomEnemies(DescriptorHandle room) = 0;
  virtual void getRoomVaultParameters(DescriptorHandle room, byte &maxDigit, unsigned &numDigits) const = 0;
  virtual void clearRoomVault(DescriptorHandle room) = 0;
  virtual bool roomHasUnopenedChest(DescriptorHandle room) const = 0;
  virtual bool roomHasTrap(DescriptorHandle room) const = 0;
  virtual bool roomHasLivingStatue(DescriptorHandle room) const = 0;
  virtual bool roomHasFountain(DescriptorHandle room) const = 0;
  virtual void clearRoomChestAndTrap(DescriptorHandle room) = 0;
  virtual void setRoomVisited(DescriptorHandle room) = 0;
  virtual byte getRoomMoneyAmount(DescriptorHandle room) const = 0;
  virtual void setRoomMoneyAmount(DescriptorHandle room, byte n) = 0;
  virtual int getRoomNextLootSlot(DescriptorHandle room, unsigned &iterPos) const = 0;
  virtual void clearRoomLootSlot(DescriptorHandle room, unsigned iterPos, unsigned n) = 0;
  virtual bool dropItemInRoom(DescriptorHandle room, ItemCategory cat, byte id, byte itemStat, byte itemAmmo) = 0;
  virtual byte getRoomLootItem(DescriptorHandle room, unsigned n, ItemCategory &cat, byte &itemStat, byte &itemAmmo) = 0;
  virtual Utils::StringSpan getFloorMap() const = 0;
  virtual void setMapVisited(MapPosition pos, bool visited) = 0;
  virtual void prepareFloorMap(unsigned floor) = 0;
  virtual void restoreFloorVisitedMarkers() = 0;
  virtual bool inCombat() const = 0;
  virtual byte getNumEnemies() const = 0;
  virtual void clearCombat() = 0;
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
  void setRandomSource(RandomSource *randomSource_) { randomSource = randomSource_; }
protected:
  Database() : randomSource(nullptr) {}
  RandomSource *randomSource;
};

class DatabaseFactory {
public:
  virtual Database *load(File::ReadHandle &handle) = 0;
  virtual bool save(File::WriteHandle &handle, const Database &db) = 0;
  virtual ~DatabaseFactory() { }
};

}

#endif // TUNNELS_DATABASE_H_
