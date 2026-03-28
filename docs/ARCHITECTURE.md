# ivtools Architecture

This document describes the design and architecture of the `ivtools` suite.

`ivtools` is built as a layered system of frameworks and applications, primarily leveraging the InterViews and Unidraw libraries.

## Layered Architecture

### 1. InterViews (Base Framework)
InterViews is the foundational C++ GUI toolkit. Its core design philosophy is based on:
- **Glyphs**: Extremely lightweight graphical objects used to build user interfaces. Unlike traditional widgets, Glyphs do not each own a system window, allowing for thousands of objects without significant overhead.
- **Kits**: Abstract factories (e.g., `WidgetKit`, `LayoutKit`) used to create UI components in a look-and-feel independent manner (supporting Motif, OpenLook, etc.).
- **Resources**: A reference-counting base class (`Resource`) for automatic memory management of shared objects.

### 2. Unidraw (Structured Graphics Framework)
Unidraw sits on top of InterViews and provides a framework for building domain-specific editors (graphical, schematic, etc.). It follows a variation of the Model-View-Controller (MVC) pattern:
- **Component (Model)**: Represents the underlying data and structure of a graphical object.
- **ComponentView (View)**: Handles the graphical representation and rendering of a Component.
- **Tool & Command (Controller)**: `Tool` objects handle user input and interaction (e.g., selection, move, rotate), while `Command` objects encapsulate operations that can be executed, undone, and redone.
- **Catalog**: Manages the persistence (saving/loading) of component hierarchies.

### 3. ComTerp (Command Interpreter)
`ComTerp` (Command TERPreter) is a distributed command interpreter integrated into `ivtools`.
- It allows applications like `comdraw` to be controlled via a scripting language.
- It uses `ComFunc` objects to define executable commands and `ComValue` objects for data passing.
- It supports remote execution, allowing one application to send commands to another over a network.

### 4. Applications (Top Layer)
The final applications are specialized editors built using the components of the frameworks below them:
- **idraw**: The classic InterViews drawing editor, primarily focused on PostScript output.
- **comdraw**: An extension of the drawing editor that integrates `ComTerp`, making it scriptable and externally controllable.
- **drawtool**: A multi-format editor supporting various graphical imports and exports.
- **graphdraw**: A specialized editor for directed and undirected graphs.

## Summary of Design Patterns
- **MVC**: Primarily in Unidraw for separating graphical models from their views and interactions.
- **Command Pattern**: Encapsulating actions as objects to support undo/redo and scripting.
- **Abstract Factory (Kits)**: For look-and-feel independence in InterViews.
- **Flyweight (Glyphs)**: To minimize memory usage for large numbers of small graphical elements.
- **Observer Pattern**: Used for keeping views in sync with their subjects (models).
