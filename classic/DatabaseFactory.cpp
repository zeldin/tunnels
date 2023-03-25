#include "system.h"

#include "classic/DatabaseFactory.h"
#include "classic/DatabaseImpl.h"
#include "File.h"

namespace Tunnels { namespace Classic {

Database *DatabaseFactory::load(File::ReadHandle &handle)
{
  DatabaseImpl *database = new DatabaseImpl;
  if (handle.readBytes(database->data) != sizeof(database->data)) {
    delete database;
    database = nullptr;
  }
  return database;
}

bool DatabaseFactory::save(File::WriteHandle &handle, const Database &db)
{
  const DatabaseImpl *database = dynamic_cast<const DatabaseImpl *>(&db);
  if (!database)
    return false;
  return handle.writeBytes(database->data) == sizeof(database->data);
}

}}
