# Bézier Curves
A simple bezier curve editor that allows varying number of control points ( 2 - 9 ) and renders the bezier curve between the first and last control points.

- A circle follows the curve formed by the points, speed of which can be controlled.
- The positions of control points can be controlled with the imgui interface

### Running
You need to have SFML installed on your device.
On debian based Linux it can be installed with
```console
sudo apt-get install libsfml-dev
```
After installing open a terminal in the location of this code and enter
```console
make && ./a.out
```
This will make and run the program.