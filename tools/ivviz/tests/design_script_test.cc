// Consistency checks for the design tours (no test framework needed).
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

#include "src/scene/design_script.h"

namespace {

int failures = 0;

void Check(bool ok, const std::string& what) {
  if (!ok) {
    std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    ++failures;
  }
}

}  // namespace

int main() {
  using namespace ivviz::scene;
  const auto& tours = DesignTours();
  Check(tours.size() == 9, "nine tours (keys 1-9)");
  float total = 0;
  for (size_t i = 0; i < tours.size(); ++i) {
    const Tour& t = tours[i];
    const std::string id = "tour " + std::to_string(i + 1) + " (" + t.name + ")";
    auto active = [&](Part p) {
      return std::find(t.active.begin(), t.active.end(), p) != t.active.end();
    };
    Check(t.duration > 0, id + ": positive duration");
    Check(!t.steps.empty() && !t.title.empty() && !t.layer.empty() && !t.where.empty(),
          id + ": has title, layer, source and steps");
    Check(t.showcase >= 0 && t.showcase <= 1, id + ": showcase in [0,1]");
    Check(!t.patterns.empty() && t.patterns.size() <= 3, id + ": one to three patterns");
    Check(!t.trace.empty(), id + ": has a call trace");
    for (const PatternUse& pu : t.patterns) {
      Check(pu.pattern < Pattern::kCount && !pu.roles.empty(), id + ": pattern and roles");
      for (Part p : pu.parts) Check(active(p), id + ": pattern roles are lit classes");
    }
    float prev = -1;
    for (const TraceLine& l : t.trace) {
      Check(l.at >= prev && l.at >= 0 && l.at <= 1, id + ": trace times ascend within [0,1]");
      Check(l.depth >= 0 && l.depth < 10 && !l.file.empty(), id + ": trace depth and file");
      prev = l.at;
    }
    for (const Flow& f : t.flows) {
      Check(f.from != f.to && f.from < Part::kCount && f.to < Part::kCount, id + ": flow ends");
      Check(active(f.from) && active(f.to), id + ": flows connect lit classes");
      Check(f.start >= 0 && f.start <= f.end && f.end <= 1, id + ": flow window");
      Check(f.label_at >= 0 && f.label_at <= 1 && f.lane >= 0, id + ": label position, lane");
    }
    for (size_t j = 0; j < i; ++j) Check(tours[j].name != t.name, id + ": unique name");
    total += t.duration;
  }
  Check(std::fabs(total - TotalDuration()) < 1e-4f, "TotalDuration sums the tours");

  for (int p = 0; p < static_cast<int>(Pattern::kCount); ++p) {
    const PatternInfo& info = Info(static_cast<Pattern>(p));
    Check(info.name && std::string(info.abbrev).size() == 2, "pattern info " + std::to_string(p));
  }

  // Cursor <-> seconds round trip, and clamping at both ends.
  for (float t = 0; t < TotalDuration(); t += 0.37f) {
    const Cursor c = CursorAt(t);
    Check(std::fabs(SecondsAt(c) - t) < 1e-3f, "round trip at t=" + std::to_string(t));
  }
  Check(CursorAt(-5).stage == 0 && CursorAt(-5).progress == 0, "clamps before the start");
  const Cursor end = CursorAt(TotalDuration() + 10);
  Check(end.stage == static_cast<int>(tours.size()) - 1 && end.progress == 1, "clamps at the end");

  // Within a tour the trace only grows, and it is complete at the end.
  for (int i = 0; i < static_cast<int>(tours.size()); ++i) {
    size_t prev_lines = 0;
    for (float p = 0; p <= 1.f; p += 0.05f) {
      const size_t n = TraceUpTo({i, p}).size();
      Check(n >= prev_lines, "trace never shrinks within a tour");
      prev_lines = n;
    }
    Check(TraceUpTo({i, 1.f}).size() == tours[i].trace.size(), "whole trace at the end");
  }

  if (failures) return 1;
  std::printf("design_script_test: all checks passed\n");
  return 0;
}
