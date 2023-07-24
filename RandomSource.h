#ifndef TUNNELS_RANDOMSOURCE_H_
#define TUNNELS_RANDOMSOURCE_H_

namespace Tunnels {

class RandomSource {
public:
  virtual byte random(byte lower, byte upper) = 0;
};

}

#endif // TUNNELS_RANDOMSOURCE_H_
