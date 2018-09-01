#  Dual-Screen PDF Viewer for latex-beamer

[![Build Status](https://travis-ci.org/dannyedel/dspdfviewer.svg?branch=master)](https://travis-ci.org/dannyedel/dspdfviewer)

This is a simple viewer for latex-beamer presentations that are built
with the **show notes on second screen** option of latex-beamer.

It will take your PDF file, split it in a left and right half and
render the two halves individually to the screens.

## Usage
There is a manpage included that explains all the options and keybindings. Most important ones are probably:

Command line:

dspdfviewer [options] *pdf-file*

where the most useful option is probably -f, because it enables you to use the software with a standard pdf (i.e. not specifically built for latex-beamer).

In-Program-Controls:

Left/Right, Mouse Buttons or Mouse Wheel: Back/Forward

S or F12: Swap screens (if you see the wall clock on the projector)

B or . (period): blank/unblank audience screen

Q/Esc: Quit

## Installation

Please read the [installation section] on the project's website for
detailed instructions, broken down by operating system.

For generic from-source installation instructions, you can also consult
the INSTALL file.

[installation section]: http://dspdfviewer.danny-edel.de/#how-do-i-install-it


### On OSX

1. Clone or Download Source

2. Install required libraries (using MacPort).
```
sudo port install cmake qt5 poppler-qt5
sudo port upgrade --force boost
```

3. Prepare building using CMake
```
mkdir build
cd build
cmake ..
```

4. Build and Install
```
make
sudo make install
```


## Running 
My favorite options

```
dspdfviewer -f -p0 -s0 -b30 [PDF_FILE]
```

Or, in `~/.config/dspdfviewer.ini`
```
full-page=1
presentation-clock=0
slide-clock=0
bottom-pane-height=30
```
