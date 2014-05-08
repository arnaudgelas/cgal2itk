cgal2itk
========

Scope
-----

The goal of this project is to provide a bridge in between
[ITK](http://www.itk.org/) and [CGAL](https://www.cgal.org/) worlds.


License
-------

While [ITK has permissive license terms](http://www.itk.org/ITK/project/licenseversion1.html),
CGAL license terms are more complex (depends on CGAL's package is being used).
Since [3D Polyhedral Surface package](http://doc.cgal.org/latest/Manual/packages.html)
is released under [GPL license](http://www.gnu.org/copyleft/gpl.html), and due to the viral
aspect of GPL license, this project and any derivatives are thus covered under
GPL license terms.


Compile and test
----------------

```bash
mkdir build
ccmake ..
make -j8
ctest
```
