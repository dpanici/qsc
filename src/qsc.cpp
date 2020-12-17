#include "qsc.hpp"

using namespace qsc;

void Qsc::defaults() {
  // Set defaults.
  verbose = 1;
  
  sG = 1;
  spsi = 1;
  B0 = 1.0;
  eta_bar = -1.0;
  I2 = 0.0;
  sigma0 = 0.0;
  B2s = 0.0;
  B2c = 0.0;
  p2 = 0.0;

  nfp = 3;
  nphi = 15;
}

Qsc::Qsc() :
  // Call constructor of member objects:
  d_d_phi(1, 1),
  d_d_varphi(1, 1)
{
  defaults();
  
  R0c.resize(1, 1.0);
  R0s.resize(1, 0.0);
  Z0c.resize(1, 0.0);
  Z0s.resize(1, 0.0);

}

/** High-level routine to call the low-level routines.
 */
void Qsc::calculate() {
  allocate();
  init_axis();
}
