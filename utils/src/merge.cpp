// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2022 Wouter Deconinck, Simon Gardner

#include <regex>
#include <tuple>
#include <utility>

#include <CLI/CLI.hpp>
#include <typeinfo>

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTFrameReader.h"
#include "podio/ROOTWriter.h"

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/EventHeaderCollection.h>

//std::map<std::string,podio::CollectionBase (*)()> collectionmap = {{"edm4hep::CaloHitContributionCollection",edm4hep::CaloHitContributionData}};

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
  auto store_sig  = podio::EventStore();
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
  auto store_out  = podio::EventStore();
  auto writer_out = podio::ROOTWriter(file_out, &store_out);

  typename A(podio::CollectionBase);

  auto coll_id_table_sig = store_sig.getCollectionIDTable();

  std::vector<std::pair<std::string, std::string>> collection_names;
  std::map<std::string,edm4hep::EventHeaderCollection*>       header_collections_out;
  std::map<std::string,edm4hep::MCParticleCollection*>        mcparticle_collections_out;
  std::map<std::string,edm4hep::SimTrackerHitCollection*>     tracker_collections_out;
  std::map<std::string,edm4hep::SimCalorimeterHitCollection*> calorimeter_collections_out;

  int i=0;
  for (const auto& name: coll_id_table_sig->names()) {
    auto& hits_sig = store_sig.get<podio::CollectionBase>(name);
    auto type = hits_sig.getTypeName();

    if(debug)
      std::cout << " source: " << name << " " << type << std::endl;


    // check for inclusion
    for (const auto& include_re: include_regex) {
      if (std::regex_match(name, std::regex(include_re))) {
        // check for exclusion
        bool exclude = false;
        for (const auto& exclude_re: exclude_regex) {
          exclude = exclude || std::regex_match(name, std::regex(exclude_re));
        }
	if(exclude){
	  if(verbose ||debug)
	    std::cout << " Not including " << name << std::endl;
	  continue;
	}

        if (exclude == false) {
	  //          collection_names.push_back(std::make_pair(type, name));
	  if(type == "edm4hep::EventHeaderCollection"){
	    header_collections_out[name] = &store_out.create<edm4hep::EventHeaderCollection>(name);
	  }
	  else if(type == "edm4hep::MCParticleCollection"){
	    mcparticle_collections_out[name] = &store_out.create<edm4hep::MCParticleCollection>(name);
	  }
	  else if(type == "edm4hep::SimTrackerHitCollection"){
	    tracker_collections_out[name] = &store_out.create<edm4hep::SimTrackerHitCollection>(name);
	  }
	  else if(type == "edm4hep::SimCalorimeterHitCollection"){
	    calorimeter_collections_out[name] = &store_out.create<edm4hep::SimCalorimeterHitCollection>(name);
	  }
	  else{
	    if(verbose ||debug)
	      std::cout << " Not including " << name << std::endl;
	    continue;
	  }
	  
	  if(verbose || debug)
	    std::cout << " Including " << name << std::endl;
	  
          collection_names.push_back(std::make_pair(type, name));
	  writer_out.registerForWrite(name);
	  i++;
        }
      }
   }
  }

  auto coll_id_table_out = store_out.getCollectionIDTable();

  // loop over events
  unsigned n = std::min(numberOfEvents, reader_sig.getEntries());
  for (unsigned j = 0; j < n; ++j) {
    if (verbose) std::cout << "reading signal event " << j << "/" << n << std::endl;

    // Add Signal hits
    for (const auto& [type,name]: collection_names) {
      if(type=="edm4hep::EventHeaderCollection"){
	auto& hits_sig = store_sig.get<edm4hep::EventHeaderCollection>(name);
	if (debug) std::cout << " signal:"  << " " << name << ": " << hits_sig.size() << std::endl;
	for (const auto& hit_sig: hits_sig) {
	  header_collections_out[name]->push_back(hit_sig.clone());
	}
      }
      if(type=="edm4hep::MCParticleCollection"){
	auto& hits_sig = store_sig.get<edm4hep::MCParticleCollection>(name);
	if (debug) std::cout << " signal:"  << " " << name << ": " << hits_sig.size() << std::endl;
	for (const auto& hit_sig: hits_sig ) {
	  mcparticle_collections_out[name]->push_back(hit_sig.clone());
	}
      }
      if(type=="edm4hep::SimTrackerHitCollection"){
	auto& hits_sig = store_sig.get<edm4hep::SimTrackerHitCollection>(name);
	if (debug) std::cout << " signal:"  << " " << name << ": " << hits_sig.size() << std::endl;
	for (const auto& hit_sig: hits_sig) {
	  tracker_collections_out[name]->push_back(hit_sig.clone());
	}
      }
      if(type=="edm4hep::SimCalorimeterHitCollection"){
	auto& hits_sig = store_sig.get<edm4hep::SimCalorimeterHitCollection>(name);
	if (debug) std::cout << " signal:"  " " << name << ": " << hits_sig.size() << std::endl;
	for (const auto& hit_sig: hits_sig) {
	  calorimeter_collections_out[name]->push_back(hit_sig.clone());
	}
      }
    }
    store_sig.clear();
    reader_sig.endOfEvent();

    int k=0;
    // Add background hits
    for (const auto& [reader_bkg, store_bkg, count_bkg]: readers_stores_counts_bkg) {
      // repeat requested times
      for (int count = 0; count < count_bkg; count++) {

	// clone background hits (reader and store is pointer)
	for (const auto& [type,name]: collection_names) {
	  if(type=="edm4hep::MCParticleCollection"){
	    auto& hits_bkg = store_bkg->get<edm4hep::MCParticleCollection>(name);
	    if (debug) std::cout << " background source: " << k << " event no: " << count << " " << name << ": " << hits_bkg.size() << std::endl;
	    for (const auto& hit_bkg: hits_bkg) {
	      mcparticle_collections_out[name]->push_back(hit_bkg.clone());
	    }
	  }
	  if(type=="edm4hep::SimTrackerHitCollection"){
	    auto& hits_bkg = store_bkg->get<edm4hep::SimTrackerHitCollection>(name);
	    if (debug) std::cout << " background source: " << k << " event no: " << count << " " << name << ": " << hits_bkg.size() << std::endl;
	    for (const auto& hit_bkg: hits_bkg) {
	      tracker_collections_out[name]->push_back(hit_bkg.clone());
	    }
	  }
	  if(type=="edm4hep::SimCalorimeterHitCollection"){
	    auto& hits_bkg = store_bkg->get<edm4hep::SimCalorimeterHitCollection>(name);
	    if (debug) std::cout << " background source: " << k << " event no: " << count << " " << name << ": " << hits_bkg.size() << std::endl;
	    for (const auto& hit_bkg: hits_bkg) {
	      calorimeter_collections_out[name]->push_back(hit_bkg.clone());
	    }
	  }
	}
	store_bkg->clearCollections();
	reader_bkg->endOfEvent();
      }     
      k++;
    }

    
    if (debug){
      for (const auto& [type, name]: collection_names) {
	auto coll = &store_out.get<podio::CollectionBase>(name);
	std::cout << " out event no: " << j <<  " " << name << ": " << coll->size() << std::endl;
      }
    }

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
