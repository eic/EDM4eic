#ifndef EDM4EIC_UTILS_KINEMATICS_H
#define EDM4EIC_UTILS_KINEMATICS_H

#include <edm4eic/utils/kinematics.h>

namespace edm4eic {
/**
 * A LorentzVector with (px, py, pz) and M
 */
using edm4hep::LorentzVectorM;

/**
 * A LorentzVector with (px, py, pz) and E
 */
using edm4hep::LorentzVectorE;

namespace utils {

  /**
   * Get the transverse momentum from a Particle
   */
  using edm4hep::utils::pT;
  using edm4hep::utils::pt;

  /**
   * Get the total momentum from a Particle
   */
  using edm4hep::utils::p;

  /**
   * Get the 4 momentum vector from a Particle using the momentum and the mass.
   */
  using edm4hep::utils::p4;

} // namespace utils
} // namespace edm4eic

#endif
