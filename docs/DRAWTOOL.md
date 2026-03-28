# drawtool: Technical Overview

`drawtool` is a general-purpose drawing editor that extends `idraw` with "overlay" capabilities, supporting a wider range of file formats and complex object attributes.

## C++ Components Used
- **InterViews / Unidraw**: Foundational frameworks.
- **OverlayUnidraw**: Provides the core functionality for `drawtool`, including layer management and advanced graphic attributes.
- **ComUtil**: Low-level utility library for string and data manipulation.
- **TIFF**: Integrated TIFF library for handling raster images.

## Startup Process (Call Graph)
1.  **`main(argc, argv)`**: Entry point in `src/drawtool/main.c`.
2.  **`OverlayCreator creator`**: Class factory for all graphic and command objects.
3.  **`new OverlayUnidraw(...)`**: 
    - Initializes `OverlayCatalog` for handling multiple document formats.
    - Sets up the InterViews `World` and display connection.
4.  **`new OverlayEditor(initial_file)`**: 
    - Construct the UI, including specialized overlay tools (e.g., attribute tool).
    - Sets up the `Viewer` with support for high-resolution coordinates and complex zooming.
5.  **`unidraw->Open(ed)`**: Displays the application window.
6.  **`unidraw->Run()`**: Starts the event processing loop.

## Event Traversal and Handling
`drawtool` uses the standard Unidraw event flow, with added support for complex attributes:
1.  **Input Dispatched**: X11 -> Session -> Window -> Editor -> Viewer.
2.  **Tool Interaction**: Active `Tool` creates a `Manipulator`.
3.  **Attribute Integration**: Tools in `drawtool` often interact with `AttrDialog` or `ComTerp` state variables to apply metadata or complex properties to components.
4.  **Command Execution**: A `Command` object is created and executed, modifying the `OverlayComp` model.

## Widget Refresh and Redrawing
1.  **Layered Redraw**: `drawtool` can manage drawings as a collection of overlays.
2.  **Damage Tracking**: Each overlay tracks its own damage region.
3.  **Composite Drawing**: When a redraw is triggered, the `Viewer` asks the root `OverlayComp` to draw itself.
4.  **Recursive Paint**: The root component recursively visits each child (each overlay layer) and calls their `draw` methods in back-to-front order.
5.  **Optimized Rendering**: The InterViews `Canvas` handles the clipping and coordinate transformation to ensure only the damaged region is updated on the screen.
