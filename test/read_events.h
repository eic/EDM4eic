// SPDX-License-Identifier: Apache-2.0

#ifndef EDM4EIC_TEST_READ_EVENTS_H__
#define EDM4EIC_TEST_READ_EVENTS_H__

// test data model
#include "edm4eic/RawTrackerHitCollection.h"

// podio specific includes
#include "podio/Frame.h"

// STL
#include <cassert>
#include <exception>
#include <iostream>
#include <vector>

void processEvent(const podio::Frame& event, bool verboser, unsigned eventNum) {
  auto& raw_hits = event.get<edm4eic::RawTrackerHitCollection>("RawTrackerHits");

  if (raw_hits.isValid()) {

    //-------- print particles for debugging:

    std::cout << "\n collection:  "
              << "RawTrackerHits"
              << " of type " << raw_hits.getValueTypeName() << "\n\n"
              << raw_hits << std::endl;
    //-------------------------------

    // check a few things (to be completed ...)
    auto raw_hit = raw_hits[0];
    if (raw_hit.getCellID() != 0x0123456789abcdefLL)
      throw std::runtime_error("wrong CellID for first hit - should be 0x0123456789abcdefLL ");

  } else {
    throw std::runtime_error("Collection 'RawTrackerHits' should be present");
  }

  //===============================================================================

  const auto& evtType = event.getParameter<std::string>("EventType");
  std::cout << "Event Type: " << evtType << std::endl;
}

template <typename ReaderT>
void read_events(const std::string& filename) {
  ReaderT reader;
  reader.openFile(filename);

  unsigned nEvents = reader.getEntries("events");
  for (unsigned i = 0; i < nEvents; ++i) {
    std::cout << "reading event " << i << std::endl;
    const auto event = podio::Frame(reader.readNextEntry("events"));
    processEvent(event, true, i);
  }
}

#endif
