#include "system.h"

#include "Engine.h"
#include "Database.h"
#include "File.h"
#include "Utils.h"

namespace Tunnels {

void GameEngine::ioError(bool casette, byte error)
{
  screen.drawIoError(casette, error);
  if (error == File::ERROR_BAD_ATTRIBUTE ||
      error == File::ERROR_ILLEGAL_OPCODE)
    screen.drawPrompt(0x70);
  else if(error == File::ERROR_MEMORY_FULL)
    screen.drawPrompt(0x71);
  else if(error == File::ERROR_PAST_EOF || error == File::ERROR_FILE_ERROR)
    screen.drawPrompt(0x72); // Was 0x04?!
  else
    screen.drawPrompt(0x6f);
}

void GameEngine::preserveState()
{
  // FIXME: G@>608D
  database->setSavedDirection(static_cast<Database::Direction>(direction));
  database->setSavedProgression(progression);
}

void GameEngine::restoreState()
{
  // FIXME: G@>610D
  direction = database->getSavedDirection();
  progression = database->getSavedProgression();
}

GameEngine::Diversion GameEngine::loadSave(bool isSave, unsigned len,
					   Utils::StringSpan name)
{
  if (database)
    database->setFileData(isSave, len, name);

  name.subspan(0, len);
  screen.refresh();
  bool failed = false;
  if (isSave) {
    if (!database || !file.save(*database, name, databaseFactory))
      failed = true;
  } else {
    Database *db = file.load(name, databaseFactory);
    if (db != nullptr && database != nullptr)
      delete database;
    if (db == nullptr)
      failed = true;
    else
      database = db;
    screen.setDatabase(database);
  }
  File::Error error = file.getError();
  if (error || !database)
    failed = true;
  if(failed) {
    sound.honk();
    ioError(len==3 && *name.ptr()=='C', error);
    if (Diversion d = flashBorder())
      return d;
    return DIVERSION_LOAD_SAVE;
  }
  
  return DIVERSION_NULL;
}

namespace {
template<unsigned n> void setPrefix(byte (&buffer)[n], const char *pfx)
{
  for(unsigned i=0; i<n; i++)
    if (pfx[i])
      buffer[i] = pfx[i];
    else
      break;
}
}

GameEngine::Diversion GameEngine::loadSaveMenu()
{
  byte devicename[28];
  setPrefix(devicename, "CS1                         ");
  redoTarget = DIVERSION_LOAD_SAVE;
  procdTarget = DIVERSION_NEW_OR_RESTOCK;
  acceptMask = ACCEPT_REDO | ACCEPT_NUMERIC;
  screen.drawPrompt(0x11);
  byte x;
  Diversion d;
  if (progression != 2) {
    acceptMask |= ACCEPT_PROCD;
    screen.drawPrompt(0x13);
    d = getNumberKey(x, 1, 6);
  } else
    d = getNumberKey(x, 1, 3);
  if(d)
    return d;
  sound.beep();
  bool isSave = false;
  if (x > 2) {
    preserveState();
    isSave = true;
    x -= 3;
  }
  switch (x) {
  case 0:
    screen.drawPrompt(0x18);
    break;
  case 1:
    setPrefix(devicename, "DSK1.");
    screen.drawPrompt(0x14);
    acceptMask = ACCEPT_REDO | ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC;
    d = getString(8, devicename+5);
    break;
  case 2:
    screen.drawPrompt(0x15);
    acceptMask = ACCEPT_REDO | ACCEPT_ALPHANUMERIC2;
    d = getString(devicename);
    break;
  }
  if (d)
    return d;
  Utils::StringSpan devicespan{devicename};
  unsigned len = devicespan.prefixLen();
  if (!len)
    return DIVERSION_LOAD_SAVE;
  screen.drawPrompt(0x0d);
  if ((d = loadSave(isSave, len, devicespan)))
    return d;
  restoreState();
  return DIVERSION_NULL;
}

}
