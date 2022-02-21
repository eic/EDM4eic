#ifndef EICD_UTILS_VECTOR_HH
#define EICD_UTILS_VECTOR_HH

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <string>
#include <vector>

#include <Math/Vector4D.h>

#include <edm4hep/Vector3f.h>

#include <eicd/ReconstructedParticleCollection.h>
#include <eicd/ReconstructedParticleData.h>
#include <eicd/TrackParametersCollection.h>

namespace eicd {

template <class V> concept VectorHasX = requires(V v) { v.x; };
template <class V> concept VectorHasY = requires(V v) { v.y; };
template <class V> concept VectorHasZ = requires(V v) { v.z; };
template <class V> concept ClassVector = requires(V v) { v.x(); };
template <class V>
concept Vector2D =
    VectorHasX<V>&& VectorHasY<V> && !VectorHasZ<V> && !ClassVector<V>;
template <class V>
concept Vector3D =
    VectorHasX<V>&& VectorHasY<V>&& VectorHasZ<V> && !ClassVector<V>;
template <class V> concept VectorND = Vector2D<V> || Vector3D<V>;

inline double etaToAngle(const double eta) {
  return std::atan(std::exp(-eta)) * 2.;
}
inline double angleToEta(const double theta) {
  return -std::log(std::tan(0.5 * theta));
}

// inline edm4hep::Vector2f VectorFromPolar(const double r, const double theta)
// {
//  return {r * sin(theta), r * cos(theta)};
//}
template <Vector3D V = edm4hep::Vector3f>
V sphericalToVector(const double r, const double theta, const double phi) {
  using FloatType = decltype(V().x);
  const double sth = sin(theta);
  const double cth = cos(theta);
  const double sph = sin(phi);
  const double cph = cos(phi);
  const FloatType x = r * sth * cph;
  const FloatType y = r * sth * sph;
  const FloatType z = r * cth;
  return {x, y, z};
}

template <Vector3D V> double anglePolar(const V& v) {
  return std::atan2(std::hypot(v.x, v.y), v.z);
}
template <VectorND V> double angleAzimuthal(const V& v) {
  return std::atan2(v.y, v.x);
}
template <Vector3D V> double eta(const V& v) {
  return angleToEta(anglePolar(v));
}
template <Vector2D V> double magnitude(const V& v) {
  return std::hypot(v.x, v.y);
}
template <Vector3D V> double magnitude(const V& v) {
  return std::hypot(v.x, v.y, v.z);
}
template <Vector3D V> double magnitudeTransverse(const V& v) {
  return std::hypot(v.x, v.y);
}
template <Vector3D V> double magnitudeLongitudinal(const V& v) { return v.z; }
template <VectorND V> V normalizeVector(const V& v, double norm = 1.) {
  const double old = magnitude(v);
  if (old == 0) {
    return v;
  }
  return (norm > 0) ? v * norm / old : v * 0;
}
template <Vector3D V> V vectorTransverse(const V& v) { return {v.x, v.y, 0}; }
template <Vector3D V> V vectorLongitudinal(const V& v) { return {0, 0, v.z}; }
// Two vector functions
template <VectorND V> double angleBetween(const V& v1, const V& v2) {
  const double dot = v1 * v2;
  if (dot == 0) {
    return 0.;
  }
  return acos(dot / (magnitude(v1) * magnitude(v2)));
}
// Project v onto v1
template <Vector3D V> double projection(const V& v, const V& v1) {
  const double norm = magnitude(v1);
  if (norm == 0) {
    return magnitude(v);
  }
  return v * v1 / norm;
}

} // namespace eicd
template <eicd::Vector2D V> V operator+(const V& v1, const V& v2) {
  return {v1.x + v2.x, v1.y + v2.y};
}
template <eicd::Vector3D V> V operator+(const V& v1, const V& v2) {
  return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}
template <eicd::Vector2D V> double operator*(const V& v1, const V& v2) {
  return v1.x * v2.x + v1.y * v2.y;
}
template <eicd::Vector3D V> double operator*(const V& v1, const V& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
template <eicd::Vector2D V> V operator*(const double d, const V& v) {
  return {d * v.x, d * v.y};
}
template <eicd::Vector3D V> V operator*(const double d, const V& v) {
  return {d * v.x, d * v.y, d * v.z};
}
template <eicd::Vector2D V> V operator*(const V& v, const double d) {
  return {d * v.x, d * v.y};
}
template <eicd::Vector3D V> V operator*(const V& v, const double d) {
  return {d * v.x, d * v.y, d * v.z};
}
template <eicd::VectorND V> V operator-(const V& v1, const V& v2) {
  return v1 + (-1. * v2);
}
template <eicd::VectorND V> V operator/(const V& v, const double d) {
  return (1. / d) * v;
}
#endif
