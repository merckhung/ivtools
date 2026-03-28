# idraw: Technical Overview

`idraw` is the classic PostScript drawing editor provided with the InterViews distribution.

## C++ Components Used
- **InterViews**: Foundational GUI toolkit (Glyphs, Kits, Windows, Session).
- **Unidraw**: Structured graphics framework (Editor, Viewer, Catalog, Tools, Commands).
- **UniIdraw**: A library specifically encapsulating `idraw`'s core logic (IdrawComp, IdrawEditor, IdrawCatalog).

## Startup Process (Call Graph)
1.  **`main(argc, argv)`**: Entry point in `src/idraw/main.c`.
2.  **`IdrawCreator creator`**: Instantiates the class factory for creating components.
3.  **`new Unidraw(...)`**: 
    - Creates a `World` object (InterViews session/display management).
    - Creates an `IdrawCatalog` (persistence management).
    - Initializes the `CSolver` (connector solver).
4.  **`new IdrawEditor(initial_file)`**: 
    - `IdrawEditor::Init()` creates the UI layout using `Tray`, `HBox`, `MenuBar`, etc.
    - `InitStateVars()` loads initial brush, font, and color settings.
    - `InitViewer()` creates the `Viewer` that displays the drawing area.
5.  **`unidraw->Open(ed)`**: 
    - Wraps the editor in an `ApplicationWindow`.
    - Maps the window to the X11 display.
6.  **`unidraw->Run()`**: Enters the main event loop.

## Event Traversal and Handling
### Mouse/Keyboard Events:
1.  **X11 Server**: Detects physical event.
2.  **InterViews `Session`**: `session->read(e)` receives the raw X event and wraps it in an `ivEvent` object.
3.  **`e.handle()`**: Dispatches the event to the targeted `ivWindow`.
4.  **`ivWindow`**: Passes the event to its root `Interactor` (the `IdrawEditor`).
5.  **`IdrawEditor`**: The editor delegates events to its children. If the event is in the `Viewer`:
6.  **`Viewer`**: 
    - If it's a keyboard event, it might trigger a `Command` via `KeyMap`.
    - If it's a mouse event, it delegates to the currently active `Tool` (e.g., `SelectTool`, `MoveTool`).
7.  **`Tool`**: 
    - Creates a `Manipulator` to handle the direct interaction (e.g., a "rubberband" box).
    - Once the interaction is finished, the Tool generates a `Command` (e.g., `MoveCmd`).
8.  **`Command`**: `Execute()` modifies the `Component` model and registers itself with `Unidraw` for Undo/Redo.

## Widget Refresh and Redrawing
1.  **Damage Tracking**: When a `Component` is modified, it notifies its views. `GraphicView` uses a `Damage` object to record the region that needs redrawing.
2.  **Exposure Events**: When the X server sends an exposure event (window uncovered), or when `Unidraw::Update` is called:
3.  **`Viewer::draw()`**: 
    - Clears the dirty region on the `ivCanvas`.
    - Calls `Graphic::draw(canvas)` on the root `Graphic` component.
4.  **Recursive Draw**: The root `Graphic` (often a `Picture`) recursively calls `draw` on its children.
5.  **InterViews `Canvas`**: Translates the InterViews drawing calls (e.g., `line`, `fill_rect`) into actual X11 primitives or PostScript commands.
