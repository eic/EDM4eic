// SPDX-License-Identifier: Apache-2.0

#include "test_caloroc_hits.h"
#include "podio/ROOTWriter.h"
#include "podio/ROOTReader.h"

int main(int argc, char* argv[]) {

  // announce start
  std::cout << "Start raw CALOROC tests." << std::endl;

  // run tests
  TestRaw<podio::ROOTWriter, podio::ROOTReader> test(1);
  test.WriteHits("edm4eic_rawcalorochits.root");
  test.ReadHits("edm4eic_rawcalorochits.root");

  // announce end & exit
  std::cout << "Raw CALOROC tests finished." << std::endl;
  return 0;

}
