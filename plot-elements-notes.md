# Notes

sg_element is stored in an array (pod_bvector) inside a plot in plot::item_list because it is a POD object.

## What methods of sg_element plot uses ?

```cpp
- draw(virtual_canvas, agg::trans_affine)
- clone_object
- bounding_box (through the object field)
- destructor
```

Important: the plot class do **not** access the sg_element's fields except object itself. This is good to encapsulate a struct that behaves like sg_element.

