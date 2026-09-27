#include "src/scene/design_script.h"

#include <algorithm>

namespace ivviz::scene {
namespace {

using P = Part;
using Pt = Pattern;

// Pattern colours (ARGB). Flows use the colour of the pattern they show.
constexpr uint32_t kLayersC = 0xFF8FA3B5;
constexpr uint32_t kFacadeC = 0xFFFFD166;
constexpr uint32_t kSingletonC = 0xFFFFA94D;
constexpr uint32_t kCompositeC = 0xFF4DD4C6;
constexpr uint32_t kDecoratorC = 0xFFFF8C42;
constexpr uint32_t kFlyweightC = 0xFFFF7AB6;
constexpr uint32_t kFactoryC = 0xFFB18CFF;
constexpr uint32_t kBridgeC = 0xFF5C9DFF;
constexpr uint32_t kObserverC = 0xFF6BE38A;
constexpr uint32_t kCommandC = 0xFFFF6B6B;
constexpr uint32_t kStrategyC = 0xFF5CD6FF;
constexpr uint32_t kMediatorC = 0xFFE7A6FF;
constexpr uint32_t kFactoryMethodC = 0xFFC3E86B;
constexpr uint32_t kPrototypeC = 0xFFFFE08A;
constexpr uint32_t kInterpreterC = 0xFFFF9EC4;
constexpr uint32_t kAdapterC = 0xFF9FE2FF;
constexpr uint32_t kTemplateC = 0xFFD9C5A0;

std::vector<Tour> MakeTours() {
  std::vector<Tour> t;

  // 1 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Layers",
      .title = "Five layers: from Xlib up to the drawing editors",
      .layer = "all of ivtools/src; drawtool's main() as the example",
      .where = "drawtool/main.c: main",
      .steps = {"InterViews 3.1 is the glyph toolkit; only its IV-X11 reps call Xlib.",
                "Unidraw builds editors on it: subjects, views, tools, commands with undo, a catalog for files.",
                "OverlayUnidraw, ComUnidraw and ComTerp add overlays, scripting and an interpreter.",
                "An app is mostly wiring: main() picks the Creator, Catalog, Unidraw and Editor, then Open(ed) and Run()."},
      .duration = 9.f,
      .accent = 0xFF7FB2FF,
      .active = {P::kDrawtool, P::kOverlayEditor, P::kOverlayKit, P::kUnidraw, P::kEditor,
                 P::kCatalog, P::kCreator, P::kSession},
      .patterns = {{Pt::kLayers, "each layer calls only the ones below it; apps → ivtools libs → "
                                 "Unidraw → InterViews → IV-X11 → Xlib",
                    {}},
                   {Pt::kFacade, "Unidraw: Open, Run, Update, Log, Undo, Redo, GetCatalog — one "
                                 "front for the whole editor framework",
                    {P::kUnidraw}},
                   {Pt::kTemplateMethod, "OverlayKit::Init calls InitViewer, InitLayout, "
                                         "MakeMenus; GraphKit, FrameKit override the steps",
                    {P::kOverlayKit}}},
      .flows = {{P::kDrawtool, P::kCreator, "OverlayCreator", 0.02f, 0.2f, kFactoryMethodC},
                {P::kDrawtool, P::kCatalog, "new OverlayCatalog", 0.12f, 0.32f, kLayersC, 0.5f, 1},
                {P::kDrawtool, P::kUnidraw, "new OverlayUnidraw", 0.25f, 0.45f, kFacadeC, 0.5f, 2},
                {P::kDrawtool, P::kOverlayEditor, "new OverlayEditor", 0.4f, 0.58f, kLayersC},
                {P::kOverlayEditor, P::kOverlayKit, "Init(comp)", 0.5f, 0.66f, kTemplateC},
                {P::kUnidraw, P::kEditor, "Open(ed)", 0.62f, 0.78f, kFacadeC},
                {P::kUnidraw, P::kSession, "Run(): read · handle", 0.74f, 1.f, kFacadeC, 0.6f}},
      .trace = {{0.02f, 0, "main(argc, argv)", "drawtool/main.c"},
                {0.08f, 1, "OverlayCreator creator;", "drawtool/main.c"},
                {0.16f, 1, "catalog = new OverlayCatalog(\"drawtool\", &creator)", "drawtool/main.c"},
                {0.26f, 1, "unidraw = new OverlayUnidraw(catalog, argc, argv, options, properties)",
                 "drawtool/main.c"},
                {0.34f, 2, "Unidraw::Init(catalog, world)", "Unidraw/unidraw.c"},
                {0.42f, 1, "ed = new OverlayEditor(initial_file)", "drawtool/main.c"},
                {0.52f, 2, "OverlayKit::Init(comp, name)  // InitViewer, InitLayout, MakeMenus",
                 "OverlayUnidraw/ovkit.c"},
                {0.62f, 1, "unidraw->Open(ed)", "drawtool/main.c"},
                {0.72f, 1, "unidraw->Run()", "drawtool/main.c"},
                {0.80f, 2, "while (alive()) { session->read(e); e.handle();", "Unidraw/unidraw.c"},
                {0.88f, 2, "  Process(); Sweep(); if (updated()) Update(true); }",
                 "Unidraw/unidraw.c"}},
      .showcase = 0.8f,
  });

  // 2 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Glyphs",
      .title = "Glyphs: one lightweight object for everything on screen",
      .layer = "InterViews 3.1 (include/InterViews)",
      .where = "include/InterViews/glyph.h: Glyph",
      .steps = {"Glyph is the base of every visual: request() a size, allocate() space, draw(), pick(). Glyphs own no window.",
                "PolyGlyph holds children (Box, Deck, ScrollBox); a Box leaves the arithmetic to its Layout.",
                "MonoGlyph wraps one child: Border, Patch, Shadow and InputHandler add a feature and forward the rest.",
                "Character is tiny: a char, a shared Font and a Color. Its position comes from the Allocation."},
      .duration = 9.f,
      .accent = kCompositeC,
      .active = {P::kWindow, P::kGlyph, P::kPolyGlyph, P::kMonoGlyph, P::kCharacter, P::kFont,
                 P::kCanvas, P::kInputHandler},
      .patterns = {{Pt::kComposite, "Component: Glyph · Composite: PolyGlyph (Box, Deck) · "
                                    "Leaf: Character",
                    {P::kGlyph, P::kPolyGlyph, P::kCharacter}},
                   {Pt::kDecorator, "Decorator: MonoGlyph → Border, Patch, Shadow, "
                                    "InputHandler; all are Glyphs themselves",
                    {P::kMonoGlyph, P::kInputHandler}},
                   {Pt::kFlyweight, "Flyweight: Character, shared Font (Font::lookup) · "
                                    "extrinsic state: the Allocation",
                    {P::kCharacter, P::kFont}}},
      .flows = {{P::kWindow, P::kGlyph, "request(shape)", 0.02f, 0.22f, kCompositeC},
                {P::kGlyph, P::kPolyGlyph, "allocate(c, a, ext)", 0.16f, 0.4f, kCompositeC},
                {P::kPolyGlyph, P::kMonoGlyph, "allocate", 0.34f, 0.52f, kCompositeC},
                {P::kMonoGlyph, P::kCharacter, "draw(c, a)", 0.48f, 0.7f, kDecoratorC},
                {P::kCharacter, P::kFont, "shared Font*", 0.62f, 0.82f, kFlyweightC},
                {P::kCharacter, P::kCanvas, "character(font, c, …)", 0.76f, 1.f, kFlyweightC,
                 0.55f, 1}},
      .trace = {{0.02f, 0, "Window::default_geometry() → glyph_->request(shape_)", "IV-X11/xwindow.c"},
                {0.10f, 1, "Box::request(req) → layout_->request(count, r, requisition_)",
                 "InterViews/box.c"},
                {0.20f, 0, "WindowRep::resize() → glyph_->allocate(canvas_, allocation_, ext)",
                 "IV-X11/xwindow.c"},
                {0.28f, 1, "Box::allocate(c, a, ext) → layout_->allocate(a, n, r, allocs)",
                 "InterViews/box.c"},
                {0.36f, 2, "MonoGlyph::allocate(c, a, ext) → body()->allocate(c, a, ext)",
                 "InterViews/monoglyph.c"},
                {0.48f, 0, "Window::repair() → glyph_->draw(canvas_, allocation_)",
                 "IV-X11/xwindow.c"},
                {0.56f, 1, "Box::draw(c, a) → each component->draw(c, allocation)",
                 "InterViews/box.c"},
                {0.64f, 2, "Border::draw(c, a): 4 × c->fill_rect(…); MonoGlyph::draw(c, a)", "InterViews/border.c"},
                {0.72f, 3, "Character::draw(c, a)", "InterViews/character.c"},
                {0.80f, 4, "c->character(font_, c_, width, color_, a.x(), a.y())",
                 "IV-X11/xcanvas.c"},
                {0.90f, 0, "Font::lookup(\"...\")  // one shared FontRep per name",
                 "IV-X11/xfont.c"}},
      .showcase = 0.8f,
  });

  // 3 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Kits & reps",
      .title = "Kits create the look; reps hide the window system",
      .layer = "IV-look kits and the IV-X11 implementation classes",
      .where = "InterViews/kit.c: WidgetKitImpl::make_kit",
      .steps = {"WidgetKit::instance() makes one kit from the \"gui\" style: MFKit, OLKit, SMFKit or MonoKit.",
                "Code asks the abstract kit for push_button(), menubar()… and gets glyphs in that look.",
                "LayoutKit::instance() arranges them: hbox, vbox, overlay, deck, glue, margin.",
                "Window, Canvas, Painter and Font each hold a rep; only the IV-X11 reps call Xlib."},
      .duration = 9.f,
      .accent = kFactoryC,
      .active = {P::kWidgetKit, P::kLayoutKit, P::kInputHandler, P::kPolyGlyph, P::kSession,
                 P::kWindow, P::kWindowRep, P::kCanvas, P::kCanvasRep, P::kPainter,
                 P::kPainterRep, P::kFont, P::kFontRep, P::kXlib},
      .patterns = {{Pt::kAbstractFactory, "AbstractFactory: WidgetKit · Concrete: MFKit, "
                                          "OLKit, SMFKit, MonoKit · Products: glyphs",
                    {P::kWidgetKit, P::kLayoutKit}},
                   {Pt::kSingleton, "WidgetKit::instance(), LayoutKit::instance(), "
                                    "DialogKit::instance(), Session::instance()",
                    {P::kWidgetKit, P::kLayoutKit, P::kSession}},
                   {Pt::kBridge, "Abstraction: Window, Canvas, Painter, Font · "
                                 "Implementor: WindowRep, CanvasRep, PainterRep, FontRep",
                    {P::kWindow, P::kCanvas, P::kPainter, P::kFont, P::kWindowRep,
                     P::kCanvasRep, P::kPainterRep, P::kFontRep}}},
      .flows = {{P::kSession, P::kWidgetKit, "style \"gui\"", 0.02f, 0.2f, kSingletonC},
                {P::kWidgetKit, P::kInputHandler, "push_button()", 0.14f, 0.36f, kFactoryC},
                {P::kLayoutKit, P::kPolyGlyph, "hbox() · vbox()", 0.26f, 0.46f, kFactoryC,
                 0.5f, 1},
                {P::kWindow, P::kWindowRep, "rep()", 0.44f, 0.6f, kBridgeC},
                {P::kCanvas, P::kCanvasRep, "rep()", 0.5f, 0.68f, kBridgeC},
                {P::kPainter, P::kPainterRep, "Rep()", 0.56f, 0.74f, kBridgeC},
                {P::kFont, P::kFontRep, "rep(display)", 0.62f, 0.8f, kBridgeC},
                {P::kWindowRep, P::kXlib, "XCreateWindow", 0.74f, 1.f, kBridgeC, 0.5f, 1}},
      .trace = {{0.02f, 0, "WidgetKit::instance()", "InterViews/kit.c"},
                {0.08f, 1, "WidgetKitImpl::make_kit()", "InterViews/kit.c"},
                {0.14f, 2, "style()->find_attribute(\"gui\", gui)  // \"Motif\"", "InterViews/kit.c"},
                {0.20f, 2, "return new MFKit;", "InterViews/kit.c"},
                {0.30f, 0, "kit.push_button(\"OK\", action)  // an MF-look Button glyph",
                 "IV-look/"},
                {0.38f, 0, "LayoutKit::instance()->hbox(label, hglue, button)",
                 "InterViews/layout.c"},
                {0.50f, 0, "ApplicationWindow(glyph)->map() → Window::bind()", "IV-X11/xwindow.c"},
                {0.60f, 1, "WindowRep::do_bind(): XCreateWindow(dpy, parent, …)",
                 "IV-X11/xwindow.c"},
                {0.72f, 0, "Canvas::fill_rect(…) → CanvasRep: XFillRectangle(…)",
                 "IV-X11/xcanvas.c"},
                {0.82f, 0, "Painter::Rect(c, x0, y0, x1, y1) → XDrawRectangle(…)",
                 "IV-2_6/xpainter.c"},
                {0.92f, 0, "Font::lookup(name) → FontRep (XFontStruct)", "IV-X11/xfont.c"}},
      .showcase = 0.72f,
  });

  // 4 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Events",
      .title = "One event loop: glyphs are picked, observers are told",
      .layer = "InterViews session, events, handlers and observers",
      .where = "InterViews/session.c: Session::run",
      .steps = {"Session::run() loops read(e); e.handle(). Display::get() wraps the next X event.",
                "Event::handle() asks Window::target(): glyph_->pick() walks the tree to the Handler that was hit.",
                "InputHandler turns raw events into press(), drag(), release() and keystroke().",
                "A Button sets its TelltaleState; notify() updates every Observer, then its Action executes."},
      .duration = 9.f,
      .accent = kObserverC,
      .active = {P::kXlib, P::kDisplay, P::kSession, P::kEvent, P::kWindow, P::kGlyph,
                 P::kInputHandler, P::kObservable, P::kMonoGlyph},
      .patterns = {{Pt::kObserver, "Subject: Observable (TelltaleState, Adjustable) · "
                                   "Observer: Button, Telltale, ScrollBar",
                    {P::kObservable, P::kMonoGlyph}},
                   {Pt::kCommand, "Command: Action::execute() · Macro chains actions · "
                                  "ActionCallback(T) binds a member function",
                    {P::kInputHandler}},
                   {Pt::kSingleton, "Session::instance() owns the loop; Display holds the "
                                    "one X connection",
                    {P::kSession}}},
      .flows = {{P::kXlib, P::kDisplay, "XNextEvent", 0.02f, 0.18f, kLayersC},
                {P::kDisplay, P::kSession, "Event", 0.12f, 0.3f, kLayersC},
                {P::kSession, P::kEvent, "e.handle()", 0.26f, 0.42f, kLayersC},
                {P::kEvent, P::kWindow, "target(e)", 0.38f, 0.54f, kCompositeC},
                {P::kWindow, P::kGlyph, "pick(c, a, 0, hit)", 0.5f, 0.68f, kCompositeC, 0.5f, 2},
                {P::kEvent, P::kInputHandler, "h->event(e)", 0.64f, 0.8f, kCommandC, 0.5f, 1},
                {P::kInputHandler, P::kObservable, "state->set(…)", 0.76f, 0.9f, kObserverC,
                 0.5f, 1},
                {P::kObservable, P::kMonoGlyph, "update(obs)", 0.86f, 1.f, kObserverC, 0.5f, 2}},
      .trace = {{0.02f, 0, "Session::run()", "InterViews/session.c"},
                {0.08f, 1, "Session::read(e) → Display::get(e) → XNextEvent(dpy, &xe)",
                 "IV-X11/xwindow.c"},
                {0.26f, 1, "Event::handle()", "IV-X11/xevent.c"},
                {0.34f, 2, "Event::handler() → Window::target(e)", "IV-X11/xwindow.c"},
                {0.44f, 3, "glyph_->pick(canvas_, allocation_, 0, hit); hit.handler()",
                 "IV-X11/xwindow.c"},
                {0.60f, 2, "h->event(e)", "IV-X11/xevent.c"},
                {0.66f, 3, "InputHandlerImpl::event(e) → press / drag / release",
                 "InterViews/input.c"},
                {0.74f, 4, "Button::release(e): s->set(TelltaleState::is_chosen, …)",
                 "InterViews/button.c"},
                {0.82f, 5, "Observable::notify() → each Observer::update(this)",
                 "InterViews/observe.c"},
                {0.92f, 4, "action()->execute()  // e.g. ActionCallback(App)", "InterViews/button.c"}},
      .showcase = 0.83f,
  });

  // 5 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Subjects & views",
      .title = "Components are subjects; views observe and draw them",
      .layer = "Unidraw components, views and structured graphics",
      .where = "Unidraw/component.c: Component::Notify",
      .steps = {"A Component models one thing: a rectangle, a node, a drawing. GraphicComps nests them.",
                "Views are separate: a GraphicView attaches to its subject; Notify() calls Update() on each view.",
                "Views are made by ClassId: Component::Create(category) asks the Creator for the matching view.",
                "Each view owns a Graphic; Picture nests Graphics as GraphicComps nests components."},
      .duration = 9.f,
      .accent = kObserverC,
      .active = {P::kComponent, P::kGraphicComps, P::kGraphicView, P::kGraphic, P::kPicture,
                 P::kCreator, P::kOverlayComp, P::kOverlayView, P::kDamage},
      .patterns = {{Pt::kObserver, "Subject: Component · Observer: ComponentView "
                                   "(GraphicView, PostScriptView, OverlayView)",
                    {P::kComponent, P::kGraphicView, P::kOverlayComp, P::kOverlayView}},
                   {Pt::kComposite, "GraphicComps ▸ GraphicComp · Picture ▸ Graphic · "
                                    "GraphicViews ▸ GraphicView",
                    {P::kGraphicComps, P::kPicture, P::kGraphic}},
                   {Pt::kFactoryMethod, "Creator::Create(ClassId) picks the view class for "
                                        "a subject + category (IdrawCreator, OverlayCreator)",
                    {P::kCreator}}},
      .flows = {{P::kGraphicComps, P::kComponent, "Append(comp)", 0.02f, 0.18f, kCompositeC},
                {P::kComponent, P::kCreator, "Create(view category)", 0.14f, 0.36f,
                 kFactoryMethodC, 0.4f},
                {P::kCreator, P::kGraphicView, "new RectView", 0.3f, 0.5f, kFactoryMethodC,
                 0.5f, 1},
                {P::kComponent, P::kGraphicView, "Notify() → Update()", 0.5f, 0.7f, kObserverC,
                 0.5f, 2},
                {P::kGraphicView, P::kDamage, "IncurDamage(g)", 0.64f, 0.82f, kObserverC,
                 0.5f, 3},
                {P::kGraphic, P::kPicture, "Append(g)", 0.7f, 0.88f, kCompositeC},
                {P::kOverlayComp, P::kOverlayView, "Notify()", 0.8f, 1.f, kObserverC}},
      .trace = {{0.02f, 0, "GraphicComps::Append(comp)", "Unidraw/grcomp.c"},
                {0.12f, 0, "view = (GraphicView*) comp->Create(COMPONENT_VIEW)",
                 "Unidraw/component.c"},
                {0.20f, 1, "unidraw->GetCatalog()->GetCreator()->Create(Combine(RECT_COMP, "
                           "COMPONENT_VIEW))",
                 "Unidraw/component.c"},
                {0.30f, 2, "IdrawCreator::Create(ClassId id)  // switch (id) … new RectView",
                 "UniIdraw/idcreator.c"},
                {0.40f, 0, "comp->Attach(view)", "Unidraw/component.c"},
                {0.52f, 0, "Component::Notify()", "Unidraw/component.c"},
                {0.58f, 1, "for each attached view: view->Update()", "Unidraw/component.c"},
                {0.64f, 2, "RectView::Update(): IncurDamage(rect);", "Unidraw/rect.c"},
                {0.72f, 3, "*rect = *GetRectComp()->GetGraphic(); IncurDamage(rect)",
                 "Unidraw/rect.c"},
                {0.80f, 3, "GraphicView::IncurDamage(g) → viewer->GetDamage()->Incur(g)",
                 "Unidraw/grview.c"},
                {0.90f, 0, "Picture::Append(g)  // views mirror the subject tree",
                 "Unidraw/picture.c"}},
      .showcase = 0.72f,
  });

  // 6 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Tool → Command",
      .title = "A mouse drag: tool → manipulator → command",
      .layer = "Unidraw controllers: Viewer, Tool, Manipulator, Command",
      .where = "Unidraw/viewer.c: Viewer::UseTool",
      .steps = {"A press reaches Viewer::Handle(), which uses the editor's current Tool on it.",
                "CreateManipulator() gets a Manipulator for the view under the pointer: DragManip, VertexManip…",
                "Viewer::Manipulate(): Grasp, Manipulating per motion, Effect. Rubber-banding only.",
                "InterpretManipulator() makes a MoveCmd; Execute() lets each component Interpret() it and Notify().",
                "Reversible commands are logged: Unidraw::Log() adds them to the history."},
      .duration = 10.f,
      .accent = kCommandC,
      .active = {P::kEvent, P::kViewer, P::kTool, P::kManipulator, P::kCommand, P::kComponent,
                 P::kGraphicView, P::kDamage, P::kUnidraw, P::kSelection},
      .patterns = {{Pt::kStrategy, "Context: Viewer · Strategy: Tool (SelectTool, MoveTool, "
                                   "GraphicCompTool…) and its Manipulator",
                    {P::kViewer, P::kTool, P::kManipulator}},
                   {Pt::kCommand, "Command: MoveCmd, PasteCmd… · Invoker: UseTool, KeyMap, "
                                  "menus · Receiver: Component::Interpret",
                    {P::kCommand, P::kComponent}},
                   {Pt::kObserver, "Component::Notify → GraphicView::Update → Damage::Incur",
                    {P::kGraphicView, P::kDamage}}},
      .flows = {{P::kEvent, P::kViewer, "Handle(e)", 0.02f, 0.14f, kLayersC},
                {P::kViewer, P::kTool, "CreateManipulator", 0.1f, 0.26f, kStrategyC},
                {P::kTool, P::kManipulator, "DragManip", 0.2f, 0.34f, kStrategyC},
                {P::kViewer, P::kManipulator, "Grasp · Manipulating · Effect", 0.3f, 0.52f,
                 kStrategyC, 0.5f, 1},
                {P::kTool, P::kCommand, "InterpretManipulator → MoveCmd", 0.5f, 0.64f,
                 kCommandC, 0.5f, 2},
                {P::kCommand, P::kComponent, "Interpret(cmd)", 0.62f, 0.76f, kCommandC, 0.4f},
                {P::kComponent, P::kGraphicView, "Notify → Update", 0.72f, 0.86f, kObserverC,
                 0.5f, 1},
                {P::kGraphicView, P::kDamage, "Incur(g)", 0.8f, 0.92f, kObserverC, 0.5f, 3},
                {P::kCommand, P::kUnidraw, "Log()", 0.88f, 1.f, kCommandC, 0.5f, 1}},
      .trace = {{0.02f, 0, "Viewer::Handle(e) → UseTool(CurTool(), e)", "Unidraw/viewer.c"},
                {0.10f, 1, "m = t->CreateManipulator(this, e, rel)", "Unidraw/viewer.c"},
                {0.18f, 2, "MoveTool::CreateManipulator → gv->CreateManipulator  // DragManip",
                 "Unidraw/move.c"},
                {0.30f, 1, "Manipulate(m, e): m->Grasp(e);", "Unidraw/viewer.c"},
                {0.38f, 2, "do { Read(e); } while (m->Manipulating(e)); m->Effect(e)",
                 "Unidraw/viewer.c"},
                {0.50f, 1, "cmd = t->InterpretManipulator(m)  // new MoveCmd(ed, dx, dy)",
                 "Unidraw/move.c"},
                {0.60f, 1, "cmd->Execute()  // each comp in the clipboard", "Unidraw/command.c"},
                {0.66f, 2, "GraphicComp::Interpret(cmd): gr->Translate(dx, dy); Notify()",
                 "Unidraw/grcomp.c"},
                {0.74f, 3, "RectView::Update() → IncurDamage(rect)", "Unidraw/rect.c"},
                {0.82f, 4, "viewer->GetDamage()->Incur(g)", "Unidraw/grview.c"},
                {0.88f, 1, "if (cmd->Reversible()) cmd->Log() → unidraw->Log(cmd)",
                 "Unidraw/viewer.c"}},
      .showcase = 0.66f,
  });

  // 7 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Update & undo",
      .title = "Repair the damage, remember the past",
      .layer = "Unidraw update cycle, constraints and undo history",
      .where = "Unidraw/unidraw.c: Unidraw::DoUpdate, Unidraw::Undo",
      .steps = {"Commands only mark the world as changed; Run() calls Update(true) once for all of them.",
                "DoUpdate() lets CSolver solve the connector network first, so linked parts move together.",
                "Each Viewer repairs its Damage: only invalid areas are redrawn, through the static Painter.",
                "Undo: Unexecute() the last command, move it to the future list. The first change is MacroCmd(cmd, DirtyCmd)."},
      .duration = 10.f,
      .accent = kMediatorC,
      .active = {P::kUnidraw, P::kCSolver, P::kConnector, P::kEditor, P::kViewer, P::kDamage,
                 P::kGraphic, P::kPainter, P::kPainterRep, P::kCommand, P::kMacroCmd,
                 P::kSelection},
      .patterns = {{Pt::kCommand, "Unexecute, Reversible · per-drawing past/future lists · "
                                  "MacroCmd composes commands",
                    {P::kCommand, P::kMacroCmd, P::kUnidraw}},
                   {Pt::kMediator, "Mediator: CSolver · Colleagues: Connector, ConnectorView; "
                                   "they never move each other directly",
                    {P::kConnector, P::kCSolver}},
                   {Pt::kBridge, "Graphic draws with the static Painter _p; the X11 side "
                                 "(PainterRep, Xlib) stays behind it",
                    {P::kGraphic, P::kPainter, P::kPainterRep}}},
      .flows = {{P::kUnidraw, P::kCSolver, "csolver->Solve()", 0.02f, 0.18f, kMediatorC, 0.3f},
                {P::kCSolver, P::kConnector, "Transmit", 0.12f, 0.28f, kMediatorC},
                {P::kUnidraw, P::kEditor, "Update()", 0.26f, 0.38f, kLayersC},
                {P::kEditor, P::kViewer, "Update()", 0.34f, 0.46f, kLayersC},
                {P::kViewer, P::kDamage, "Repair()", 0.44f, 0.58f, kObserverC, 0.5f, 1},
                {P::kDamage, P::kGraphic, "DrawClipped(c, …)", 0.54f, 0.68f, kCompositeC,
                 0.5f, 1},
                {P::kGraphic, P::kPainter, "_p->Rect(c, …)", 0.64f, 0.78f, kBridgeC, 0.5f, 1},
                {P::kPainter, P::kPainterRep, "XDrawRectangle", 0.74f, 0.86f, kBridgeC},
                {P::kUnidraw, P::kCommand, "Undo: Unexecute()", 0.84f, 1.f, kCommandC, 0.5f, 2},
                {P::kCommand, P::kMacroCmd, "(cmd, DirtyCmd)", 0.9f, 1.f, kCommandC}},
      .trace = {{0.02f, 0, "Unidraw::Update(true) → DoUpdate()", "Unidraw/unidraw.c"},
                {0.08f, 1, "csolver->Solve()", "Unidraw/csolver.c"},
                {0.26f, 1, "for each open editor: ed->Update()", "Unidraw/unidraw.c"},
                {0.36f, 2, "Viewer::Update(): _selection->Hide(); _viewerView->Update()",
                 "Unidraw/viewer.c"},
                {0.46f, 3, "_damage->Repair()", "Unidraw/viewer.c"},
                {0.52f, 4, "Damage::DrawAreas(): _output->ClearRect(…)", "Unidraw/damage.c"},
                {0.58f, 5, "_graphic->DrawClipped(canvas, l, b, r, t)", "Unidraw/damage.c"},
                {0.66f, 6, "Rect::draw(c, gs) → _p->Rect(c, _x0, _y0, _x1, _y1)",
                 "Unidraw/polygons.c"},
                {0.76f, 7, "Painter::Rect → XDrawRectangle(dpy, d, gc, …)", "IV-2_6/xpainter.c"},
                {0.82f, 2, "_selection->Show()", "Unidraw/viewer.c"},
                {0.88f, 0, "Unidraw::Undo(comp, 1): cmd->Unexecute();", "Unidraw/unidraw.c"},
                {0.94f, 1, "past->Remove(cur); future->Prepend(cur)", "Unidraw/unidraw.c"}},
      .showcase = 0.7f,
  });

  // 8 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "Catalog",
      .title = "Files: the catalog saves a subject through a view",
      .layer = "Unidraw Catalog/Creator and OverlayUnidraw scripts",
      .where = "OverlayUnidraw/ovcatalog.c: OverlayCatalog::Save",
      .steps = {"Catalog is the only object that touches files: Save and Retrieve components, commands, tools.",
                "Unidraw's format writes each ClassId first; reading calls Creator::Create(ClassId, istream&).",
                "drawtool saves scripts: a SCRIPT_VIEW (an ExternView) is attached and Emit(out) writes it.",
                "OvImportCmd::Import() parses scripts back. New shapes are clones: GraphicCompTool Copy()s a prototype."},
      .duration = 9.f,
      .accent = kFactoryMethodC,
      .active = {P::kOverlayEditor, P::kCatalog, P::kCreator, P::kComponent, P::kOverlayComp,
                 P::kOverlayScript, P::kTool},
      .patterns = {{Pt::kFactoryMethod, "Creator::Create(ClassId, istream&) · IdrawCreator, "
                                        "OverlayCreator, GraphCreator extend the switch",
                    {P::kCreator, P::kCatalog}},
                   {Pt::kPrototype, "Copy(): GraphicCompTool clones its prototype component; "
                                    "Command, Tool, Graphic copy too",
                    {P::kTool, P::kComponent}},
                   {Pt::kStrategy, "ExternView: OverlayScript, PostScriptView — "
                                   "interchangeable Emit() of the same subject",
                    {P::kOverlayScript}}},
      .flows = {{P::kOverlayEditor, P::kCatalog, "Save(comp, name)", 0.02f, 0.2f,
                 kFactoryMethodC, 0.35f},
                {P::kCatalog, P::kComponent, "Create(SCRIPT_VIEW)", 0.16f, 0.36f,
                 kFactoryMethodC, 0.5f, 1},
                {P::kComponent, P::kCreator, "Create(ClassId)", 0.32f, 0.5f, kFactoryMethodC,
                 0.5f, 2},
                {P::kCreator, P::kOverlayScript, "new OverlaysScript", 0.46f, 0.62f,
                 kFactoryMethodC, 0.5f, 1},
                {P::kOverlayScript, P::kOverlayComp, "Definition(out)", 0.6f, 0.8f, kStrategyC},
                {P::kTool, P::kComponent, "prototype->Copy()", 0.76f, 0.9f, kPrototypeC,
                 0.5f, 3},
                {P::kCatalog, P::kCreator, "Retrieve: Create(id, in)", 0.86f, 1.f,
                 kFactoryMethodC}},
      .trace = {{0.02f, 0, "OverlayCatalog::Save(comp, name)", "OverlayUnidraw/ovcatalog.c"},
                {0.08f, 1, "if (UnidrawFormat(name)) return Catalog::Save(comp, name)",
                 "OverlayUnidraw/ovcatalog.c"},
                {0.16f, 1, "ev = (ExternView*) comp->Create(SCRIPT_VIEW)",
                 "OverlayUnidraw/ovcatalog.c"},
                {0.30f, 2, "OverlayCreator::Create(OVERLAYS_SCRIPT) → new OverlaysScript",
                 "OverlayUnidraw/ovcreator.c"},
                {0.46f, 1, "comp->Attach(ev); ev->Update()", "OverlayUnidraw/ovcatalog.c"},
                {0.56f, 1, "ev->Emit(out)", "OverlayUnidraw/ovcatalog.c"},
                {0.62f, 2, "OverlaysScript::Definition(out): \"picture(\" + each child view",
                 "OverlayUnidraw/scriptview.c"},
                {0.78f, 0, "OverlayCatalog::Retrieve(file, comp)", "OverlayUnidraw/ovcatalog.c"},
                {0.84f, 1, "OvImportCmd::Import(in, empty)  // parse the script",
                 "OverlayUnidraw/ovimport.c"},
                {0.92f, 0, "Catalog::ReadObject(in) → _creator->Create(classId, in, map, id)",
                 "Unidraw/catalog.c"}},
      .showcase = 0.66f,
  });

  // 9 -----------------------------------------------------------------------
  t.push_back(Tour{
      .name = "ComTerp",
      .title = "Scripting: text becomes the same commands",
      .layer = "ComTerp + ComUnidraw (comdraw, drawserv)",
      .where = "ComUnidraw/grfunc.c: CreateRectFunc::execute",
      .steps = {"comdraw reads rect(10,10,100,100) from stdin or a socket; ComTerpServ::run() evaluates it.",
                "The parser makes postfix tokens; eval_expr() pushes ComValues and calls the bound ComFunc.",
                "ComEditor::AddCommands binds names: add_command(\"rect\", new CreateRectFunc(…)).",
                "CreateRectFunc builds a RectOvComp and a PasteCmd: the same Command path as a mouse drag.",
                "drawserv also sends the command as script text to every linked peer (DistributeCmdString)."},
      .duration = 10.f,
      .accent = kInterpreterC,
      .active = {P::kComdraw, P::kDrawserv, P::kComEditor, P::kComTerp, P::kComValue,
                 P::kComFunc, P::kUnidrawFunc, P::kOverlayComp, P::kCommand, P::kUnidraw},
      .patterns = {{Pt::kInterpreter, "Expressions: postfix tokens · Context: the ComTerp "
                                      "value stack · Operations: ComFunc",
                    {P::kComTerp, P::kComValue}},
                   {Pt::kCommand, "ComFunc::execute(): every script function is an object "
                                  "ComTerp calls through one interface",
                    {P::kComFunc}},
                   {Pt::kAdapter, "UnidrawFunc adapts Unidraw Commands to ComFunc: "
                                  "execute_log(cmd) → unidraw->ExecuteCmd(cmd)",
                    {P::kUnidrawFunc, P::kCommand}}},
      .flows = {{P::kComdraw, P::kComEditor, "SetComTerp(…)", 0.02f, 0.16f, kLayersC},
                {P::kComEditor, P::kComTerp, "add_command(\"rect\", …)", 0.1f, 0.26f,
                 kCommandC, 0.5f, 1},
                {P::kComTerp, P::kComValue, "push args", 0.28f, 0.42f, kInterpreterC, 0.5f, 2},
                {P::kComTerp, P::kComFunc, "execute()", 0.38f, 0.52f, kCommandC},
                {P::kComFunc, P::kUnidrawFunc, "CreateRectFunc", 0.48f, 0.6f, kAdapterC,
                 0.5f, 1},
                {P::kUnidrawFunc, P::kOverlayComp, "new RectOvComp", 0.56f, 0.7f, kAdapterC,
                 0.5f, 2},
                {P::kUnidrawFunc, P::kCommand, "PasteCmd · execute_log", 0.66f, 0.8f,
                 kAdapterC, 0.5f, 3},
                {P::kCommand, P::kUnidraw, "ExecuteCmd(cmd)", 0.78f, 0.9f, kCommandC, 0.5f, 1},
                {P::kUnidraw, P::kDrawserv, "DistributeCmdString", 0.88f, 1.f, kObserverC,
                 0.5f, 2}},
      .trace = {{0.02f, 0, "ComTerpServ::run(one_expr, nested)", "ComTerp/comterpserv.c"},
                {0.10f, 1, "ComTerp::read_expr() → parser(…)  // scanner → postfix tokens",
                 "ComTerp/comterp.c"},
                {0.26f, 1, "ComTerp::eval_expr(nested) → eval_expr_internals()",
                 "ComTerp/comterp.c"},
                {0.34f, 2, "push_stack(ComValue(10)); push_stack(ComValue(10)); …",
                 "ComTerp/comterp.c"},
                {0.40f, 2, "func->execute()", "ComTerp/comterp.c"},
                {0.48f, 3, "CreateRectFunc::execute()", "ComUnidraw/grfunc.c"},
                {0.56f, 4, "comp = new RectOvComp(new SF_Rect(x0, y0, x1, y1, stdgraphic))",
                 "ComUnidraw/grfunc.c"},
                {0.64f, 4, "cmd = new PasteCmd(_ed, new Clipboard(comp))", "ComUnidraw/grfunc.c"},
                {0.70f, 4, "execute_log(cmd) → unidraw->ExecuteCmd(cmd)", "ComUnidraw/unifunc.c"},
                {0.80f, 5, "cmd->Execute(); if (cmd->Reversible()) cmd->Log()",
                 "Unidraw/unidraw.c"},
                {0.90f, 5, "DrawServ::ExecuteCmd → DistributeCmdString(script, link)",
                 "DrawServ/drawserv.c"}},
      .showcase = 0.72f,
  });
  return t;
}

}  // namespace

const PatternInfo& Info(Pattern p) {
  static const PatternInfo kInfo[] = {
      {"Layers", "Ly", kLayersC},
      {"Facade", "Fa", kFacadeC},
      {"Singleton", "Si", kSingletonC},
      {"Composite", "Co", kCompositeC},
      {"Decorator", "De", kDecoratorC},
      {"Flyweight", "Fw", kFlyweightC},
      {"Abstract Factory", "AF", kFactoryC},
      {"Bridge", "Br", kBridgeC},
      {"Observer", "Ob", kObserverC},
      {"Command", "Cm", kCommandC},
      {"Strategy", "St", kStrategyC},
      {"Mediator", "Me", kMediatorC},
      {"Factory Method", "FM", kFactoryMethodC},
      {"Prototype", "Pr", kPrototypeC},
      {"Interpreter", "In", kInterpreterC},
      {"Adapter", "Ad", kAdapterC},
      {"Template Method", "TM", kTemplateC},
  };
  static_assert(sizeof kInfo / sizeof kInfo[0] == static_cast<size_t>(Pattern::kCount));
  return kInfo[static_cast<int>(p)];
}

const std::vector<Tour>& DesignTours() {
  static const std::vector<Tour> tours = MakeTours();
  return tours;
}

float TotalDuration() {
  float t = 0.f;
  for (const Tour& s : DesignTours()) t += s.duration;
  return t;
}

Cursor CursorAt(float seconds) {
  const auto& tours = DesignTours();
  float t = std::max(0.f, seconds);
  for (int i = 0; i < static_cast<int>(tours.size()); ++i) {
    if (t < tours[i].duration) return {i, t / tours[i].duration};
    t -= tours[i].duration;
  }
  return {static_cast<int>(tours.size()) - 1, 1.f};
}

float SecondsAt(Cursor c) {
  const auto& tours = DesignTours();
  float t = 0.f;
  for (int i = 0; i < c.stage && i < static_cast<int>(tours.size()); ++i) t += tours[i].duration;
  if (c.stage < static_cast<int>(tours.size())) t += c.progress * tours[c.stage].duration;
  return t;
}

std::vector<TraceLine> TraceUpTo(Cursor c) {
  std::vector<TraceLine> out;
  const auto& tours = DesignTours();
  if (c.stage < 0 || c.stage >= static_cast<int>(tours.size())) return out;
  for (const TraceLine& l : tours[c.stage].trace) {
    if (l.at <= c.progress) out.push_back(l);
  }
  return out;
}

}  // namespace ivviz::scene
