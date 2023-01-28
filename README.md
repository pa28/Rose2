# Rose2

Simple UI framework for small systems.

## Documentation

Basic [documentation](https://codedocs.xyz/pa28/Rose2/) is a work in progress hosted by [codedocs.xyz](https://codedocs.xyz/)

## Features (design goals)

* Implemented on top of the [Simple DirectMedial Layer](https://www.libsdl.org/) library.
  * This provides compatability with [X11](https://en.wikipedia.org/wiki/X_Window_System)/[Wayland](https://en.wikipedia.org/wiki/Wayland_(protocol))
    desktop environments as
    well as direct graphics on the [Linux Framebuffer](https://en.wikipedia.org/wiki/Linux_framebuffer)
  * C++20 wrapper on the SDL library:
    * Scoped RAII SDL structures are released when they pass out of scope.
    * Guard structures allow temporary changes which are reverted when the guard passes
      out of scope.
* A typical Widget/Gadget based scene graph layout structure.
* Signals and Slots for a callback mechanism based on an extensible Protocol specification.
  * Signals and slots are used to implement an animation system.
* Multiple window support on systems with a desktop window manager.
* Mouse, Keyboard and Touch interaction support.
* Uses Google Material Design icon font to provide icons.
* [SQLite](https://www.sqlite.org/index.html) application setting persistence. 
* [XDG Base Directory](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
  compliant application data storage.

