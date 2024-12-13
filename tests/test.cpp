#include <vector>
#include "../dsp/envelope.hpp"

using namespace dspheaders;

std::vector<BreakPoint> bkp {
  {0.f, 0.f, 0.f},
  {1.f, 0.2, 0.f},
  {0.f, 0.7, 0.f}
};

dspheaders::Envelope env = dspheaders::Envelope::init(bkp, 48000);

int main() {
  // env.repr();
}
