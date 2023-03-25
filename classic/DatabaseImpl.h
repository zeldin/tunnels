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
    byte unknown_0400[0x32d2];
    byte pab[10];     // V@>36D2
    byte dsrname[28]; // V@>37DC
    byte unknown_36f8[8];
  } data;
protected:
  DatabaseImpl() { }
public:
  virtual void setFileData(bool isSave, unsigned len, Utils::StringSpan name)
    override;
};
  
}
}

#endif // TUNNELS_CLASSIC_DATABASEIMPL_H_
