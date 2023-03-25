#ifndef TUNNELS_CLASSIC_DATABASEFACTORY_H_
#define TUNNELS_CLASSIC_DATABASEFACTORY_H_

#include "Database.h"

namespace Tunnels {

namespace Classic {

class DatabaseFactory : public Tunnels::DatabaseFactory {
public:
  virtual Database *load(File::ReadHandle &handle) override;
  virtual bool save(File::WriteHandle &handle, const Database &db) override;
};
  
}
}

#endif // TUNNELS_CLASSIC_DATABASEFACTORY_H_
