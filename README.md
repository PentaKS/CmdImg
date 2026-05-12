# CmdImg
A simple command line tool to create ascii art of any given image in PPM3 format.
# PPM3 
PPM3 is a human readable image format consisting of header and pixel data as 'RGB' values 
like:
'P3
[width]
[height]
RGB
255 0 143
...
'
# CmdImg: features
- The tool uses PPM3 data to modify and apply different effects to approach desired image properties, then turn that data to 'ASCII' style art.
- Images are downscalable to desired console size (not upscalable).
- Works on most of console sizes but as 'font-size' increases quality degrade. Lesser the 'font-size' better the image translation to 'ASCII'.
- Various modes of 'ASCII' styles.
# Commands and Usage
| Commands | Usage |
|----------|-------|
| '-i' or '-input' | input image  e.g. -i test_image_1.ppm|
| '-o' or '-output' | output image e.g. -o output.ppm |
| '-m' or '-mode' | ascii mode / style, available modes: 
<ul>
<li> [-m 0] ascii_gradient1           
<li>[-m 1] ascii_gradient2</li>
<li>[-m 2] box_gradient1</li>
<li>[-m 3] box_gradient2</li>
<li>[-m 4] visual_density</li>
<li>[-m 5] subpixel   </li>       
<li>[-m 6] subpixel_black_shadows </li>
<li>[-m 7] subpixel_green_black</li>    
<li> e.g. -m 3</li> </ul>               |


