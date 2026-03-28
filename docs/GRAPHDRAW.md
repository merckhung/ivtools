# graphdraw: Technical Overview

`graphdraw` is a specialized editor for creating and manipulating directed and undirected graphs, built upon the Unidraw framework.

## C++ Components Used
- **InterViews / Unidraw**: Foundational frameworks.
- **GraphUnidraw**: A library extending Unidraw with graph-specific components (nodes, edges).
- **OverlayUnidraw**: Provides the base editor and catalog functionality.
- **TopoFace**: A library for managing topological relationships (connectivity between nodes and edges).

## Startup Process (Call Graph)
1.  **`main(argc, argv)`**: Entry point in `src/graphdraw/main.c`.
2.  **`GraphCreator creator`**: Factory for graph-specific components (e.g., `NodeComp`, `EdgeComp`).
3.  **`new OverlayUnidraw(...)`**: 
    - Initializes the `GraphCatalog` (subclass of `OverlayCatalog`).
    - Sets up the InterViews session.
4.  **`new GraphEditor(initial_file)`**: 
    - Inherits from `OverlayEditor`.
    - Assembles the UI with graph-related tools (node tool, edge tool).
5.  **`unidraw->Open(ed)`**: Maps the window.
6.  **`unidraw->Run()`**: Starts the event loop.

## Event Traversal and Handling
### Creating a Graph:
1.  **Node Tool**: The user selects the `NodeTool` and clicks in the `Viewer`.
2.  **`NodeTool::InterpretManipulator`**: 
    - Creates a `NodeComp` (the model) and a `NodeView` (the view).
    - Generates a `PasteCmd` to add the node to the graph.
3.  **Edge Tool**: The user selects the `EdgeTool` and drags between two nodes.
4.  **Connectivity Handling**:
    - As the user drags, the `EdgeTool` uses the `TopoFace` library to find the underlying nodes.
    - When finished, a `ConnectCmd` is generated.
5.  **`ConnectCmd`**: 
    - Updates the topological model in `TopoFace`.
    - Updates the graphical models in `Unidraw`.

## Widget Refresh and Redrawing
1.  **Automatic Routing**: When a node is moved (via `MoveTool`), its associated edges must be updated.
2.  **`CSolver` (Connector Solver)**: Unidraw's solver detects that the node's position has changed.
3.  **Edge Update**: The solver updates the endpoints of all connected `EdgeComp` objects.
4.  **Notification**: The modified edges notify their views.
5.  **Redraw**: The `Viewer` redraws the affected regions, ensuring nodes and edges remain connected visually.
