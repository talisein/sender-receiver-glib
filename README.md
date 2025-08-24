sender-receiver-glib 
====================

This repo is just where I am exploring P2300 in gtkmm.

Currently it doesn't build with gcc-15, I probably need to update to the latest stdexec.
But it can build in a Fedora 40 toolbox if you install:

```
sudo dnf install meson g++ catch2-devel doxygen python3-sphinx python3-sphinx-copybutton python3-ipython-sphinx python3-myst-parser python3-prompt-toolkit python3-nbsphinx python3-attr gtkmm4.0-devel libcurl-devel
```

Most of the glib wrap was inspired by this std::cpp talk by Ville Voutilain: https://www.youtube.com/watch?v=FX0rbx3wnVo
 
