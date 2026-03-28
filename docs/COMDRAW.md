# comdraw: Technical Overview

`comdraw` is a drawing editor that extends the Unidraw framework with a command interpreter (`ComTerp`), allowing for both interactive and scripted control.

## C++ Components Used
- **InterViews / Unidraw**: Core GUI and structured graphics frameworks.
- **OverlayUnidraw**: A library extending Unidraw with "overlay" capabilities (multiple layers, more complex attributes).
- **ComTerp**: The command interpreter engine.
- **ComUnidraw**: A library that bridges Unidraw and ComTerp, defining interpreter functions that manipulate Unidraw objects.
- **ACE (Optional)**: Used for network communication and asynchronous event handling.

## Startup Process (Call Graph)
1.  **`main(argc, argv)`**: Entry point in `src/comdraw/main.c`.
2.  **`OverlayCreator creator`**: Class factory for overlay-capable components.
3.  **`new OverlayUnidraw(...)`**: 
    - Initializes the `OverlayCatalog`.
    - Creates the `World` and InterViews session.
4.  **Network Setup (if ACE enabled)**:
    - `UnidrawImportAcceptor`: Opens a port to receive graphical data.
    - `UnidrawComterpAcceptor`: Opens a port to receive ComTerp scripts.
5.  **`new ComEditor(initial_file)`**: 
    - `ComEditor` is a subclass of `OverlayEditor`.
    - It initializes the command interpreter and links it to the editor state.
6.  **`unidraw->Open(ed)`**: Displays the window.
7.  **`unidraw->Run()`**: Enters the event loop.

## Event Traversal and Handling
`comdraw` handles events in two primary ways:

### 1. User Interaction (GUI)
Follows the same path as `idraw`:
X11 -> Session -> Window -> Editor -> Viewer -> Tool -> Command.

### 2. Scripted Interaction (ComTerp)
1.  **Input Source**: Commands can come from `stdin`, a file, or a network socket.
2.  **`ComTerp`**: Parses the text into a sequence of `ComFunc` calls.
3.  **`ComFunc` Execution**: 
    - Many functions in `comdraw` are defined in `ComUnidraw`. 
    - When a function like `line(0,0,10,10)` is executed, it directly creates a `LineComp` and adds it to the editor's component tree.
4.  **Synchronization**: Scripted changes trigger the same notification and redraw mechanisms as GUI changes.

## Widget Refresh and Redrawing
Identical to the Unidraw mechanism used in `idraw`:
1.  **Component Modification**: Triggers notification to views.
2.  **Damage Accumulation**: `OverlayView` objects track changes.
3.  **Redraw**: `Viewer` iterates through its views and calls `draw(canvas)` on the underlying `Graphic` objects.
4.  **Efficiency**: `comdraw` often uses the "overlay" mechanism to redraw only specific layers or objects, improving performance for large drawings.
