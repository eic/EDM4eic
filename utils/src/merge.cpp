#include <regex>
#include <tuple>
#include <utility>

#include <CLI/CLI.hpp>

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"

int main(int argc, char **argv) {
  // setup CLI options
  CLI::App app;

  // debug
  bool debug{false};
  app.add_flag("--debug,-d", debug, "Enable debug output");

  // verbose
  bool verbose{false};
  app.add_flag("--verbose,-v", verbose, "Enable verbose output");

  // signal input file
  std::string file_sig{""};
  app.add_option("--signal,-s", file_sig, "Signal file");

  // background input files
  std::vector<std::tuple<std::string, unsigned int>> files_bkg{};
  app.add_option("--background,-b", files_bkg, "Background files");

  // output file
  std::string file_out{""};
  app.add_option("--output,-o", file_out, "Output file");

  // collection include regex
  std::vector<std::string> include_regex{{".*"}};
  app.add_option("--include,-i", include_regex, "Collection inclusion regex");

  // collection exclude regex
  std::vector<std::string> exclude_regex{};
  app.add_option("--exclude,-e", exclude_regex, "Collection exclusion regex");

  // signal input file
  unsigned int numberOfEvents{0};
  app.add_option("--numberOfEvents,-n", numberOfEvents, "Number of events");

  CLI11_PARSE(app, argc, argv);

  if (debug) verbose = true;

  // input reader
  auto reader_sig = podio::ROOTReader();
  auto store_sig = podio::EventStore();
  reader_sig.openFile(file_sig);
  store_sig.setReader(&reader_sig);

  // background readers
  std::vector<std::tuple<podio::ROOTReader*, podio::EventStore*, unsigned int>> readers_stores_counts_bkg;
  for (const auto& [file_bkg, count_bkg]: files_bkg) {
    readers_stores_counts_bkg.emplace_back(new podio::ROOTReader(), new podio::EventStore(), count_bkg);
    std::get<0>(readers_stores_counts_bkg.back())->openFile(file_bkg);
    std::get<1>(readers_stores_counts_bkg.back())->setReader(std::get<0>(readers_stores_counts_bkg.back()));
  }

  // output writer
  auto store_out = podio::EventStore();
  auto writer_out = podio::ROOTWriter(file_out, &store_out);

  // hit collections
  auto coll_id_table_sig = store_sig.getCollectionIDTable();
  std::vector<std::pair<size_t, std::string>> calorimeter_collection_names;
  std::vector<std::pair<size_t, std::string>> tracker_collection_names;
  for (const auto& name: coll_id_table_sig->names()) {
    // check for inclusion
    for (const auto& include_re: include_regex) {
      if (std::regex_match(name, std::regex(include_re))) {
        // check for exclusion
        bool exclude = false;
        for (const auto& exclude_re: exclude_regex) {
          exclude = exclude || std::regex_match(name, std::regex(exclude_re));
        }
        // check for type by getting FIXME does not work
        auto& hits_sig = store_sig.get<edm4hep::SimTrackerHitCollection>(name);
        if (! store_sig.isValid()) {
          std::cout << "Collection " << name << " invalid as tracker collection" << std::endl;
          exclude = false;
        }
        //
        if (exclude == false) {
          std::cout << "Collection " << name << " included" << std::endl;
          tracker_collection_names.push_back(std::make_pair(tracker_collection_names.size(), name));
        }
      }
    }
  }

  // create output calorimeter collections
  std::vector<edm4hep::SimCalorimeterHitCollection*> calorimeter_collections_out;
  for (const auto& [i, calorimeter_collection_name]: calorimeter_collection_names) {
    calorimeter_collections_out.push_back(&(store_out.create<edm4hep::SimCalorimeterHitCollection>(calorimeter_collection_name)));
    writer_out.registerForWrite(calorimeter_collection_name);
  }

  // create output tracker collections
  std::vector<edm4hep::SimTrackerHitCollection*> tracker_collections_out;
  for (const auto& [i, tracker_collection_name]: tracker_collection_names) {
    tracker_collections_out.push_back(&(store_out.create<edm4hep::SimTrackerHitCollection>(tracker_collection_name)));
    writer_out.registerForWrite(tracker_collection_name);
  }

  // loop over events
  unsigned n = std::min(numberOfEvents, reader_sig.getEntries());
  for (unsigned i = 0; i < n; ++i) {
    if (verbose) std::cout << "reading signal event " << i << "/" << n << std::endl;

    // loop over signal collections
    for (const auto& [i, tracker_collection_name]: tracker_collection_names) {
      // clone signal hits
      auto& hits_sig = store_sig.get<edm4hep::SimTrackerHitCollection>(tracker_collection_name);
      if (debug) std::cout << " sig " << tracker_collection_name << ": " << hits_sig.size() << std::endl;
      for (const auto& hit_sig: hits_sig) {
        tracker_collections_out[i]->push_back(hit_sig.clone());
      }
    }

    // loop over background collections
    for (const auto& [reader_bkg, store_bkg, count_bkg]: readers_stores_counts_bkg) {
      // repeat requested times
      for (auto count = 0; count != count_bkg; ++count) {
        // clone background hits (reader and store is pointer)
        for (const auto& [i, tracker_collection_name]: tracker_collection_names) {
          auto& hits_bkg = store_bkg->get<edm4hep::SimTrackerHitCollection>(tracker_collection_name);
          if (debug) std::cout << " bkg" << i << " " << tracker_collection_name << ": " << hits_bkg.size() << std::endl;
          for (const auto& hit_bkg: hits_bkg) {
            tracker_collections_out[i]->push_back(hit_bkg.clone());
          }
        }
        store_bkg->clear();
        reader_bkg->endOfEvent();
      }
    }

    for (const auto& [i, tracker_collection_name]: tracker_collection_names) {
      if (debug) std::cout << " out " << tracker_collection_name << ": " << tracker_collections_out[i]->size() << std::endl;
    }

    // clear input stores
    store_sig.clear();
    reader_sig.endOfEvent();

    // write event
    writer_out.writeEvent();
    store_out.clearCollections();
  }

  // close files
  reader_sig.closeFile();
  for (const auto& [reader_bkg, store_bkg, count_bkg]: readers_stores_counts_bkg) {
    reader_bkg->closeFile();
  }
  writer_out.finish();

  return 0;
}
