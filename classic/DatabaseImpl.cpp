#include "system.h"

#include "classic/DatabaseImpl.h"
#include "Utils.h"

namespace Tunnels { namespace Classic {

Utils::StringSpan DatabaseImpl::getDescription() const
{
  return data.description;
}

Utils::StringSpan DatabaseImpl::getPatternTable() const
{
  return data.patternTable;
}

void DatabaseImpl::clearPlayerSheet(unsigned n)
{
  /* FIXME */
  data.player[n].classId = 0;
}

Utils::StringSpan DatabaseImpl::getPlayerName(unsigned n) const
{
  return data.player[n].name;
}

void DatabaseImpl::setPlayerName(unsigned n, Utils::StringSpan name)
{
  name.store(data.player[n].name);
}

void DatabaseImpl::setPlayerClass(unsigned n, unsigned c)
{
  data.player[n].classId = c << 6;
  for (unsigned i = 0; i < 64; i++)
    data.patternTable[(n << 6) | i] = data.classPatternTable[c][i];
}

Utils::StringSpan DatabaseImpl::getClassName(unsigned n) const
{
  return data.classes[n].name;
}

Utils::StringSpan DatabaseImpl::getClassPatternTable(unsigned n) const
{
  return data.classPatternTable[n];
}

byte DatabaseImpl::getNumClassChoices() const
{
  byte n = data.numClasses;
  if ((n & 0x80)) {
    n = -n;
    if (data.numPlayers != 1)
      --n;
  }
  return n;
}

void DatabaseImpl::setPlayerColor(unsigned n, unsigned c)
{
  static constexpr byte colors[] = { 0xce, 0x4e, 0xde, 0x6e };
  data.playerColor[n] = colors[c];
}

Utils::StringSpan DatabaseImpl::getDictionaryWord(byte n) const
{
  return data.dictionary[n];
}

void DatabaseImpl::setFileData(bool isSave, unsigned len,
			       Utils::StringSpan name)
{
  enum {
    OPCODE_OPEN = 0,
    OPCODE_CLOSE = 1,
    OPCODE_READ = 2,
    OPCODE_WRITE = 3,
    OPCODE_REWIND = 4,
    OPCODE_LOAD = 5,
    OPCODE_SAVE = 6,
    OPCODE_DELETE = 7,
    OPCODE_SCRATCH = 8,
    OPCODE_STATUS = 9
  };
  enum {
    FLAG_FIX = 0x00,
    FLAG_VAR = 0x10,
    FLAG_DIS = 0x00,
    FLAG_INT = 0x08,
    FLAG_UPDATE = 0x00,
    FLAG_INPUT  = 0x04,
    FLAG_OUTPUT = 0x02,
    FLAG_APPEND = 0x06,
    FLAG_SEQ = 0x00,
    FLAG_REL = 0x01
  };
  data.pab[0] = (isSave? OPCODE_SAVE : OPCODE_LOAD);
  data.pab[1] = (isSave? (FLAG_FIX | FLAG_INT | FLAG_OUTPUT | FLAG_SEQ) :
		         (FLAG_FIX | FLAG_INT | FLAG_INPUT  | FLAG_SEQ));
  data.pab[2] = 0x04; // Load address: V@>0400
  data.pab[3] = 0x00;
  data.pab[4] = 0x80; // Record length
  data.pab[5] = 0x80; // Number of bytes
  data.pab[6] = 0x33; // File size: >3300
  data.pab[7] = 0x00;
  data.pab[8] = 0x00; // Screen offset
  data.pab[9] = len;  // DSR name length
  name.store(data.dsrname);
}

}}
