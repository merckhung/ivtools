# ivtools: List of C++ Classes

This document provides a non-exhaustive list of key C++ classes within the `ivtools` suite, categorized by their respective frameworks.

## InterViews (Base GUI Framework)

The foundational toolkit for building user interfaces using lightweight objects.

| Class | Description |
| :--- | :--- |
| `Aggregate` | A glyph that can contain multiple other glyphs. |
| `Adjustable` | Protocol for objects that can be scrolled or zoomed. |
| `Box` | A polyglyph that arranges its components along a major axis (HBox for horizontal, VBox for vertical). |
| `Canvas` | An abstract surface for drawing. |
| `Color` | Represents an RGB-alpha color object for output. |
| `Cursor` | Manages the shape and appearance of the mouse pointer. |
| `Display` | Manages the connection to a windowing system (e.g., X11). |
| `Event` | Represents user input events (mouse clicks, key presses). |
| `Font` | Encapsulates a typeface used for rendering text. |
| `Glyph` | The base class for all lightweight visible objects. |
| `Image` | A glyph that displays raster data. |
| `Layout` | Strategy object used by boxes to determine the size and position of their components. |
| `Observable` | Part of the Observer pattern; an object that can be watched for state changes. |
| `Observer` | An object that reacts to changes in an `Observable`. |
| `Placement` | A monoglyph used for precise positioning of another glyph. |
| `Printer` | A specialized canvas for generating PostScript output. |
| `Raster` | Represents a multi-bit-per-pixel image. |
| `Resource` | Base class providing reference counting for memory management. |
| `Rule` | A glyph used to draw horizontal or vertical lines (HRule, VRule). |
| `Window` | Represents a top-level window in the windowing system. |
| `World` | The root object representing the entire screen and session. |

## Unidraw (Structured Graphics Framework)

A framework for building graphical editors using an MVC-based approach.

| Class | Description |
| :--- | :--- |
| `Clipboard` | A container for a set of component subjects used in cut/copy/paste operations. |
| `Command` | Encapsulates an action that can be executed, undone, and redone. |
| `Component` | The "Model" in the MVC pattern; represents the underlying data of a graphical object. |
| `ComponentView` | The "View" in the MVC pattern; handles the rendering of a component. |
| `ControlInfo` | Manages persistent information (names, labels, keys) for UI controls. |
| `CSolver` | A constraint solver used for maintaining connectivity between graphical objects. |
| `Editor` | Base class for the top-level window of a Unidraw application. |
| `Graphic` | Base class for structured graphics objects that maintain state like colors and transforms. |
| `GraphicView` | A specialized view for rendering `Graphic` objects. |
| `GVUpdater` | Ensures that a view is kept in sync with its subject's state. |
| `Iterator` | Marks a position within a data structure (e.g., a list of components). |
| `KeyMap` | Maintains a mapping from keycodes to specific actions or controls. |
| `LineComp` | Represents a line segment component. |
| `Manipulator` | Base class for defining direct-manipulation semantics (e.g., dragging). |
| `PolygonComp` | Represents a polygon graphical component. |
| `PostScriptView` | Handles the generation of PostScript code for a graphical component. |
| `Selection` | Manages the set of currently selected components in an editor. |
| `Tool` | Handles user input to perform specific tasks like selection, scaling, or rotation. |
| `TransferFunct` | Defines how state variables change in response to user input or other stimuli. |

## ComTerp (Command Interpreter)

A distributed command interpreter for scripting and remote application control.

| Class | Description |
| :--- | :--- |
| `AddAssignFunc` | Implements the `+=` addition assignment operator. |
| `AssignFunc` | Implements the `=` assignment operator. |
| `BitAndFunc` | Implements the `&` bitwise AND operator. |
| `ComFunc` | Base class for all executable command/function objects in the interpreter. |
| `ComTerp` | The core command interpreter class that parses and executes scripts. |
| `ComTerpServ` | A server class that allows `ComTerp` to receive commands over a socket. |
| `ComValue` | Represents a polymorphic data value passed between functions. |
| `DecrFunc` | Implements the `--` decrement operator. |
| `DivAssignFunc` | Implements the `/=` division assignment operator. |
| `IncrFunc` | Implements the `++` increment operator. |
| `NumFunc` | Base class for functions that perform numerical operations. |
| `Parser` | Handles the grammatical parsing of the ComTerp language. |
| `Scanner` | Performs lexical analysis (tokenizing) of the input script. |

## OS and Utilities (System Abstraction)

| Class | Description |
| :--- | :--- |
| `Directory` | Provides a system-independent interface for reading file system directories. |
| `File` | Abstract base class for file input and output operations. |
| `Host` | Provides information about the local system (hostname, etc.). |
| `String` | A robust string class with various manipulation utilities. |
| `UList` | A generic circular intrusive doubly-linked list. |
