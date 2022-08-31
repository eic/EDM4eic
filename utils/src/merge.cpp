#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

int main() {
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  reader.openFile("sim_central_electrons.edm4hep.root");
  store.setReader(&reader);

  //auto reader_bkg = podio::ROOTReader();
  //auto store_bkg = podio::EventStore();
  //reader_bkg.openFile("bkg_central_electrons.edm4hep.root");
  //store_bkg.setReader(&reader_bkg);

  // test writing a subset of the event to a new file
  auto writer = podio::ROOTWriter("output.root", &store);
  writer.registerForWrite("MCParticles");

  unsigned nEvents = reader.getEntries();
  for (unsigned i = 0; i < nEvents; ++i) {
    std::cout << "reading event " << i << std::endl;

    for (unsigned j = 0; j < 2; ++j) {
      std::cout << "adding event " << j << std::endl;

      //store_bkg.clear();
      //reader_bkg.endOfEvent();
    }

    writer.writeEvent();

    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  //reader_bkg.closeFile();
  return 0;
}
