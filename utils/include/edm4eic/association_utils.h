// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2025 Wouter Deconinck

#ifndef EDM4EIC_UTILS_ASSOCIATION_HH
#define EDM4EIC_UTILS_ASSOCIATION_HH

#include <podio/ObjectID.h>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <type_traits>

namespace edm4eic {

/**
 * @brief Helper class to efficiently lookup associations between objects.
 *
 * This class provides O(1) lookup time for associations by building hash maps
 * from association collections. It handles both one-to-one and one-to-many
 * relationships, and satisfies C++20 range concepts for compatibility with
 * standard algorithms.
 *
 * Satisfies: std::ranges::forward_range, std::ranges::sized_range
 *
 * When 'from' and 'to' types are different, provides operator[] for natural lookup syntax.
 * Named methods are always available regardless of type differences.
 *
 * @tparam AssociationCollection Type of the association collection (e.g., MCRecoCalorimeterHitAssociationCollection)
 * @tparam GetFromObjectFunc Function type to extract the 'from' object from an association
 * @tparam GetToObjectFunc Function type to extract the 'to' object from an association
 *
 * Example usage:
 * @code
 *   // For rec->sim associations
 *   auto lookup = make_association_lookup(
 *     mchitassociations,
 *     [](const auto& assoc) { return assoc.getRawHit(); },  // from: rec
 *     [](const auto& assoc) { return assoc.getSimHit(); }   // to: sim
 *   );
 *
 *   // Natural operator[] syntax when types are different
 *   auto sim_hits = lookup[raw_hit];  // O(1) lookup
 *   for (const auto& sim_hit : sim_hits) {
 *     // Process sim_hit
 *   }
 *
 *   // Named methods always available
 *   auto sim_hits_alt = lookup.lookup_from_to(raw_hit);
 *   auto raw_hits = lookup.lookup_to_from(sim_hit);
 *
 *   // Can also iterate over all from->to mappings
 *   for (const auto& [from_id, to_objs] : lookup.from_to_view()) {
 *     // Process mapping
 *   }
 *
 *   // Works with C++20 ranges
 *   auto filtered = lookup.from_to_view() 
 *                 | std::views::filter([](const auto& p) { return p.second.size() > 1; });
 * @endcode
 */
template <typename AssociationCollection, typename GetFromObjectFunc, typename GetToObjectFunc>
class association_lookup {
public:
  using from_object_t = std::decay_t<decltype(std::declval<GetFromObjectFunc>()(
      *std::declval<const AssociationCollection*>()->begin()))>;
  using to_object_t = std::decay_t<decltype(std::declval<GetToObjectFunc>()(
      *std::declval<const AssociationCollection*>()->begin()))>;
  
  using from_to_map_t = std::unordered_map<podio::ObjectID, std::vector<to_object_t>>;
  using to_from_map_t = std::unordered_map<podio::ObjectID, std::vector<from_object_t>>;

  // Compile-time check if 'from' and 'to' types are different
  static constexpr bool different_types = !std::is_same_v<from_object_t, to_object_t>;

  /**
   * @brief Construct an association lookup helper
   *
   * @param associations The association collection to index
   * @param get_from_object Function to extract 'from' object from association
   * @param get_to_object Function to extract 'to' object from association
   */
  association_lookup(const AssociationCollection* associations, GetFromObjectFunc get_from_object,
                     GetToObjectFunc get_to_object)
      : m_get_from_object(get_from_object), m_get_to_object(get_to_object) {
    if (associations) {
      build_maps(associations);
    }
  }

  /**
   * @brief Lookup 'to' objects using operator[] (only when types differ)
   *
   * @param from_obj The 'from' object
   * @return Vector of associated 'to' objects (empty if none found)
   */
  std::vector<to_object_t> operator[](const from_object_t& from_obj) const 
    requires different_types
  {
    return lookup_from_to(from_obj);
  }

  /**
   * @brief Lookup 'to' objects associated with a 'from' object
   *
   * @param from_obj The 'from' object
   * @return Vector of associated 'to' objects (empty if none found)
   */
  std::vector<to_object_t> lookup_from_to(const from_object_t& from_obj) const {
    auto it = m_from_to_map.find(from_obj.getObjectID());
    if (it != m_from_to_map.end()) {
      return it->second;
    }
    return {};
  }

  /**
   * @brief Lookup 'from' objects associated with a 'to' object
   *
   * @param to_obj The 'to' object
   * @return Vector of associated 'from' objects (empty if none found)
   */
  std::vector<from_object_t> lookup_to_from(const to_object_t& to_obj) const {
    auto it = m_to_from_map.find(to_obj.getObjectID());
    if (it != m_to_from_map.end()) {
      return it->second;
    }
    return {};
  }

  /**
   * @brief Check if a 'from' object has any associations
   *
   * @param from_obj The 'from' object
   * @return true if associations exist
   */
  bool has_from_associations(const from_object_t& from_obj) const {
    return m_from_to_map.find(from_obj.getObjectID()) != m_from_to_map.end();
  }

  /**
   * @brief Check if a 'to' object has any associations
   *
   * @param to_obj The 'to' object
   * @return true if associations exist
   */
  bool has_to_associations(const to_object_t& to_obj) const {
    return m_to_from_map.find(to_obj.getObjectID()) != m_to_from_map.end();
  }

  /**
   * @brief Get view of from->to map for iteration (satisfies std::ranges::forward_range)
   */
  const from_to_map_t& from_to_view() const { return m_from_to_map; }

  /**
   * @brief Get view of to->from map for iteration (satisfies std::ranges::forward_range)
   */
  const to_from_map_t& to_from_view() const { return m_to_from_map; }

  /**
   * @brief Get the total number of associations indexed
   */
  size_t size() const { return m_total_associations; }

  /**
   * @brief Check if the lookup is empty
   */
  bool empty() const { return m_total_associations == 0; }

  // Iterator support for range-based for loops over from->to map
  auto begin() const { return m_from_to_map.begin(); }
  auto end() const { return m_from_to_map.end(); }
  auto cbegin() const { return m_from_to_map.cbegin(); }
  auto cend() const { return m_from_to_map.cend(); }

private:
  void build_maps(const AssociationCollection* associations) {
    for (const auto& assoc : *associations) {
      auto from_obj = m_get_from_object(assoc);
      auto to_obj = m_get_to_object(assoc);

      m_from_to_map[from_obj.getObjectID()].push_back(to_obj);
      m_to_from_map[to_obj.getObjectID()].push_back(from_obj);
      ++m_total_associations;
    }
  }

  GetFromObjectFunc m_get_from_object;
  GetToObjectFunc m_get_to_object;

  from_to_map_t m_from_to_map;
  to_from_map_t m_to_from_map;
  size_t m_total_associations = 0;
};

/**
 * @brief Helper function to create an association_lookup with type deduction
 *
 * Example:
 * @code
 *   auto lookup = make_association_lookup(
 *     mchitassociations,
 *     [](const auto& a) { return a.getRawHit(); },  // from
 *     [](const auto& a) { return a.getSimHit(); }   // to
 *   );
 * @endcode
 */
template <typename AssociationCollection, typename GetFromObjectFunc, typename GetToObjectFunc>
association_lookup<AssociationCollection, GetFromObjectFunc, GetToObjectFunc>
make_association_lookup(const AssociationCollection* associations, GetFromObjectFunc get_from_object,
                        GetToObjectFunc get_to_object) {
  return association_lookup<AssociationCollection, GetFromObjectFunc, GetToObjectFunc>(
      associations, get_from_object, get_to_object);
}

} // namespace edm4eic

#endif // EDM4EIC_UTILS_ASSOCIATION_HH
