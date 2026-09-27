// The design of ivtools, as a sequence of guided "tours": which classes
// take part, which GoF design pattern each one plays a role in, what moves
// between them (calls and data) and the call trace behind it.
//
// Every class, method and file named here exists in this repository
// (ivtools/src/include/{InterViews,IV-look,IV-X11,IV-2_6,Unidraw}, and
// ivtools/src/{OverlayUnidraw,ComUnidraw,ComTerp,DrawServ,...}). The call
// traces follow the real control flow; argument lists are abbreviated.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ivviz::scene {

// Classes on the architecture diagram, grouped by layer (rows).
enum class Part {
  // Applications (each main.c wires a Creator, Catalog, Unidraw and Editor).
  kIdraw,
  kDrawtool,
  kComdraw,
  kGraphdraw,
  kDrawserv,
  // ivtools extension libraries: OverlayUnidraw, ComUnidraw, ComTerp.
  kOverlayKit,
  kOverlayEditor,
  kOverlayComp,
  kOverlayView,
  kOverlayScript,
  kComEditor,
  kUnidrawFunc,
  kComTerp,
  kComFunc,
  kComValue,
  // Unidraw: controllers and infrastructure.
  kUnidraw,
  kEditor,
  kViewer,
  kKeyMap,
  kTool,
  kManipulator,
  kCommand,
  kMacroCmd,
  kCatalog,
  kCreator,
  // Unidraw: subjects, views, graphics.
  kComponent,
  kGraphicComps,
  kGraphicView,
  kSelection,
  kGraphic,
  kPicture,
  kDamage,
  kConnector,
  kCSolver,
  kStateVar,
  // InterViews 3.1 glyph toolkit.
  kSession,
  kEvent,
  kInputHandler,
  kGlyph,
  kPolyGlyph,
  kMonoGlyph,
  kCharacter,
  kWidgetKit,
  kLayoutKit,
  kObservable,
  // Window-system bridge: abstractions and their X11 implementations.
  kWindow,
  kWindowRep,
  kCanvas,
  kCanvasRep,
  kPainter,
  kPainterRep,
  kFont,
  kFontRep,
  kDisplay,
  kXlib,
  kCount
};

// The design patterns the tours point out. Each has a fixed colour, used
// for its badge on the diagram and its card in the pattern panel.
enum class Pattern {
  kLayers,  // architectural, not GoF
  kFacade,
  kSingleton,
  kComposite,
  kDecorator,
  kFlyweight,
  kAbstractFactory,
  kBridge,
  kObserver,
  kCommand,
  kStrategy,
  kMediator,
  kFactoryMethod,
  kPrototype,
  kInterpreter,
  kAdapter,
  kTemplateMethod,
  kCount
};

struct PatternInfo {
  const char* name;
  const char* abbrev;  // 2 letters, for the badge on a class block
  uint32_t color;      // ARGB
};
const PatternInfo& Info(Pattern p);

// One pattern as it appears in a tour: which classes play it and how
// (the "roles" line reads like "Component: Glyph · Composite: PolyGlyph").
struct PatternUse {
  Pattern pattern;
  std::string roles;
  std::vector<Part> parts;
};

// A call or a piece of data moving between two classes during
// [start, end] of a tour (fractions of the tour).
struct Flow {
  Part from, to;
  std::string label;
  float start = 0.f, end = 1.f;
  uint32_t color = 0xFF7FD8FF;  // ARGB
  float label_at = 0.5f;        // where the label sits along the path
  int lane = 0;                 // offsets the route, so parallel flows don't overlap
};

// One line of the call trace; `depth` is the call-stack depth.
struct TraceLine {
  float at;  // fraction of the tour
  int depth;
  std::string text;
  std::string file;  // relative to ivtools/src
};

struct Tour {
  std::string name;   // short, for the tour rail
  std::string title;  // headline
  std::string layer;  // which libraries the tour is about
  std::string where;  // main source reference
  std::vector<std::string> steps;
  float duration;     // seconds at 1x speed
  uint32_t accent;    // ARGB
  std::vector<Part> active;
  std::vector<PatternUse> patterns;
  std::vector<Flow> flows;
  std::vector<TraceLine> trace;
  float showcase = 0.7f;  // a representative moment, for screenshots
};

const std::vector<Tour>& DesignTours();

// Total length of the sequence at 1x speed.
float TotalDuration();

// Position in the sequence: tour index and progress within it (0..1).
struct Cursor {
  int stage = 0;
  float progress = 0.f;
};
Cursor CursorAt(float seconds);
float SecondsAt(Cursor c);

// Trace lines of the current tour printed up to `c`, oldest first. The
// trace restarts with each tour (every tour is its own call sequence).
std::vector<TraceLine> TraceUpTo(Cursor c);

}  // namespace ivviz::scene
