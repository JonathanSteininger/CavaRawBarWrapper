! Announcment !
===============
This config is getting re-written and refactored for **EWW v0.6.0**<br>
All scripts will be re-written in python for better performance and better maintainability. <br>
This work is being done on the eww0.6 branch. <br>
<br>
Consider the main branch unmaintained.
<br>
<br>



CavaRawBarWrapper
=================

- [Description](#description)
- [Compilation](#compilation)
  - [Requirements](#requirments)
  - [Building](#building)
- [Usage](#usage)
  - [Arguments and Help](#arguments-and-help)
      -  [Help Screen](#help-screen)
  - [EWW](#eww)
- [What it outputs](#output)









Description
-----------

This Project is a small cli program written in C that takes the raw ascii output from <a href="https://github.com/karlstav/cava">Cava</a> 
and converts it into lines of <a href="https://en.wikipedia.org/wiki/Block_Elements">block elements</a>. <br>
<br>









Compilation
-----------

#### Requirments
<ul>
  <li>Cmake</li>
</ul>

#### Building
To build this program you will need to clone the repository

```
git clone https://github.com/JonathanSteininger/CavaRawBarWrapper.git
```

Go into the directory

```
cd CavaRawBarWrapper
```

Add a build directory
```
mkdir build
```
make build files and build the program
```
cmake -S . -B build && cmake --build build
```
The final program is "build/cavawrapper"

You can do this all at once
```
git clone https://github.com/JonathanSteininger/CavaRawBarWrapper.git
cd CavaRawBarWrapper
mkdir build
cmake -S . -B build && cmake --build build
```











Usage
-----

### General

To use this cli program. You need to pipe in data in the format "0;0;0;0;0;\n" through stdin. The size of the numbers and the amount of values doesn't matter.<br>
aslong as there arent more than 300 values It should work.<br><br>

You can use an application like <a href="https://github.com/karlstav/cava">Cava<a/> in raw ascii output mode to get this data and pipe it into this program.<br>
An example would be like this:<br>
`cava | ./cavawrapper`

Its recommended to make a local cava config and pass that into cava.<br>
`cava -p local/path/to/config | ./cavawrapper` <br>
<br>

#### Arguments and Help

You can get a list of arguments their uses through the use of the -h and --help argument.
`./cavawrapper -h`

##### Help Screen
```
cavawrapper Help Screen

This program reads the raw output from cava in ascii mode.
The cava output will need to be piped into this program.

Optional Arguments:

	--bar-added-height [num]  -  (default: 0)  -  adds a set amount of height to every bar.
		Usefull if you want to permenantly see the bars even when theres little to no sound.

	--minimum-output-height [amount of layers]  -  (default: 10)  -  forces the program to output a minimum amount of layers.
		This will not change the bars height, but will make the program draw alot more empty space.
		This is usefull if another program struggels with the length of the output changing alot.


	--input-bar-delimiter [ascii code]  -  (default: 59)  -  Changes the expected charecter between inputed bar values.

	--input-frame-delimiter [ascii code]  -  (default: 10)  - Changes the expected charecter after all bar values are printed.

	--output-layer-delimiter [ascii code]  -  (default: 59)  - Changes the output charecter that goes in between each layer

	--output-frame-delimiter [ascii code]  -  (default: 10)  - Changes the output charecter that goes after evey layer has been printed

	--position-output [position]  -  (default: bottom)  -  Chooses the direction of the output. options: bottom, left, top, right
```


#### notes

I recommend you use this program just for the application that needs it by copying it into its directory and having it start the program.
<hr>

### Eww

To use this for <a href="https://github.com/elkowar/eww">Eww</a> I recommend you copy the cavawrapper program into the eww config directory with eww.yuck.<br>
Making a local Cava config file is recommended.<br>
<br>
To use it in eww:

```
(deflisten soundBars
  `cava -p ./localCavaConfig | ./cavawrapper`)
```
<br>
To Display the bars:

```
(defwidget []
  (box :valign "end"
        (box :valign "end" "${replace(soundBars, ";", "
")}")
       )
)
```
This will replace the raw data's seperator charecter with a new line.<br>
you can't insert a \n char in eww so you have to make the string use a real new line char instead (thats why it looks ugly and wraps like that).

I recomend to use a monospaced font like "Hack" so the bars connect on each line. 

#### Eww struggling with small bar sizes
For some reason eww stutters when the outputed string changes size really fast.<br>
You can use the argument --minimum-output-height to set a minimum amount of layers.<br>
The larger this value is the smoother eww will display it. However, eww will use more power when nothing is playing.
<br>
<br>





Output
------

This program will take an input until a end of frame charecter has been found (default = '\n').<br>
When the end of the frame has been found it will process it into the final output.<br>
<br>
The output will be layers of text split by a delimiter charecter (default = ';') and print it to std out.<br>
<br>
Lets stay the input is '0;1;2;3;4;\n'<br>
The output would be: ' ▁▂▃▄\n'<br>
<br>
if the bars are high enough the output will be split into layers with a delimiter.<br>
input: '0;1;12;8\n;'<br>
output: '  ▄ ; ▁██\n'<br>
<br>
When You take this output and replace the delimiter ';' with '\n' you get:<br>
  ▄ <br>
 ▁██<br>
 <br>
Using a monospaced font like Hack would have the bars semlessly stack on top of each other to give the elusion of a solid bar.
