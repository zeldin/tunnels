#include "system.h"

#include "classic/ScreenEngine.h"
#include "classic/SequencePack.h"

#include <cstdio>
#include <cstdlib>

namespace Tunnels { namespace Classic {

namespace Vocab {

enum {
  A = 1, AT, ABOVE, ACCEPT, dummy1, ALL, ALLOWED, AND, ARE, ARROW,
  ASCEND, ATTACK, ATTEMPT, AVAILABLE, BACK, BAR, BATTLE, BE, BEEN,
  BEFORE, BEGIN, BELOW, BONUS, BREAK, BUY, BY, CANCEL, CANT, CASSETTE,
  CATEGORY, CHANCE, CHANGE, CHECK, CLASS, CLIMB, COLOR, COMBAT, COMMAND,
  COMPLETE, CONGRATULATIONS, CONSTRUCTION, CONTINUE, COST, CURRENT,
  CURSOR, DAMAGE, DATA, DECREASE, DEFENSE, DESCEND, DEVICE, DIFFERENT,
  DIFFICULTY, DIGGING, DIGITS, DIRECTION, DISK, WD, DONT, DOOR, DOWN,
  DROP, DROPPED, DUNGEON, EACH, EASIEST, ENOUGH, END, ENTER, ERROR,
  ESCAPE, SPEED, FAIL, FILENAME, FILE, FIND, FINISHED, FIRE, FLOOR,
  FOR, FORMATION, FOUND, FROM, FRONT, GAME, GIVE, GO, GOES, dummy2,
  HALL, HANDS, HARDEST, HAS, HAVE, HEAR, HERE, HIT, HOLD, HP, IF, IN,
  INCREASE, INSERT, IS, IT, ITEM, KEY, KILLED, LEAVE, LEFT, LISTEN,
  LOAD, LOOK, LOST, MAKE, MANY, MAP, MAXIMUM, MEDIUM, MISS, MOBILITY,
  MORE, MOVE, MOVEMENT, NAME, NEED, NEGOTIATION, NEW, NEXT, NO, NONE,
  FORWARD, NOT, NUMBER, OBJECT, OF, OLD, ON, ONE, OPEN, OPTION,
  OR, ORDER, OTHER, OUT, OVER, PER, PLEASE, POINT, POSITION, POSSESSION,
  PRESS, LATE, PROCD, PROGRAM, PROTECTION, PURCHASE, QUEST, WOUND,
  RATINGS, READY, REAR, REDO, REFUSE, REPORT, RESTOCK, RETURN, RIGHT,
  ROOM, SAME, SAVE, dummy3, SECOND, SECRET, SELECT, SELECTION, SHOW,
  SIDE, SOMETHING, SORRY, THAT, SPACE, SPECIAL, STAIR, START, STATUS,
  STOCKING, SUCCESSFULLY, SUMMARY, SWITCHES, TAKE, TARGET, RANGING,
  THE, THIS, THOSE, THROUGH, TO, TRADE, TRY, TUNNELS_OF_DOOM, TURN,
  TWO, UNAVAILABLE, UNDER, UNLIMITED, UNTRIED, UP, USE, USER, WAIT,
  WARNING, WANT, WHEN, WHICH, WHO, WHOM, WILL, WITH, WORKING, YOU,
  YOUR, PROTECTION2, RESISTANCE,

  cNEXTL, cJMP, cGPL, cSP, cLT, cGT, cCOLON, cPERIOD, cCOMMA, cDASH,
  cSLASH, cQUESTION, cEXCLAM, cLPAREN, cRPAREN, cNUM, cCURSOR,
  cHCHAR, cVCHAR, cCLEARL, cTEXT, cLIST, cFWD, cPLURAL, cED, cING,
  cINIT, cCLREOS, cBACK, cCALL, cEND
};

constexpr char words[] =
  "A" "AT" "ABOVE" "ACCEPT" "" "ALL" "ALLOWED" "AND" "ARE" "ARROW" "ASCEND"
  "ATTACK" "ATTEMPT" "AVAILABLE" "BACK" "BAR" "BATTLE" "BE" "BEEN" "BEFORE"
  "BEGIN" "BELOW" "BONUS" "BREAK" "BUY" "BY" "CANCEL" "CAN'T" "CASSETTE"
  "CATEGORY" "CHANCE" "CHANGE" "CHECK" "CLASS" "CLIMB" "COLOR" "COMBAT"
  "COMMAND" "COMPLETE" "CONGRATULATIONS" "CONSTRUCTION" "CONTINUE" "COST"
  "CURRENT" "CURSOR" "DAMAGE" "DATA" "DECREASE" "DEFENSE" "DESCEND" "DEVICE"
  "DIFFERENT" "DIFFICULTY" "DIGGING" "DIGITS" "DIRECTION" "DISK" "WD" "DON'T"
  "DOOR" "DOWN" "DROP" "DROPPED" "DUNGEON" "EACH" "EASIEST" "ENOUGH" "END"
  "ENTER" "ERROR" "ESCAPE" "SPEED" "FAIL" "FILENAME" "FILE" "FIND" "FINISHED"
  "FIRE" "FLOOR" "FOR" "FORMATION" "FOUND" "FROM" "FRONT" "GAME" "GIVE" "GO"
  "GOES" "" "HALL" "HANDS" "HARDEST" "HAS" "HAVE" "HEAR" "HERE" "HIT" "HOLD"
  "HP" "IF" "IN" "INCREASE" "INSERT" "IS" "IT" "ITEM" "KEY" "KILLED" "LEAVE"
  "LEFT" "LISTEN" "LOAD" "LOOK" "LOST" "MAKE" "MANY" "MAP" "MAXIMUM" "MEDIUM"
  "MISS" "MOBILITY" "MORE" "MOVE" "MOVEMENT" "NAME" "NEED" "NEGOTIATION"
  "NEW" "NEXT" "NO" "NONE" "FORWARD" "NOT" "NUMBER" "OBJECT" "OF" "OLD"
  "ON" "ONE" "OPEN" "OPTION" "OR" "ORDER" "OTHER" "OUT" "OVER" "PER" "PLEASE"
  "POINT" "POSITION" "POSSESSION" "PRESS" "LATE" "PROC'D" "PROGRAM"
  "PROTECTION" "PURCHASE" "QUEST" "WOUND" "RATINGS" "READY" "REAR" "REDO"
  "REFUSE" "REPORT" "RESTOCK" "RETURN" "RIGHT" "ROOM" "SAME" "SAVE" ""
  "SECOND" "SECRET" "SELECT" "SELECTION" "SHOW" "SIDE" "SOMETHING" "SORRY"
  "THAT" "SPACE" "SPECIAL" "STAIR" "START" "STATUS" "STOCKING" "SUCCESSFULLY"
  "SUMMARY" "SWITCHES" "TAKE" "TARGET" "RANGING" "THE" "THIS" "THOSE"
  "THROUGH" "TO" "TRADE" "TRY" "TUNNELS OF DOOM" "TURN" "TWO" "UNAVAILABLE"
  "UNDER" "UNLIMITED" "UNTRIED" "UP" "USE" "USER" "WAIT" "WARNING" "WANT"
  "WHEN" "WHICH" "WHO" "WHOM" "WILL" "WITH" "WORKING" "YOU" "YOUR"
  "PROTECTION" "RESISTANCE";
constexpr unsigned short wordIndex[] = {
  0, 1, 3, 8, 14, 14, 17, 24, 27, 30, 35, 41, 47, 54, 63, 67, 70, 76, 78,
  82, 88, 93, 98, 103, 108, 111, 113, 119, 124, 132, 140, 146, 152, 157,
  162, 167, 172, 178, 185, 193, 208, 220, 228, 232, 239, 245, 251, 255,
  263, 270, 277, 283, 292, 302, 309, 315, 324, 328, 330, 335, 339, 343,
  347, 354, 361, 365, 372, 378, 381, 386, 391, 397, 402, 406, 414, 418,
  422, 430, 434, 439, 442, 451, 456, 460, 465, 469, 473, 475, 479, 479,
  483, 488, 495, 498, 502, 506, 510, 513, 517, 519, 521, 523, 531, 537,
  539, 541, 545, 548, 554, 559, 563, 569, 573, 577, 581, 585, 589, 592,
  599, 605, 609, 617, 621, 625, 633, 637, 641, 652, 655, 659, 661, 665,
  672, 675, 681, 687, 689, 692, 694, 697, 701, 707, 709, 714, 719, 722,
  726, 729, 735, 740, 748, 758, 763, 767, 773, 780, 790, 798, 803, 808,
  815, 820, 824, 828, 834, 840, 847, 853, 858, 862, 866, 870, 870, 876,
  882, 888, 897, 901, 905, 914, 919, 923, 928, 935, 940, 945, 951, 959,
  971, 978, 986, 990, 996, 1003, 1006, 1010, 1015, 1022, 1024, 1029, 1032,
  1047, 1051, 1054, 1065, 1070, 1079, 1086, 1088, 1091, 1095, 1099, 1106,
  1110, 1114, 1119, 1122, 1126, 1130, 1134, 1141, 1144, 1148, 1158, 1168,
};


constexpr SequencePack::UniformSequence<0, byte> prompt() {
  return SequencePack::UniformSequence<0, byte>{};
}
template <typename ... U>
constexpr SequencePack::UniformSequence<1+sizeof...(U), byte>
prompt(byte c, U ... rest)
{
  return SequencePack::UniformSequence<1+sizeof...(U), byte>{c, prompt(rest...)};
}

constexpr const auto prompts = SequencePack::index
  (SequencePack::sequence
   (prompt(cCURSOR, 0, 0, cCLREOS, CHANGE, cEND), // 00
    prompt(cCURSOR, -1, 2, cCLREOS, cEND), // 01
    prompt(cFWD, 2, PRESS, BACK, WHEN, FINISHED, cEND), // 02
    prompt(cFWD, 3, PRESS, cSP, cLT, OR, cSP, cGT, FOR, MORE, cEND), // 03
    prompt(cCURSOR, 0, 9, cCLEARL, ' ', COMMAND, SUMMARY, cEND), // 04
    prompt(PRESS, PROCD, FOR, NEXT, cTEXT, -1, cEND), // 05
    prompt(cFWD, 3, PRESS, REDO, TO, CHANGE, cEND), // 06
    prompt(cFWD, 2, PRESS, PROCD, TO, CONTINUE, cEND), // 07
    prompt(cFWD, 2, SELECT, ONE, OF, THE, ABOVE, cEND), // 08
    prompt(cCLEARL, '_', cEND), // 09
    prompt(cCLEARL, ' ', cEND), // 0A
    prompt(), // 0B
    prompt(cFWD, 1, PRESS, PROCD, WHEN, FINISHED, cEND), // 0C
    prompt(cCURSOR, 22, 2, cCLREOS, cNEXTL, cCLEARL, '_', // 0D
	   cCURSOR, -1, 12, cSP, cBACK, WORKING, cEND),
    prompt(cINIT, cEND), // 0E
    prompt(cRPAREN, cQUESTION, cSP, cEND), // 0F
    prompt(cCURSOR, 0, 9, cVCHAR, 0xa0, TUNNELS_OF_DOOM, cNEXTL, // 10
	   cCLEARL, '_', cEND),
    prompt(cINIT, cCURSOR, 22, 2, cCALL, 0x08, cCURSOR, 3, 3, LOAD, // 11
	   DATA, FROM, cCOLON, cFWD, 49, cLIST, '1', '3', '-'),
    prompt(CASSETTE, cFWD, 23, DISK, cNUM, 1, cFWD, 25, OTHER, // 12
	   cCURSOR, 5, 4, cEND),
    prompt(cCURSOR, 22, 2, cCALL, 0x08, cNEXTL, cCALL, 0x0c, // 13
	   cCURSOR, 9, 3, SAVE, DATA, TO, cCOLON, cFWD, 51,
	   cLIST, '4', '6', '-', cJMP, 0x12),
    prompt(cCURSOR, 15, 3, cCLREOS, FILENAME, cCOLON, cEND), // 14
    prompt(cCURSOR, 15, 3, cCLREOS, DEVICE, NAME, cCOLON, cNEXTL, cEND), // 15
    prompt(cCURSOR, 22, 2), // 16
    prompt(cCLEARL, ' ', cCALL, 0x06, cNEXTL, cCLEARL, ' ', cJMP, 0x07), // 17
    prompt(cCURSOR, 21, 3, cCLEARL, ' ', THIS, WILL, TAKE, cNUM, 200, // 18
	   SECOND, cPLURAL, cEND),
    prompt(cCURSOR, 18, 3, cCLREOS, YOU, ARE, TO, cHCHAR, 'O', 1, // 19
	   LATE, TO, SAVE, cNEXTL, cSP, THE, cGPL, 0xf1, 0x93),
    prompt(cINIT, cCURSOR, 5, 9, GAME, SELECTION, cCOLON, cFWD, 43, // 1A
	   cNUM, 1, cDASH, NEW, DUNGEON, cFWD, 49, cNUM, 2, cDASH,
	   RESTOCK, OLD, DUNGEON, cCURSOR, 7, 4, cEND),
    prompt(cCURSOR, 11, 4, cNUM, 3, cDASH, CONTINUE, CURRENT, // 1B
	   GAME, cCURSOR, 5, 4, cEND),
    prompt(cCURSOR, 7, 3, DIFFICULTY, cCOLON, cFWD, 21, // 1C
	   cLIST, '1', '3', '-', EASIEST, cFWD, 24, MEDIUM, cFWD, 25,
	   HARDEST, cCURSOR, 23, 2, cJMP, 0x08),
    prompt(cCURSOR, 3, 2, NUMBER, OF, FLOOR, cPLURAL, cLPAREN, // 1D
	   cHCHAR, '0', 1, cDASH, cCALL, 0x20, cGPL, 0xf1, 0xa6),
    prompt(cCURSOR, 5, 2, cCLREOS, NUMBER, IN, cTEXT, -2, // 1E
	   cCALL, 0x1f, cBACK, cGPL, 0xf1, 0xbc),
    prompt(cLPAREN, cHCHAR, '1', 1, cDASH, cHCHAR, '0', 1, // 1F
	   cRPAREN, cQUESTION),
    prompt(cBACK, cBACK, cBACK, cBACK, cEND), // 20
    prompt(cINIT, cCURSOR, 3, 9, cTEXT, -1, cSP, cHCHAR, '0', 1, // 21
	   cCURSOR, 5, 9, NAME, cQUESTION, cFWD, 58, CLASS, cCALL, 0x1f,
	   cFWD, 55, COLOR, cCALL, 0x1f, cHCHAR, '4', 1, cFWD, 110,
	   cCLEARL, '_', cFWD, 37, CLASS, cPLURAL, cFWD, 6,
	   COLOR, cPLURAL, cGPL, 0xf1, 0xc5),
    prompt(cCURSOR, 12, 4, SELECT, A, DIFFERENT, COLOR, cPERIOD, cEND), // 22
    prompt(cINIT, cCURSOR, 3, 7, POINT, OF, NO, RETURN, cFWD, 40, // 23
	   CHECK, THE, DATA, BELOW, BEFORE, cFWD, 13, CONTINUE, cING,
	   cFWD, 34, cCLEARL, '_', cCURSOR, 22, 3, cCALL, 0x06, cNEXTL,
	   cCALL, 0x07, cGPL, 0xf2, 0x5b),
    prompt(cCURSOR, 2, 11, cCLREOS, PLEASE, WAIT, cFWD, 12, DUNGEON, // 24
	   UNDER, CONSTRUCTION, cCURSOR, 8, 0, cTEXT, 0x06, 0x00, 192,
	   cCURSOR, 14, 0, cTEXT, 0x06, 0xc0, 192, cEND),
    prompt(cCURSOR, 5, 8, cCLEARL, ' ', DIGGING, FLOOR, cNEXTL, // 25
	   cCLEARL, ' ', cCURSOR, 5, -1, cGPL, 0xf2, 0x9f),
    prompt(cCURSOR, 6, 8, cCLEARL, ' ', STOCKING, FLOOR, // 26
	   cGPL, 0xf2, 0x9f),
    prompt(cINIT, cCALL, 0x04, cCURSOR, 22, 2, cCALL, 0x03, cNEXTL, // 27
	   cCALL, 0x02, cCURSOR, 3, 3, cEND),
    prompt(cCURSOR, 5, 18, cCLEARL, ' ', DESCEND, cING, cNEXTL, // 28
	   cCLEARL, ' ', cEND),
    prompt(cVCHAR, 0xdd, cCURSOR, 1, 23, THIS, cFWD, 27, GAME, // 29
	   cFWD, 28, IS, cFWD, 28, OVER, cEXCLAM, cCURSOR, 21, 2,
	   cCLEARL, '_', cNEXTL, STATUS, REPORT, cPLURAL, ARE, AVAILABLE,
	   cNEXTL, cFWD, 1, PRESS, BEGIN, FOR, A, NEW, GAME, cGPL, 0xf2, 0xaa),
    prompt(cCURSOR, 5, 21, cHCHAR, '_', 10, cFWD, 22, COMPLETE, cED, // 2A
	   cFWD, 22, QUEST, cPLURAL, cCOLON, cCURSOR, 17, 21,
	   cHCHAR, '_', 10, cEND),
    prompt(cCURSOR, 18, 4, CONGRATULATIONS, cEXCLAM, YOU, HAVE, cNEXTL, // 2B
	   cFWD, 3, SUCCESSFULLY, COMPLETE, cED, cNEXTL,
	   cTEXT, 0x06, 0x02, 28, cEND),
    prompt(cCURSOR, 19, 2, THE, cTEXT, -2, HAS, BEEN, KILLED, cNEXTL, // 2C
	   BY, THE, cTEXT, 0x02, 0x44, 14, cEXCLAM, cEND),
    prompt(cCURSOR, 21, 3, cCLREOS, YOU, NEED, A, MAP, TO, // 2D
	   DESCEND, cEXCLAM, cEND),
    prompt(cCURSOR, 20, 2, cCLREOS, cCURSOR, 23, 5, SELECT, MOVEMENT, // 2E
	   OPTION, cEND),
    prompt(cCURSOR, 19, 4, ASCEND, cING, cEND), // 2F
    prompt(cCURSOR, 19, 4, DESCEND, cING, cEND), // 30
    prompt(cCURSOR, 18, 20, SPEED, cCOLON, cFWD, 40, cCLREOS, YOUR, // 31
	   TURN, cCOMMA, cGPL, 0xf3, 0x36),
    prompt(cCURSOR, 21, 3, YOU, ARE, USE, cING, YOUR, HANDS, cEND), // 32
    prompt(cCURSOR, 20, 3, cCLREOS, THE, cTEXT, 0x02, 0x44, 14, // 33
	   ATTACK, cGPL, 0xf3, 0xab),
    prompt(cPLURAL), // 34
    prompt(cEXCLAM, cEND), // 35
    prompt(cCURSOR, 21, 3, YOU, HIT, THE, cTEXT, 0x11, 0x26, 12, // 36
	   cEXCLAM, cEND),
    prompt(cCURSOR, 21, 3, YOU, MISS, cED, cEXCLAM, cEND), // 37
    prompt(cCURSOR, 18, 2, END, OF, THE, BATTLE, cEND), // 38
    prompt(cCURSOR, 20, 2, YOU, ALL, NEED, TO, LEAVE, BY, THE, cNEXTL, // 39
	   SAME, DOOR, cEXCLAM, COMBAT, GOES, ON, cEXCLAM, cEND),
    prompt(cCURSOR, 20, 3, YOU, HAVE, ESCAPE, cED, FROM, THE, // 3A
	   cFWD, 6, cTEXT, 0x02, 0x44, 12, cEXCLAM, cEND),
    prompt(cCURSOR, 23, 2, cCALL, 0x02, cCURSOR, 15, 2, BUY, FOR, // 3B
	   cQUESTION, cEND),
    prompt(cCURSOR, 16, 4, NO, ITEM, cPLURAL, AVAILABLE, HERE, cEND), // 3C
    prompt(cINIT, cCURSOR, 0, 8, cCLEARL, ' ', cTEXT, 0x25, 0x94, 16, // 3D
	   cCURSOR, 2, 20, COST, cCOLON, cCURSOR, 12, 2, cCLEARL, '_', cNEXTL,
	   cTEXT, 0x32, 0x70, 16, cCOLON, cNEXTL, cCLEARL, '_',
	   cGPL, 0xf3, 0xbe),
    prompt(cCURSOR, 4, 4, cNUM, 1, cTEXT, -6, cPLURAL, cCALL, 0x64, // 3E
	   cNUM, 2, cTEXT, -5, cTEXT, -6, cPLURAL, cCALL, 0x64,
	   cNUM, 3, cTEXT, -5, cTEXT, -6, cPLURAL, cCALL, 0x64,
	   cNUM, 4, cTEXT, -7, cCALL, 0x64,
	   cNUM, 5, cTEXT, -8, cPLURAL, cCALL, 0x64,
	   cNUM, 6, cTEXT, -11, cCALL, 0x64,
	   cNUM, 7, cTEXT, -12, cCALL, 0x3f,
	   cCURSOR, 23, 2, cCLREOS, cCALL, 0x0c, cGPL, 0xf3, 0xd0),
    prompt(cCURSOR, 16, 2, BUY, WHICH, NUMBER, cQUESTION, cEND), // 3F
    prompt(cCURSOR, -1, 5, NO, ITEM, cEND), // 40
    prompt(cCURSOR, 22, 2, ALLOWED, ITEM, cPLURAL, ARE, ARROW, cED, cEND, // 41
	   cJMP, 0x3f),
    prompt(cCURSOR, 18, 4, cCLREOS, YOU, DONT, HAVE, ENOUGH, cNEXTL, // 42
	   cFWD, 2, cTEXT, 0x32, 0x70, 16, cEXCLAM, cEND),
    prompt(cCURSOR, 18, 6, cCLREOS, YOU, CANT, USE, THOSE, cEND), // 43
    prompt(cCURSOR, 18, 3, cCLREOS, SORRY, cCOMMA, NO, MORE, ARE, // 44
	   ALLOWED, cEND),
    prompt(cCURSOR, 18, 11, cCLREOS, PURCHASE, cED, cEND), // 45
    prompt(cCURSOR, 7, 4, cLIST, '1', '9', ' ', cCURSOR, 16, 3, // 46
	   cNUM, 10, cEND),
    prompt(cINIT, cCURSOR, 18, 13, WHICH, NUMBER, cQUESTION, cEND), // 47
    prompt(cCURSOR, 3, 3, YOU, CANT, HOLD, ALL, OF, THIS, cEXCLAM, // 48
	   cNEXTL, WHICH, ITEM, WILL, YOU, GIVE, UP, cQUESTION,
	   cFWD, 37, cNUM, 0, cGPL, 0xf4, 0x29),
    prompt(cCURSOR, 3, 13, TRADE, cING, cFWD, 47, // 49
	   cNUM, 1, cTEXT, -7, cCALL, 0x64,
	   cNUM, 2, cTEXT, -8, cPLURAL, cCALL, 0x64,
	   cNUM, 3, cTEXT, -6, cPLURAL, cCALL, 0x64,
	   cNUM, 4, cCURSOR, 17, 2, cCLEARL, ' ',
	   WHO, START, cPLURAL, cQUESTION, cGPL, 0xf4, 0x3b),
    prompt(cCURSOR, 19, 2, cCLEARL, ' ', GIVE, TO, cQUESTION, // 4A
	   cCURSOR, 22, 3, cCLREOS, IF, NO, NAME, IS, ENTER, cED, cCOMMA,
	   THE, cNEXTL, cFWD, 4, ITEM, WILL, BE, DROPPED, cPERIOD, cEND),
    prompt(cCURSOR, 21, 3, cCLREOS, WARNING, cCOLON, ITEM, WILL, BE, // 4B
	   LOST, cNEXTL, cCALL, 0x06, cNEXTL, cJMP, 0x07),
    prompt(cCURSOR, 22, 2, USE, ON, WHOM, cQUESTION, cEND), // 4C
    prompt(cCURSOR, 19, 2, cTEXT, -10, FOUND, cEND), // 4D
    prompt(cCURSOR, 19, 2, cCLREOS, THE, cTEXT, -10, FAIL, cED, // 4E
	   cEXCLAM, cEND),
    prompt(cCURSOR, 19, 2, NO, cTEXT, -10, cPLURAL, FOUND, cEND), // 4F
    prompt(INCREASE, cPLURAL, cEND), // 50
    prompt(DECREASE, cPLURAL, cEND), // 51
    prompt(SHOW, cPLURAL, cTEXT, -9, cPLURAL, IN, ROOM, cEND), // 52
    prompt(FIND, cPLURAL, cTEXT, -10, cPLURAL, cEND), // 53
    prompt(MAKE, cPLURAL, cTEXT, -10, cPLURAL, MISS, cBACK, cBACK, // 54
	   FIRE, cEND),
    prompt(cCURSOR, -1, 2, cCLREOS, USER, cQUESTION, cEND), // 55
    prompt(cCURSOR, 17, 2, cCLREOS, USE, WHICH, ITEM, cQUESTION, cEND), // 56
    prompt(cCURSOR, 23, 4, THE, ITEM, HAS, NO, USE, HERE, cPERIOD, cEND), // 57
    prompt(cCURSOR, 19, 2, WHO, WILL, OPEN, THE, cNEXTL, // 58
	   cTEXT, 0x34, 0xa0, 12, cQUESTION, cEND),
    prompt(cCURSOR, 18, 4, YOU, HAVE, FOUND, cCALL, 0x64, // 59
	   cGPL, 0xf4, 0x4a),
    prompt(), // 5A
    prompt(cCURSOR, 19, 2, HERE, IS, A, MAP, OF, THIS, FLOOR, cEND), // 5B
    prompt(cCURSOR, 18, 2, A, QUEST, IS, COMPLETE, cED, cEXCLAM, YOU, // 5C
	   cNEXTL, HAVE, FOUND, THE, cGPL, 0xf4, 0x63),
    prompt(NONE, cEND), // 5D
    prompt(NONE, cSP, cLPAREN, cBACK, HANDS, cRPAREN, cEND), // 5E
    prompt(UNLIMITED, cGPL, 0xf4, 0x78), // 5F
    prompt(ONE, USE, PER, BATTLE, cEND), // 60
    prompt(LEFT, cEND), // 61
    prompt(cCURSOR, 20, 12, FLOOR, cCOLON, cGPL, 0xf4, 0x8a), // 62
    prompt(cNEXTL), // 63
    prompt(cNEXTL, cFWD, 1, cEND), // 64
    prompt(cCURSOR, 3, 6, HALL, AND, ROOM, MOVEMENT, // 65
	   cCALL, 0x63, cHCHAR, 'E', 1, cDASH, MOVE, FORWARD,
	   cCALL, 0x64, cHCHAR, 'S', 1, cDASH, MOVE, LEFT,
	   cCALL, 0x64, cHCHAR, 'D', 1, cDASH, MOVE, RIGHT,
	   cCALL, 0x64, cHCHAR, 'X', 1, cDASH, MOVE, BACK,
	   cCALL, 0x66, OTHER, COMMAND, cPLURAL,
	   cCALL, 0x63, cLT, cDASH, ASCEND, STAIR, cPLURAL,
	   cCALL, 0x64, cGT, cDASH, DESCEND, STAIR, cPLURAL,
	   cCALL, 0x64, cHCHAR, 'K', 1, cDASH, SAVE, A, GAME, cEND),
    prompt(cNEXTL, cNEXTL, cFWD, 3, cEND), // 66
    prompt(cCURSOR, 3, 6, REPORT, cPLURAL, // 67
	   cCALL, 0x63, cBACK, cNUM, 1, cDASH, cTEXT, -1, STATUS,
	   cCALL, 0x64, cBACK, cNUM, 2, cDASH, cTEXT, -2, STATUS,
	   cCALL, 0x64, cBACK, cNUM, 3, cDASH, cTEXT, -9, REPORT,
	   cCALL, 0x66, HALL, OPTION, cPLURAL,
	   cCALL, 0x63, cHCHAR, 'B', 1, cDASH, BREAK, THROUGH, DOOR,
	   cCALL, 0x64, cHCHAR, 'C', 1, cDASH, CHECK, FOR, SECRET, DOOR, cPLURAL,
	   cCALL, 0x64, cHCHAR, 'L', 1, cDASH, LISTEN, AT, A, DOOR,
	   cCALL, 0x64, cHCHAR, 'M', 1, cDASH, LOOK, AT, THE, MAP,
	   cCALL, 0x64, cHCHAR, 'O', 1, cDASH, CHANGE, cTEXT, -2, ORDER,
	   cCALL, 0x64, cHCHAR, 'T', 1, cDASH, TRADE, ITEM, cPLURAL,
	   cCALL, 0x64, cHCHAR, 'U', 1, cDASH, USE, ITEM, cEND),
    prompt(cCURSOR, 3, 6, COMBAT, OPTION, cPLURAL, // 68
	   cCALL, 0x63, cHCHAR, 'F', 1, cDASH, FIRE, A, cTEXT, -5, cTEXT, -6,
	   cCALL, 0x64, cHCHAR, 'N', 1, cDASH, ATTEMPT, NEGOTIATION, cPLURAL,
	   cCALL, 0x64, cHCHAR, 'U', 1, cDASH, USE, ITEM,
	   cCALL, 0x64, cHCHAR, 'W', 1, cDASH, CHANGE, cTEXT, -6, cPLURAL,
	   cCALL, 0x64, cBACK, ENTER, cDASH, END, OF, TURN, cEND),
    prompt(cCOLON, cNEXTL, cFWD, 1, cEND), // 69
    prompt(cINIT, cCURSOR, 2, 6, cHCHAR, 0xf0, 1, cHCHAR, 0xf2, 1, // 6A
	   cFWD, 18, cHCHAR, 0xf8, 1, cHCHAR, 0xfa, 1, cFWD, 10,
	   cHCHAR, 0xf1, 1, cHCHAR, 0xf3, 1, cFWD, 18, cHCHAR, 0xf9, 1,
	   cHCHAR, 0xfb, 1, cFWD, 40, MAXIMUM, HIT, POINT, cPLURAL,
	   cCALL, 0x69, cTEXT, -3, cCOLON,
	   cCALL, 0x63, cTEXT, -9, RATINGS,
	   cCALL, 0x69, ATTACK,
	   cCALL, 0x69, MAXIMUM, DAMAGE,
	   cCALL, 0x69, DEFENSE, cCOLON,
	   cCALL, 0x63, RESISTANCE,
	   cCALL, 0x69, SPEED, cCOLON, cFWD, 5, MOBILITY,
	   cCALL, 0x69, NEGOTIATION, cCOLON, cNEXTL, cNEXTL,
	   SPECIAL, ATTACK, CHANCE, cCOLON, cCURSOR, 23, 2,
	   cCALL, 0x02, cGPL, 0xf4, 0xd0),
    prompt(cCURSOR, 4, 13, STATUS, cCURSOR, 7, 8, HIT, POINT, // 6B
	   cPLURAL, cCOLON, cFWD, 24, WOUND, cPLURAL, cCOLON, cFWD, 20,
	   cTEXT, -3, cCOLON, cFWD, 25, cTEXT, -4, cCOLON, cFWD, 47,
	   cTEXT, -6, cPLURAL, cCURSOR, -1, 20, BONUS, cCOLON, cFWD, 24,
	   DAMAGE, cCOLON, cFWD, 56, DAMAGE, cCOLON, cFWD, 78,
	   cTEXT, -7, cCOLON, cFWD, 24, cTEXT, -8, cCOLON, cFWD, 20,
	   PROTECTION, cCOLON, cGPL, 0xf5, 0x81),
    prompt(cCURSOR, 4, 8, cTEXT, 0x32, 0x80, 16, cCURSOR, 7, 7, // 6C
	   cLIST, '1', '9', ':', cCURSOR, 16, 6, cNUM, 10, cCALL, 0x69,
	   DATA, ON, WHICH, ITEM, cQUESTION, cGPL, 0xf6, 0x20),
    prompt(cINIT, cCURSOR, 2, 10, QUEST, STATUS, cNEXTL, cNEXTL, // 6D
	   cTEXT, 0x32, 0x70, 16, cCOLON, cNEXTL, cTEXT, -11, cPLURAL, cCOLON,
	   cCURSOR, 7, 3, QUEST, OBJECT, cPLURAL, cCALL, 0x69,
	   cLIST, '1', '8', ':', cCURSOR, 16, 4, cHCHAR, '*', 1, TURN,
	   cPLURAL, LEFT, TO, FIND, cCURSOR, 20, 2, HIT, POINT, cPLURAL,
	   cNEXTL, DAMAGE, cNEXTL, cNEXTL, cCALL, 0x02, cGPL, 0xf6, 0x5d),
    prompt(FILE, ERROR, cEND), // 6E
    prompt(DEVICE, ERROR, cEND), // 6F
    prompt(PROGRAM, ERROR, cEND), // 70
    prompt(OUT, OF, SPACE, cEND), // 71
    prompt(FILE, NOT, FOUND, cEND), // 72
    prompt(cCURSOR, 23, 3, cTEXT, -6, NOT, cTEXT, -5, cEND), // 73
    prompt(cCURSOR, 23, 3, YOU, HAVE, USE, cED, IT, UP, cEXCLAM, cEND), // 74
    prompt(cCURSOR, 23, 3, cTEXT, -6, UNAVAILABLE, cEND), // 75
    prompt(cCURSOR, 23, 2, YOU, CANT, FIRE, FROM, HERE, cEND), // 76
    prompt(cCURSOR, 21, 3, POSITION, CURSOR, ON, TARGET, cNEXTL, PRESS, // 77
	   THE, SPACE, BAR, TO, FIRE, cNEXTL, PRESS, BACK, TO, CANCEL,
	   FIRE, cING, cEND),
    prompt(cCURSOR, 21, 3, YOU, HEAR, SOMETHING, // 78
	   cPERIOD, cPERIOD, cPERIOD, cEND),
    prompt(cCURSOR, 20, 2, NEGOTIATION, FAIL, cED, cEND), // 79
    prompt(cCURSOR, 23, 4, YOU, DONT, HAVE, THAT, MANY, cEND), // 7A
    prompt(cCURSOR, 22, 5, PRESS, PROCD, TO, ACCEPT, cFWD, 10, // 7B
	   PRESS, REDO, TO, REFUSE, cEND),
    prompt(cCURSOR, 3, 10, CHANGE, ORDER, cFWD, 55, FRONT, // 7C
	   cFWD, 125, cHCHAR, 'E', 1, cFWD, 61, cHCHAR, 'S', 1,
	   cFWD, 3, cHCHAR, 'D', 1, cFWD, 61, cHCHAR, 'X', 1,
	   cFWD, 92, REAR, cCURSOR, 20, 2, EACH, DIRECTION, KEY, SWITCHES,
	   cNEXTL, cFWD, 4, cTEXT, -1, cPLURAL, ON, THAT, SIDE,
	   cNEXTL, cNEXTL, cJMP, 0x02),
    prompt(cCURSOR, 19, 2, cCLREOS, THE, cTEXT, 0x02, 0x44, 14, WANT, // 7D
	   cNEXTL, cTEXT, 0x32, 0x70, 16, cGPL, 0xf7, 0x87),
    prompt(cCURSOR, 18, 2, WHO, WILL, TRY, TO, cTEXT, 0x2d, 0xbe, 8, // 7E
	   cNEXTL, THE, cTEXT, 0x34, 0xac, 12, cQUESTION, cCURSOR, 22, 2,
	   THE, cTEXT, 0x11, 0x62, 16, HAS, cNEXTL, DIGITS, RANGING,
	   FROM, cSP, cHCHAR, '1', 1, TO, cGPL, 0xf7, 0xd9),
    prompt(cCURSOR, 20, 2, cCLREOS, HP, cCOLON, cFWD, 7, WD, cCOLON, // 7F
	   cCURSOR, 22, 2, REDO, CHANGE, cPLURAL, cTEXT, -1,
	   cCURSOR, 23, 2, PRESS, BACK, TO, GIVE, UP,
	   cCURSOR, 18, 2, cTEXT, 0x11, 0x62, 16, cQUESTION, cEND),
    prompt(cCURSOR, 22, 2, cCLREOS, cTEXT, 0x25, 0x78, 28, cEND), // 80
    prompt(READY, cEND), // 81
    prompt(cCURSOR, 18, 0, cCLEARL, ' ', cEND), // 82
    prompt(cLPAREN, cBACK, cTEXT, -5, cRPAREN, cEND), // 83
    prompt(), // 84
    prompt(UNTRIED, cEND), // 85
    prompt(cCURSOR, 20, 2, cCLREOS, WHO, WILL, USE, THE, // 86
	   cNEXTL, cTEXT, 0x32, 0x50, 16, cQUESTION, cEND),
    prompt(cCURSOR, 18, 2, cCLREOS, NUMBER, OF, cTEXT, 0x32, 0x70, 16, // 87
	   cNEXTL, TO, GIVE, cQUESTION, cEND),
    prompt(cCURSOR, 21, 2, ITEM, LOST, cEXCLAM, TRY, GIVE, cING, // 88
	   MORE, cPERIOD, cEND),
    prompt(cCURSOR, 21, 2, NO, IN, cBACK, FORMATION, cPERIOD, // 89
	   GIVE, MORE, cPERIOD, cEND),
    prompt(cCURSOR, 22, 2, cCLREOS, YOU, CANT, USE, THAT, ITEM, // 8A
	   cEXCLAM, cEND)
    ));

}

void ScreenEngine::initScreen()
{
  screen.all(' ');
  screen.loadRomFont();
  screen.setBackground(6);
  static constexpr byte up[][VDP::PATTERN_H] {
    { 0x00, 0x10, 0x38, 0x54, 0x10, 0x10, 0x10, 0x10 }
  };
  screen.loadPatterns(60, up);
  static constexpr byte down[][VDP::PATTERN_H] {
    { 0x00, 0x10, 0x10, 0x10, 0x10, 0x54, 0x38, 0x10 }
  };
  screen.loadPatterns(62, down);
  static constexpr byte patterns[][VDP::PATTERN_H] {
    { 0x18, 0x24, 0x5a, 0xa1, 0xa1, 0x5a, 0x24, 0x18 },
    { 0x00, 0x00, 0x10, 0x08, 0x7c, 0x08, 0x10, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x7c },
    { 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x82, 0x7c },
    { 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00 }
  };
  screen.loadPatterns(91, patterns);
  static constexpr byte color_table[] {
    0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b,
    0x1b, 0x00, 0x00, 0xcc, 0x44, 0xdd, 0x66,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x4b, 0x1b, 0x1b
  };
  screen.loadColorTable(4, color_table);
  drawPrompt(0x10);
}

unsigned ScreenEngine::putDigit(unsigned y, unsigned x, unsigned d)
{
  if (!d) {
    byte c = screen.gchar(y, x-1);
    if (c != '.' && (c < '0' || c > '9'))
      return x;
  }
  screen.hchar(y, x++, '0'+d, 1);
  return x;
}

void ScreenEngine::drawPrompt(unsigned n)
{
  const byte *ptr = Vocab::prompts.entry(n);
  byte c;
  while ((c = *ptr++) != Vocab::cEND)
    if (c < Vocab::cNEXTL) {
      if (c) {
	unsigned o = Vocab::wordIndex[c-1];
	unsigned l = Vocab::wordIndex[c] - o;
	unsigned x = screen.getXpt();
	unsigned y = screen.getYpt();
	screen.hstr(y, x, reinterpret_cast<const byte *>(&Vocab::words[o]), l);
	if ((x += l) < 30)
	  screen.hchar(y, x++, ' ', 1);
	else
	  x = 31;
	screen.setXpt(x);
      }
    } else if (c < Vocab::cNUM) {
      if (c >= Vocab::cSP) {
	screen.hchar(screen.getYpt(), screen.getXpt()-1,
		     " <>:.,-/?!()"[c-Vocab::cSP], 1);
	screen.hchar(screen.getYpt(), screen.getXpt(), ' ', 1);
	screen.setXpt(screen.getXpt()+1);
      } else switch(c) {
	case Vocab::cNEXTL:
	  screen.setYpt(screen.getYpt()+1);
	  screen.setXpt(2);
	  break;
	case Vocab::cJMP:
	case Vocab::cGPL:
	  printf("Bad C %02x\n", (unsigned)c);
	  abort();
      }
    } else switch(c) {
      case Vocab::cNUM:
	{
	  unsigned n = *ptr++;
	  unsigned x = screen.getXpt();
	  unsigned y = screen.getYpt();
	  screen.hchar(y, x, ' ', 3);
	  unsigned d = n/100;
	  n %= 100;
	  x = putDigit(y, x, d);
	  d = n/10;
	  n %= 10;
	  x = putDigit(y, x, d);
	  screen.hchar(y, x++, '0'+n, 1);
	  screen.setXpt(++x);
	}
	break;
      case Vocab::cCURSOR:
	screen.setYpt(*ptr++);
	screen.setXpt(*ptr++);
	break;
      case Vocab::cHCHAR:
	{
	  byte c = *ptr++;
	  byte n = *ptr++;
	  screen.hchar(screen.getYpt(), screen.getXpt()-1, c, n);
	  screen.setXpt(screen.getXpt()+n);
	}
	break;
      case Vocab::cVCHAR:
	{
	  byte c = *ptr++;
	  screen.vchar(0, 0, c, 2*VDP::ROWS);
	  screen.vchar(0, VDP::COLUMNS-2, c, 2*VDP::ROWS);
	}
	break;
      case Vocab::cCLEARL:
	screen.hchar(screen.getYpt(), 2, *ptr++, 28);
	break;
      case Vocab::cTEXT:
	printf("Bad C %02x\n", (unsigned)c);
	abort();
      case Vocab::cLIST:
	{
	  byte a = *ptr++;
	  byte b = *ptr++;
	  byte c = *ptr++;
	  unsigned x = screen.getXpt();
	  unsigned y = screen.getYpt();
	  do {
	    screen.hchar(y, x, a, 1);
	    screen.hchar(y, x+1, c, 1);
	    y++;
	  } while(++a <= b);
	  x += 2;
	  if (x < 30)
	    screen.hchar(screen.getYpt(), x++, ' ', 1);
	  screen.setXpt(x);
	}
	break;
      case Vocab::cFWD:
	{
	  unsigned x = screen.getXpt() + *ptr++;
	  while(x >= 32) {
	    x -= 32;
	    screen.setYpt(screen.getYpt()+1);
	  }
	  screen.setXpt(x);
	}
	break;
      case Vocab::cPLURAL:
	{
	  unsigned y = screen.getYpt();
	  unsigned x = 29;
	  byte c;
	  while (x && (c = screen.gchar(y, x)) == ' ') --x;
	  if (c == 'X' || c == 'S') {
	    x++;
	    screen.hchar(y, x++, 'E', 1);
	  } else if (c == 'F') {
	    screen.hchar(y, x++, 'V', 1);
	    screen.hchar(y, x++, 'E', 1);
	  } else
	    x++;
	  screen.hchar(y, x++, 'S', 1);
	  if (x < 30)
	    screen.hchar(y, x++, ' ', 1);
	  screen.setXpt(x);
	}
	break;
      case Vocab::cED:
	{
	  unsigned x = screen.getXpt() - 2;
	  unsigned y = screen.getYpt();
	  if (screen.gchar(y, x) != 'E')
	    x++;
	  screen.hstr(y, x, "ED"); x+=2;
	  if (x < 30)
	    screen.hchar(y, x++, ' ', 1);
	  screen.setXpt(x);
	}
	break;
      case Vocab::cING:
	{
	  unsigned x = screen.getXpt() - 2;
	  unsigned y = screen.getYpt();
	  if (screen.gchar(y, x) != 'E')
	    x++;
	  screen.hstr(y, x, "ING"); x+=3;
	  if (x < 30)
	    screen.hchar(y, x++, ' ', 1);
	  screen.setXpt(x);
	}
	break;
      case Vocab::cINIT:
	initScreen();
	break;
      case Vocab::cCLREOS:
	printf("Bad C %02x\n", (unsigned)c);
	abort();
      case Vocab::cBACK:
	screen.setXpt(screen.getXpt()-1);
	break;
      case Vocab::cCALL:
	drawPrompt(*ptr++);
	screen.setXpt(screen.getXpt()+1);
	break;
    }
}

}}