// Draws one frame of the ivtools design tour with Skia into a CPU pixel
// buffer (N32 premultiplied). Areas left transparent show the Vulkan
// backdrop.
//
// Layout is designed for 1600x900 and scaled uniformly to the target size.
#pragma once

#include <cstdint>
#include <vector>

#include "include/core/SkPath.h"
#include "src/scene/design_script.h"
#include "src/scene/text.h"

namespace ivviz::scene {

struct FrameModel {
  int width = 1600, height = 900;
  Cursor cursor;
  float time = 0.f;   // wall-clock seconds, drives small animations (pulses)
  float speed = 1.f;  // playback speed, shown in the header
  bool paused = false;
  bool show_keys = true;  // the keyboard help line
};

// Where a class sits on the diagram (design units), and the orthogonal
// route a flow between two classes takes through the channels between the
// layer rows. Exposed for tests.
SkRect BlockRect(Part p);
std::vector<SkPoint> RoutePoints(Part from, Part to, int lane);
SkPath FlowRoute(Part from, Part to, int lane);

class Scene {
 public:
  explicit Scene(const Fonts* fonts) : fonts_(fonts) {}

  // `pixels` is width * height * 4 bytes.
  void Render(const FrameModel& m, uint8_t* pixels);

 private:
  void DrawHeader(SkCanvas* c, const FrameModel& m);
  void DrawRail(SkCanvas* c, const FrameModel& m);
  void DrawDiagram(SkCanvas* c, const FrameModel& m);
  void DrawInfo(SkCanvas* c, const FrameModel& m);
  void DrawPatterns(SkCanvas* c, const FrameModel& m);
  void DrawTrace(SkCanvas* c, const FrameModel& m);

  const Fonts* fonts_;
};

}  // namespace ivviz::scene
