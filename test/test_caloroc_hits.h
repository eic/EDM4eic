// SPDX-License-Identifier: Apache-2.0

#ifndef EDM4EIC_TEST_CALOROC_HITS_H
#define EDM4EIC_TEST_CALOROC_HITS_H

// edm for caloroc hits
#include "edm4eic/RawCALOROCHitCollection.h"

// podio-specific includes
#include "podio/Frame.h"

// stl includes
#include <array>
#include <iostream>
#include <string>



// ============================================================================
//! Test implementation of raw CALOROC hits
// ============================================================================
/*! This is a simple unit test to check the implementation of both
 *  type 1A (1 readout) and type 1B (low and high gain readouts)
 *  raw CALOROC hits.
 */
template <class WriterT, ReaderT>
class TestCALOROCHits {

  private:

    /// this is the threshold used to define the
    /// time-of-arrival and time-over-threshold
    /// values
    uint32_t m_threshold;

    /// this array defines amplitudes which mock-up a
    /// typical waveform produced by an HGC/CALOROC
    static const std::array<uint32_t, 112> m_arrAdcCounts {
      0,   0,   1,   2,    11,  43,  143, 338, 542,
      729, 875, 963, 1000, 993, 956, 902, 840, 774,
      709, 647, 593, 544,  500, 461, 428, 400, 374,
      350, 328, 307, 286,  268, 252, 236, 221, 206,
      193, 181, 169, 157,  146, 137, 129, 120, 112,
      105, 98,  91,  85,   81,  75,  70,  65,  62,
      58,  54,  50,  47,   44,  42,  39,  37,  34,
      31,  29,  28,  27,   24,  23,  21,  20,  19,
      18,  17,  16,  15,   14,  13,  12,  11,  11,
      10,  10,  10,  9,    9,   8,   8,   7,   7,
      7,   6,   6,   6,    5,   5,   5,   5,   5,
      5,   5,   5,   4,    4,   4,   4,   4,   4,
      4,   4,   4,   3};

  public:

    // ------------------------------------------------------------------------
    //! Test writing CALOROC hits
    // ------------------------------------------------------------------------
    void WriteHits(const std::string& outfilename) {

      std::cout << "\n --- Begin raw CALOROC write test" << std::endl;

      // create writer to store output hits
      WriterT writer(outfilename);
      std::cout << "   - created writer, writing to:\n"
                << "       " << outfilename
                << std::endl;

      // create frame & collections to hold output hits
      auto frame = podio::Frame();
      auto ahits = edm4eic::RawCALOROCHitCollection();
      auto bhits = edm4eic::RawCALOROCHitCollection();
      std::cout << "   - created frame/colletions" << std::endl;

      // 1st create a type 1A hit
      auto ahit = ahits.create();
      ahit.setType(0);

      /* TODO SET 1A members here */
      std::cout << "   - created type 1A hit"  << std::endl;

      // 2nd create a type 1B hit
      auto bhit = bhits.Create();
      bhit.setType(1);

      /* TODO SET 1B members here */
      std::cout << "   - created type 1B hit"  << std::endl;

      // write hit collections to frame & add a flag to
      // indicate this is a test frame
      frame.put(std::move(ahits), "RawType1ACALOROCHits");
      frame.put(std::move(bhits), "RawType1BCALOROCHits");
      frame.putParameter("FrameType", "test");
      std::cout << "   - Saved hits to frame"  << std::endl;

      // write frame, close writer
      writer.writeFrame(frame, "events");
      writer.finish();
      std::cout << "   - Wrote frame & closed writer\n" 
                << "   - Write test complete!"
                << std::endl;

      // exit
      return;

    }  // end 'WriteHits(std::string&)'

    // ------------------------------------------------------------------------
    //! Test reading CALOROC hits
    // ------------------------------------------------------------------------
    void ReadHits(const std::string& infilename) {

      std::cout << "\n --- Begin raw CALOROC read test" << std::endl;

      /* TODO put test here */
      return;

    }  // end 'ReadHits(std::string&)'

    // ctor/dtor
    TestCALOROCHits(const uint32_t threshold = 1) : m_threshold(threshold) {};
    ~TestCALOROCHits() {};

}  // end TestCALOROCHits<WriterT, ReaderT>


#endif
