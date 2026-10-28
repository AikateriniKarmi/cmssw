#include <cmath>
#include <memory>
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Scouting/interface/Run3ScoutingParticle.h"

class BadScoutingEventFilter : public edm::stream::EDFilter<> {
public:
  explicit BadScoutingEventFilter(const edm::ParameterSet&);
  ~BadScoutingEventFilter() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  bool filter(edm::Event&, const edm::EventSetup&) override;

  edm::EDGetTokenT<Run3ScoutingParticleCollection> particlesToken_;
  double maxPt_;
};

BadScoutingEventFilter::BadScoutingEventFilter(const edm::ParameterSet& config)
    : particlesToken_(consumes<Run3ScoutingParticleCollection>(config.getParameter<edm::InputTag>("particles"))),
      maxPt_(config.getParameter<double>("maxPt")) {
  produces<Run3ScoutingParticleCollection>();
}

void BadScoutingEventFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription description;
  description.add<edm::InputTag>("particles");
  description.add<double>("maxPt");
  descriptions.addWithDefaultLabel(description);
}

bool BadScoutingEventFilter::filter(edm::Event& event, const edm::EventSetup&) {
  const auto particles = event.getHandle(particlesToken_);
  bool validEvent = true;

  for (const auto& particle : *particles) {
    const double pt = particle.pt();

    if (!std::isfinite(pt) || pt < 0.0 || pt > maxPt_) {
      validEvent = false;
      break;
    }
  }

  auto outputParticles = std::make_unique<Run3ScoutingParticleCollection>();

  if (validEvent) {
    *outputParticles = *particles;
  }

  event.put(std::move(outputParticles));
  return validEvent;
}

DEFINE_FWK_MODULE(BadScoutingEventFilter);
