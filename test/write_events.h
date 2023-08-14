// SPDX-License-Identifier: Apache-2.0

#ifndef EDM4EIC_TEST_WRITE_EVENTS_H
#define EDM4EIC_TEST_WRITE_EVENTS_H

// Data model
#include "edm4eic/RawTrackerHitCollection.h"

// STL
#include <iostream>
#include <vector>

// podio specific includes
#include "podio/Frame.h"

template <class WriterT>
void write(std::string outfilename) {
  std::cout << "start processing" << std::endl;

  WriterT writer(outfilename);

  unsigned nevents = 10;

  // =============== event loop ================================
  for (unsigned i = 0; i < nevents; ++i) {
    std::cout << " --- processing event " << i << std::endl;
    auto event = podio::Frame();

    auto raw_hits = edm4eic::RawTrackerHitCollection();
    auto raw_hit = raw_hits.create();
    raw_hit.setCellID(0x0123456789abcdefLL);
    raw_hit.setCharge(0x01234567);
    raw_hit.setTimeStamp(0x89abcdef);

    //-------- print hits for debugging:
    std::cout << "\n collection:  "
              << "RawTrackerHits"
              << " of type " << raw_hits.getValueTypeName() << "\n\n"
              << raw_hits << std::endl;

    event.put(std::move(raw_hits), "RawTrackerHits");

    //===============================================================================

    event.putParameter("EventType", "test");

    writer.writeFrame(event, "events");
  }

  writer.finish();
}

#endif