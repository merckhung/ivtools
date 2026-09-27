#include "src/scene/scene.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "include/core/SkBlurTypes.h"
#include "include/core/SkColor.h"
#include "include/core/SkContourMeasure.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkRRect.h"
#include "include/effects/SkDashPathEffect.h"

namespace ivviz::scene {
namespace {

// Palette (ARGB).
constexpr SkColor kPanel = 0xD90B1620;
constexpr SkColor kPanelBorder = 0xFF1F3444;
constexpr SkColor kText = 0xFFE6EEF5;
constexpr SkColor kMuted = 0xFF8FA3B5;
constexpr SkColor kFaint = 0xFF4E6272;
constexpr SkColor kBlock = 0xE6122230;
constexpr SkColor kBlockDim = 0xB30E1A24;
constexpr SkColor kTrace = 0xFF22394A;
constexpr SkColor kInk = 0xFF0B1620;

SkColor WithAlpha(SkColor c, float a) {
  return SkColorSetA(c, static_cast<U8CPU>(std::clamp(a, 0.f, 1.f) * 255.f));
}

SkColor Mix(SkColor a, SkColor b, float t) {
  t = std::clamp(t, 0.f, 1.f);
  auto ch = [t](unsigned x, unsigned y) {
    return static_cast<U8CPU>(std::lround(x + (static_cast<float>(y) - x) * t));
  };
  return SkColorSetARGB(ch(SkColorGetA(a), SkColorGetA(b)), ch(SkColorGetR(a), SkColorGetR(b)),
                        ch(SkColorGetG(a), SkColorGetG(b)), ch(SkColorGetB(a), SkColorGetB(b)));
}

SkPaint Fill(SkColor c) {
  SkPaint p;
  p.setAntiAlias(true);
  p.setColor(c);
  return p;
}

SkPaint Stroke(SkColor c, float width) {
  SkPaint p = Fill(c);
  p.setStyle(SkPaint::kStroke_Style);
  p.setStrokeWidth(width);
  return p;
}

void Panel(SkCanvas* c, SkRect r, float radius = 12.f) {
  c->drawRRect(SkRRect::MakeRectXY(r, radius, radius), Fill(kPanel));
  c->drawRRect(SkRRect::MakeRectXY(r.makeInset(0.5f, 0.5f), radius, radius),
               Stroke(kPanelBorder, 1.f));
}

void Glow(SkCanvas* c, SkRect r, float radius, SkColor color, float sigma) {
  SkPaint p = Fill(color);
  p.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, sigma));
  c->drawRRect(SkRRect::MakeRectXY(r, radius, radius), p);
}

float Smooth(float t) {
  t = std::clamp(t, 0.f, 1.f);
  return t * t * (3.f - 2.f * t);
}

// ---------------------------------------------------------------------------
// Diagram layout (design coordinates, 1600x900): six layer rows of a
// ten-column grid. Flows run through the channels between rows and the
// gaps between columns, so they never cross a class block.
// ---------------------------------------------------------------------------

constexpr float kGridLeft = 140.f, kGridRight = 992.f;
constexpr int kCols = 10;
constexpr float kPitch = (kGridRight - kGridLeft) / kCols;
constexpr float kGap = 8.f;
constexpr float kRowTop = 190.f, kRowH = 46.f, kChannel = 28.f;
constexpr int kRows = 6;

float RowTop(int r) { return kRowTop + r * (kRowH + kChannel); }
float RowBottom(int r) { return RowTop(r) + kRowH; }
float GapX(int boundary) { return kGridLeft + boundary * kPitch - kGap * 0.5f; }

enum class Style { kClass, kApp, kImpl };

struct Block {
  int row, col, span;
  const char* label;
  const char* sub;
  Style style = Style::kClass;
};

const Block& BlockOf(Part p) {
  using S = Style;
  static const Block kBlocks[] = {
      // Applications.
      {0, 0, 2, "idraw", "IdrawEditor", S::kApp},
      {0, 2, 2, "drawtool", "OverlayEditor", S::kApp},
      {0, 4, 2, "comdraw", "ComEditor + ComTerp", S::kApp},
      {0, 6, 2, "graphdraw", "GraphEditor", S::kApp},
      {0, 8, 2, "drawserv", "DrawEditor + links", S::kApp},
      // OverlayUnidraw, ComUnidraw, ComTerp.
      {1, 0, 1, "OverlayKit", "editor builder"},
      {1, 1, 1, "OverlayEditor", "IdrawEditor"},
      {1, 2, 1, "OverlayComp", "GraphicComp"},
      {1, 3, 1, "OverlayView", "GraphicView"},
      {1, 4, 1, "OverlayScript", "ExternView"},
      {1, 5, 1, "ComEditor", "OverlayEditor"},
      {1, 6, 1, "UnidrawFunc", "rect(), move()"},
      {1, 7, 1, "ComTerp", "parser, stack"},
      {1, 8, 1, "ComFunc", "execute()"},
      {1, 9, 1, "ComValue", "args, results"},
      // Unidraw controllers.
      {2, 0, 1, "Unidraw", "facade"},
      {2, 1, 1, "Editor", "per window"},
      {2, 2, 1, "Viewer", "GraphicBlock"},
      {2, 3, 1, "KeyMap", "key → cmd"},
      {2, 4, 1, "Tool", "Move, Select"},
      {2, 5, 1, "Manipulator", "DragManip…"},
      {2, 6, 1, "Command", "Execute()"},
      {2, 7, 1, "MacroCmd", "cmd list"},
      {2, 8, 1, "Catalog", "files"},
      {2, 9, 1, "Creator", "by ClassId"},
      // Unidraw subjects and views.
      {3, 0, 1, "Component", "subject"},
      {3, 1, 1, "GraphicComps", "composite"},
      {3, 2, 1, "GraphicView", "draws a comp"},
      {3, 3, 1, "Selection", "of views"},
      {3, 4, 1, "Graphic", "Rect, Line…"},
      {3, 5, 1, "Picture", "composite"},
      {3, 6, 1, "Damage", "Incur, Repair"},
      {3, 7, 1, "Connector", "pins, slots"},
      {3, 8, 1, "CSolver", "Solve()"},
      {3, 9, 1, "StateVar", "brush, mode"},
      // InterViews 3.1.
      {4, 0, 1, "Session", "run()"},
      {4, 1, 1, "Event", "handle()"},
      {4, 2, 1, "InputHandler", "MonoGlyph"},
      {4, 3, 1, "Glyph", "ref-counted"},
      {4, 4, 1, "PolyGlyph", "Box, Deck"},
      {4, 5, 1, "MonoGlyph", "Border, Patch"},
      {4, 6, 1, "Character", "char + Font"},
      {4, 7, 1, "WidgetKit", "MFKit, OLKit"},
      {4, 8, 1, "LayoutKit", "hbox, vbox"},
      {4, 9, 1, "Observable", "notify()"},
      // Window system: abstractions and their X11 reps.
      {5, 0, 1, "Window", "abstraction"},
      {5, 1, 1, "WindowRep", "IV-X11", S::kImpl},
      {5, 2, 1, "Canvas", "abstraction"},
      {5, 3, 1, "CanvasRep", "IV-X11", S::kImpl},
      {5, 4, 1, "Painter", "IV-2_6"},
      {5, 5, 1, "PainterRep", "IV-X11", S::kImpl},
      {5, 6, 1, "Font", "shared"},
      {5, 7, 1, "FontRep", "IV-X11", S::kImpl},
      {5, 8, 1, "Display", "X connection"},
      {5, 9, 1, "Xlib", "X server", S::kImpl},
  };
  static_assert(sizeof kBlocks / sizeof kBlocks[0] == static_cast<size_t>(Part::kCount));
  return kBlocks[static_cast<int>(p)];
}

struct RowLabel {
  const char* title;
  const char* sub;
};
constexpr RowLabel kRowLabels[kRows] = {
    {"Applications", "main.c wiring"},  {"ivtools libs", "Overlay · Com"},
    {"Unidraw", "controllers"},         {"Unidraw", "subjects · views"},
    {"InterViews 3.1", "glyph toolkit"}, {"Window system", "IV-X11 bridge"},
};

// Lane offsets: 0, +5, -5, +10, -10, ...
float LaneOffset(int lane) {
  if (lane <= 0) return 0.f;
  const float mag = 5.f * ((lane + 1) / 2);
  return lane % 2 ? mag : -mag;
}

std::string Clock(float seconds) {
  char buf[16];
  std::snprintf(buf, sizeof buf, "%5.1f", seconds);
  return buf;
}

}  // namespace

SkRect BlockRect(Part p) {
  const Block& b = BlockOf(p);
  const float l = kGridLeft + b.col * kPitch;
  const float r = kGridLeft + (b.col + b.span) * kPitch - kGap;
  return SkRect::MakeLTRB(l, RowTop(b.row), r, RowBottom(b.row));
}

std::vector<SkPoint> RoutePoints(Part from, Part to, int lane) {
  const Block& ba = BlockOf(from);
  const Block& bb = BlockOf(to);
  const SkRect a = BlockRect(from), b = BlockRect(to);
  const float off = LaneOffset(lane);
  const float ax = std::clamp(a.centerX() + off, a.fLeft + 8, a.fRight - 8);
  const float bx = std::clamp(b.centerX() - off, b.fLeft + 8, b.fRight - 8);
  if (ba.row == bb.row) {
    // A U-turn through the channel below the row.
    const float y = RowBottom(ba.row) + kChannel * 0.5f + off * 0.8f;
    return {{ax, a.fBottom}, {ax, y}, {bx, y}, {bx, b.fBottom}};
  }
  const bool down = bb.row > ba.row;
  const float y1 = down ? RowBottom(ba.row) + kChannel * 0.5f + off * 0.8f
                        : RowTop(ba.row) - kChannel * 0.5f + off * 0.8f;
  const float y2 = down ? RowTop(bb.row) - kChannel * 0.5f + off * 0.8f
                        : RowBottom(bb.row) + kChannel * 0.5f + off * 0.8f;
  std::vector<SkPoint> pts = {{ax, down ? a.fBottom : a.fTop}, {ax, y1}};
  if (std::abs(bb.row - ba.row) > 1) {
    // Down (or up) the column gap next to the target, on the source's side.
    const int k = ax < b.centerX() ? bb.col : bb.col + bb.span;
    const float gx = GapX(k) + std::clamp(off * 0.3f, -2.f, 2.f);
    pts.push_back({gx, y1});
    pts.push_back({gx, y2});
  }
  pts.push_back({bx, y2});
  pts.push_back({bx, down ? b.fTop : b.fBottom});
  return pts;
}

SkPath FlowRoute(Part from, Part to, int lane) {
  const std::vector<SkPoint> pts = RoutePoints(from, to, lane);
  SkPathBuilder pb;
  pb.moveTo(pts[0]);
  for (size_t i = 1; i < pts.size(); ++i) pb.lineTo(pts[i]);
  return pb.detach();
}

// ---------------------------------------------------------------------------

void Scene::Render(const FrameModel& m, uint8_t* pixels) {
  const SkImageInfo info = SkImageInfo::MakeN32Premul(m.width, m.height);
  std::unique_ptr<SkCanvas> canvas =
      SkCanvas::MakeRasterDirect(info, pixels, static_cast<size_t>(m.width) * 4);
  SkCanvas* c = canvas.get();
  c->clear(SK_ColorTRANSPARENT);
  const float s = std::min(m.width / 1600.f, m.height / 900.f);
  c->translate((m.width - 1600.f * s) * 0.5f, (m.height - 900.f * s) * 0.5f);
  c->scale(s, s);
  DrawHeader(c, m);
  DrawRail(c, m);
  DrawDiagram(c, m);
  DrawInfo(c, m);
  DrawPatterns(c, m);
  DrawTrace(c, m);
}

void Scene::DrawHeader(SkCanvas* c, const FrameModel& m) {
  const Tour& st = DesignTours()[m.cursor.stage];
  c->drawRect(SkRect::MakeLTRB(-400, -400, 2000, 132), Fill(0x99060D13));
  c->drawLine(-400, 132, 2000, 132, Stroke(0x401F3444, 1.f));
  // Logo mark: three stacked glyph boxes (a tiny composite).
  c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(28, 18, 30, 30), 6, 6), Fill(st.accent));
  c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(33, 23, 20, 8), 2, 2), Fill(kInk));
  c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(33, 35, 8, 8), 2, 2), Fill(kInk));
  c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(45, 35, 8, 8), 2, 2), Fill(kInk));
  const float tw = DrawText(c, "How ivtools is designed", 72, 42, fonts_->Bold(26), kText);
  DrawText(c, "InterViews · Unidraw · ComTerp: the design patterns in the code, and how the calls flow",
           72 + tw + 16, 42, fonts_->Regular(15), kMuted);

  char right[96];
  std::snprintf(right, sizeof right, "t = %s s / %.0f s   %s %.1fx",
                Clock(SecondsAt(m.cursor)).c_str(), TotalDuration(),
                m.paused ? "paused" : "playing", m.speed);
  DrawText(c, right, 1572, 32, fonts_->Mono(14), kMuted, Align::kRight);
  if (m.show_keys) {
    DrawText(c, "Space pause · ←/→ tour · 1-9 jump · +/− speed · R restart", 1572, 52,
             fonts_->Regular(12.5f), kFaint, Align::kRight);
  }
}

void Scene::DrawRail(SkCanvas* c, const FrameModel& m) {
  const auto& tours = DesignTours();
  const int n = static_cast<int>(tours.size());
  const float x0 = 70, x1 = 1530, y = 92;
  const float step = (x1 - x0) / (n - 1);
  c->drawLine(x0, y, x1, y, Stroke(kTrace, 4));
  const float done_x = x0 + step * (m.cursor.stage + m.cursor.progress);
  SkPaint prog = Stroke(tours[m.cursor.stage].accent, 4);
  prog.setStrokeCap(SkPaint::kRound_Cap);
  c->drawLine(x0, y, std::min(done_x, x1), y, prog);
  for (int i = 0; i < n; ++i) {
    const float x = x0 + step * i;
    const bool current = i == m.cursor.stage;
    const bool done = i < m.cursor.stage;
    const SkColor col = current || done ? tours[i].accent : kFaint;
    if (current) {
      const float pulse = 0.5f + 0.5f * std::sin(m.time * 4.f);
      c->drawCircle(x, y, 17 + 3 * pulse, Fill(WithAlpha(col, 0.18f)));
    }
    c->drawCircle(x, y, 11, Fill(done ? col : kInk));
    c->drawCircle(x, y, 11, Stroke(col, 2.5f));
    char num[12];
    std::snprintf(num, sizeof num, "%d", i + 1);
    DrawText(c, num, x, y + 4.5f, fonts_->Bold(12), done ? kInk : col, Align::kCenter);
    DrawText(c, tours[i].name, x, y + 34, current ? fonts_->Bold(14) : fonts_->Regular(13.5f),
             current ? kText : (done ? kMuted : kFaint), Align::kCenter);
  }
}

void Scene::DrawDiagram(SkCanvas* c, const FrameModel& m) {
  const Tour& st = DesignTours()[m.cursor.stage];
  const float p = m.cursor.progress;
  Panel(c, SkRect::MakeLTRB(24, 140, 1004, 644));
  const float tw = DrawText(c, "ivtools classes by layer", 40, 170, fonts_->Bold(16), kText);
  DrawText(c, "lit: in this tour · small print: base class or role · badges: pattern roles · dots: calls in flight",
           40 + tw + 14, 170, fonts_->Regular(12.5f), kFaint);

  auto is_active = [&](Part part) {
    return std::find(st.active.begin(), st.active.end(), part) != st.active.end();
  };

  // Layer bands and their names.
  for (int r = 0; r < kRows; ++r) {
    const SkRect band = SkRect::MakeLTRB(34, RowTop(r) - 7, 996, RowBottom(r) + 7);
    c->drawRRect(SkRRect::MakeRectXY(band, 9, 9), Fill(r % 2 ? 0x400F2130 : 0x26142838));
    SkFont title = fonts_->Bold(12.5f);
    while (TextWidth(title, kRowLabels[r].title) > 94 && title.getSize() > 9.f) {
      title.setSize(title.getSize() - 0.5f);
    }
    DrawText(c, kRowLabels[r].title, 42, RowTop(r) + 20, title, kMuted);
    DrawText(c, kRowLabels[r].sub, 42, RowTop(r) + 35, fonts_->Regular(10.5f), kFaint);
  }

  // Label placement avoids badges and earlier labels.
  std::vector<SkRect> taken;

  // Pattern badges per block, in the order the patterns are listed.
  std::vector<std::vector<Pattern>> badges(static_cast<size_t>(Part::kCount));
  for (const PatternUse& pu : st.patterns) {
    for (Part part : pu.parts) badges[static_cast<int>(part)].push_back(pu.pattern);
  }

  // Blocks.
  for (int i = 0; i < static_cast<int>(Part::kCount); ++i) {
    const Part part = static_cast<Part>(i);
    const Block& b = BlockOf(part);
    const SkRect r = BlockRect(part);
    const bool active = is_active(part);
    const SkColor border = active ? st.accent : 0xFF2A3E4E;
    if (active) Glow(c, r.makeOutset(2, 2), 8, WithAlpha(border, 0.4f), 7);
    c->drawRRect(SkRRect::MakeRectXY(r, 7, 7), Fill(active ? kBlock : kBlockDim));
    SkPaint edge = Stroke(border, active ? 2.f : 1.2f);
    if (b.style == Style::kImpl) {
      const float dash[] = {5.f, 3.f};
      edge.setPathEffect(SkDashPathEffect::Make(dash, 0));
    }
    c->drawRRect(SkRRect::MakeRectXY(r.makeInset(0.75f, 0.75f), 7, 7), edge);
    const SkColor label_col = active ? kText : kMuted;
    SkFont font = fonts_->Bold(b.style == Style::kApp ? 13.5f : 12.f);
    while (TextWidth(font, b.label) > r.width() - 8 && font.getSize() > 8.5f) {
      font.setSize(font.getSize() - 0.5f);
    }
    const float cy = r.centerY() - 2;
    DrawText(c, b.label, r.centerX(), cy, font, label_col, Align::kCenter);
    const SkFont sub = fonts_->Regular(b.style == Style::kApp ? 10.5f : 9.5f);
    DrawText(c, Ellipsize(sub, b.sub, r.width() - 6), r.centerX(), cy + 14, sub,
             active ? kMuted : kFaint, Align::kCenter);

    // Badges sit on the top edge, right-aligned.
    float bx = r.fRight - 4;
    for (auto it = badges[i].rbegin(); it != badges[i].rend(); ++it) {
      const PatternInfo& pi = Info(*it);
      const SkFont bf = fonts_->Bold(8.5f);
      const float w = TextWidth(bf, pi.abbrev) + 8;
      const SkRect tag = SkRect::MakeLTRB(bx - w, r.fTop - 6, bx, r.fTop + 6);
      taken.push_back(tag);
      c->drawRRect(SkRRect::MakeRectXY(tag, 6, 6), Fill(pi.color));
      DrawText(c, pi.abbrev, tag.centerX(), tag.fBottom - 3.2f, bf, kInk, Align::kCenter);
      bx -= w + 2;
    }
  }

  // Flows: a lit trace with a stream of packets and a label in the channel.
  for (const Flow& f : st.flows) {
    if (p < f.start) continue;
    const float local = f.end > f.start ? (p - f.start) / (f.end - f.start) : 1.f;
    const bool running = local <= 1.f;
    const SkPath path = FlowRoute(f.from, f.to, f.lane);
    SkContourMeasureIter iter(path, false);
    sk_sp<SkContourMeasure> cm = iter.next();
    if (!cm) continue;
    const float len = cm->length();
    const float fade = running ? 1.f : 0.45f;
    SkPaint lit = Stroke(WithAlpha(f.color, 0.6f * fade), 2.5f);
    lit.setStrokeCap(SkPaint::kRound_Cap);
    lit.setStrokeJoin(SkPaint::kRound_Join);
    if (running) {
      const float head = len * Smooth(std::min(1.f, local * 3.f));
      SkPathBuilder partial;
      if (cm->getSegment(0, head, &partial, true)) c->drawPath(partial.detach(), lit);
      const float spacing = 24.f;
      const float offset = std::fmod(m.time * 90.f, spacing);
      for (float d = offset; d < head; d += spacing) {
        SkPoint pos;
        SkVector tan;
        if (!cm->getPosTan(d, &pos, &tan)) continue;
        SkPaint dot = Fill(f.color);
        dot.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, 1.2f));
        c->drawCircle(pos.fX, pos.fY, 3.3f, dot);
      }
      // Arrowhead at the target once the trace has arrived.
      SkPoint end;
      SkVector tan;
      if (head >= len - 0.5f && cm->getPosTan(len, &end, &tan)) {
        SkPathBuilder arrow;
        const SkVector n{-tan.fY, tan.fX};
        arrow.moveTo(end);
        arrow.lineTo(end.fX - tan.fX * 8 + n.fX * 4.5f, end.fY - tan.fY * 8 + n.fY * 4.5f);
        arrow.lineTo(end.fX - tan.fX * 8 - n.fX * 4.5f, end.fY - tan.fY * 8 - n.fY * 4.5f);
        arrow.close();
        c->drawPath(arrow.detach(), Fill(f.color));
      }
    } else {
      c->drawPath(path, lit);
    }
  }
  // Labels last, so no trace crosses them. Each sits on the route's longest
  // horizontal run (a channel), at label_at along it.
  for (const Flow& f : st.flows) {
    if (p < f.start || f.label.empty()) continue;
    const float local = f.end > f.start ? (p - f.start) / (f.end - f.start) : 1.f;
    if (local > 1.f) continue;
    const std::vector<SkPoint> pts = RoutePoints(f.from, f.to, f.lane);
    SkPoint a{}, b{};
    float best = -1;
    for (size_t i = 1; i < pts.size(); ++i) {
      const float w = std::fabs(pts[i].fX - pts[i - 1].fX);
      if (std::fabs(pts[i].fY - pts[i - 1].fY) < 0.5f && w > best) {
        best = w;
        a = pts[i - 1];
        b = pts[i];
      }
    }
    if (best < 0) continue;
    const SkFont font = fonts_->Bold(11);
    const float w = TextWidth(font, f.label) + 14;
    const float x0 = a.fX + (b.fX - a.fX) * f.label_at;
    // Slide along the channel (then off it) until the pill is free.
    SkRect pill;
    for (int k = 0; k < 13; ++k) {
      const float shift = (k % 2 ? 1.f : -1.f) * ((k + 1) / 2) * (w * 0.5f + 4.f);
      const float dy = k < 9 ? 0.f : (k % 2 ? 14.f : -14.f);
      const float x = std::clamp(x0 + (k < 9 ? shift : 0.f), 40.f + w / 2, 990.f - w / 2);
      pill = SkRect::MakeXYWH(x - w / 2, a.fY - 9 + dy, w, 18);
      bool free = true;
      for (const SkRect& t : taken) free = free && !SkRect::Intersects(t, pill.makeOutset(2, 1));
      if (free) break;
    }
    taken.push_back(pill);
    const float alpha = Smooth(std::min(1.f, local * 6.f));
    c->drawRRect(SkRRect::MakeRectXY(pill, 9, 9), Fill(WithAlpha(0xF20B1620, 0.95f * alpha)));
    c->drawRRect(SkRRect::MakeRectXY(pill, 9, 9), Stroke(WithAlpha(f.color, alpha), 1.2f));
    DrawText(c, f.label, pill.centerX(), pill.fBottom - 5, font, WithAlpha(f.color, alpha),
             Align::kCenter);
  }
}

void Scene::DrawInfo(SkCanvas* c, const FrameModel& m) {
  const auto& tours = DesignTours();
  const Tour& st = tours[m.cursor.stage];
  const SkRect r = SkRect::MakeLTRB(1020, 140, 1576, 468);
  Panel(c, r);
  c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeLTRB(r.fLeft, r.fTop + 14, r.fLeft + 4, r.fTop + 70),
                                   2, 2),
               Fill(st.accent));
  char tag[48];
  std::snprintf(tag, sizeof tag, "TOUR %d OF %d", m.cursor.stage + 1,
                static_cast<int>(tours.size()));
  DrawText(c, tag, r.fLeft + 22, r.fTop + 28, fonts_->Bold(12), st.accent);
  float y = r.fTop + 56;
  for (const std::string& line : Wrap(fonts_->Bold(21), st.title, r.width() - 44)) {
    DrawText(c, line, r.fLeft + 22, y, fonts_->Bold(21), kText);
    y += 26;
  }
  const float lw = DrawText(c, "Layer:", r.fLeft + 22, y + 2, fonts_->Bold(12.5f), kMuted) + 6;
  for (const std::string& line : Wrap(fonts_->Regular(12.5f), st.layer, r.width() - 44 - lw)) {
    DrawText(c, line, r.fLeft + 22 + lw, y + 2, fonts_->Regular(12.5f), kMuted);
    y += 17;
  }
  y += 10;

  const int n = static_cast<int>(st.steps.size());
  const int current = std::min(n - 1, static_cast<int>(m.cursor.progress * n));
  // Largest body size whose wrapped steps fit above the source line.
  float size = 14.f, lh = 18.f;
  for (; size > 10.f; size -= 0.25f) {
    lh = size * 1.26f;
    float h = 0;
    for (const std::string& step : st.steps) {
      h += Wrap(fonts_->Regular(size), step, r.width() - 70).size() * lh + 5;
    }
    if (y + h <= r.fBottom - 30) break;
  }
  const SkFont body = fonts_->Regular(size);
  for (int i = 0; i < n; ++i) {
    const bool now = i == current;
    const SkColor col = i <= current ? kText : kMuted;
    const float bx = r.fLeft + 22;
    c->drawCircle(bx + 9, y - 5, 9, Fill(now ? st.accent : (i < current ? 0xFF2A4152 : 0xFF182A36)));
    char num[12];
    std::snprintf(num, sizeof num, "%d", i + 1);
    DrawText(c, num, bx + 9, y - 1, fonts_->Bold(11), now ? kInk : kMuted, Align::kCenter);
    for (const std::string& line : Wrap(body, st.steps[i], r.width() - 70)) {
      DrawText(c, line, bx + 26, y, body, col);
      y += lh;
    }
    y += 5;
  }
  const SkFont mono = fonts_->Mono(12);
  const std::string where = Ellipsize(mono, st.where, r.width() - 110);
  DrawText(c, "Source:", r.fLeft + 22, r.fBottom - 14, fonts_->Bold(12), kMuted);
  DrawText(c, where, r.fLeft + 78, r.fBottom - 14, mono, st.accent);
}

void Scene::DrawPatterns(SkCanvas* c, const FrameModel& m) {
  const Tour& st = DesignTours()[m.cursor.stage];
  const SkRect r = SkRect::MakeLTRB(1020, 480, 1576, 644);
  Panel(c, r);
  const float tw =
      DrawText(c, "Design patterns", r.fLeft + 18, r.fTop + 24, fonts_->Bold(14.5f), kText);
  DrawText(c, "the badge on a class marks its role", r.fLeft + 18 + tw + 10, r.fTop + 24,
           fonts_->Regular(11.5f), kFaint);
  float y = r.fTop + 48;
  const SkFont roles = fonts_->Regular(11.5f);
  float name_w = 0;
  for (const PatternUse& pu : st.patterns) {
    const PatternInfo& pi = Info(pu.pattern);
    name_w = std::max(name_w, TextWidth(fonts_->Bold(9.5f), pi.abbrev) + 18 +
                                  TextWidth(fonts_->Bold(13), pi.name));
  }
  const float text_x = r.fLeft + 18 + std::max(110.f, name_w) + 14;
  const float text_w = r.fRight - 16 - text_x;
  for (const PatternUse& pu : st.patterns) {
    const PatternInfo& pi = Info(pu.pattern);
    const SkFont bf = fonts_->Bold(9.5f);
    const float bw = TextWidth(bf, pi.abbrev) + 10;
    const SkRect tag = SkRect::MakeXYWH(r.fLeft + 18, y - 11, bw, 14);
    c->drawRRect(SkRRect::MakeRectXY(tag, 7, 7), Fill(pi.color));
    DrawText(c, pi.abbrev, tag.centerX(), tag.fBottom - 3.5f, bf, kInk, Align::kCenter);
    DrawText(c, pi.name, tag.fRight + 8, y, fonts_->Bold(13), pi.color);
    std::vector<std::string> lines = Wrap(roles, pu.roles, text_w);
    if (lines.size() > 3) {
      lines.resize(3);
      lines[2] = Ellipsize(roles, lines[2] + " …", text_w);
    }
    float ly = y;
    for (const std::string& line : lines) {
      DrawText(c, line, text_x, ly, roles, kMuted);
      ly += 14.f;
    }
    y = std::max(y + 20.f, ly) + 6.f;
  }
}

void Scene::DrawTrace(SkCanvas* c, const FrameModel& m) {
  const Tour& st = DesignTours()[m.cursor.stage];
  const SkRect r = SkRect::MakeLTRB(24, 658, 1576, 884);
  c->drawRRect(SkRRect::MakeRectXY(r, 12, 12), Fill(0xF2050B10));
  c->drawRRect(SkRRect::MakeRectXY(r.makeInset(0.5f, 0.5f), 12, 12), Stroke(kPanelBorder, 1.f));
  c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeLTRB(r.fLeft, r.fTop, r.fRight, r.fTop + 28), 12,
                                   12),
               Fill(0xFF0F1D28));
  c->drawRect(SkRect::MakeLTRB(r.fLeft, r.fTop + 16, r.fRight, r.fTop + 28), Fill(0xFF0F1D28));
  for (int i = 0; i < 3; ++i) {
    c->drawCircle(r.fLeft + 18 + i * 16, r.fTop + 14, 5,
                  Fill(i == 0 ? 0xFFFF6B6B : i == 1 ? 0xFFFFC75F : 0xFF6BE38A));
  }
  const float tw = DrawText(c, "call trace", r.fLeft + 72, r.fTop + 19, fonts_->Bold(12.5f), kMuted);
  DrawText(c, "· " + st.where, r.fLeft + 72 + tw + 8, r.fTop + 19, fonts_->Mono(12), st.accent);
  DrawText(c, "the control flow in ivtools/src, newest call last; arguments abbreviated",
           r.fRight - 16, r.fTop + 19, fonts_->Regular(11.5f), kFaint, Align::kRight);

  const std::vector<TraceLine> lines = TraceUpTo(m.cursor);
  const SkFont mono = fonts_->Mono(13);
  const SkFont file_font = fonts_->Mono(11.5f);
  const float line_h = 17.f;
  const float top = r.fTop + 50;
  const int max_lines = static_cast<int>((r.fBottom - 10 - top) / line_h) + 1;
  const int first = std::max(0, static_cast<int>(lines.size()) - max_lines);
  const float indent = TextWidth(mono, "  ");
  const float file_x = r.fRight - 16;
  float y = top;
  for (int i = first; i < static_cast<int>(lines.size()); ++i) {
    const TraceLine& l = lines[i];
    const bool newest = i == static_cast<int>(lines.size()) - 1;
    // Indent guides: one faint rule per call-stack level.
    for (int d = 0; d < l.depth; ++d) {
      const float gx = r.fLeft + 20 + d * indent * 1.5f + 3;
      c->drawLine(gx, y - 13, gx, y + 4, Stroke(0xFF1C2E3B, 1.f));
    }
    const float x = r.fLeft + 20 + l.depth * indent * 1.5f;
    SkColor col = newest ? Mix(st.accent, 0xFFFFFFFF, 0.25f) : 0xFFB9F6C8;
    if (newest) {
      const float blink = 0.5f + 0.5f * std::sin(m.time * 6.f);
      c->drawRRect(SkRRect::MakeRectXY(SkRect::MakeLTRB(r.fLeft + 8, y - 13, r.fRight - 8, y + 5),
                                       4, 4),
                   Fill(WithAlpha(st.accent, 0.10f + 0.06f * blink)));
    }
    const float file_w = TextWidth(file_font, l.file);
    const std::string text = Ellipsize(mono, l.text, file_x - file_w - 24 - x);
    DrawText(c, text, x, y, mono, col);
    DrawText(c, l.file, file_x, y, file_font, newest ? kMuted : kFaint, Align::kRight);
    y += line_h;
  }
  if (lines.empty()) {
    DrawText(c, "(the trace starts as the tour plays)", r.fLeft + 20, top, mono, kFaint);
  }
}

}  // namespace ivviz::scene
