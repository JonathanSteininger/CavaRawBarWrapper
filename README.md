# CavaRawBarWrapper
<h2>description</h2>
This Project is a small cli program written in C that takes the raw ascii output from <a href="https://github.com/karlstav/cava">Cava</a> and converts it into lines of <a href="https://en.wikipedia.org/wiki/Block_Elements">block&nbspelements</a>. <br>

<h2>Instillation</h2>
<h4>Requirments</h4>
<ul>
  <li>Cmake</li>
</ul>
<br>
<h4>Building</h4>
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
<h2>Usage</h2>

To use this cli program. You need to pipe in data in the format "0;0;0;0;0;\n" through stdin. The size of the numbers and the amount of values doesn't matter.<br>
aslong as there arent more than 300 values It should work.<br><br>

You can use an application like <a href="https://github.com/karlstav/cava">Cava<a/> in raw ascii output mode to get this data and pipe it into this program.<br>
An example would be like this:<br>
`cava | ./cavawrapper`

Its recommended to make a local cava config and pass that into cava.<br>
`cava -p local/path/to/config | ./cavawrapper`

<h4>notes</h4>
I recomment you use this program just for the application that needs it by copying it into its directory and having it start the program.
<hr>
<h3><a href="https://github.com/elkowar/eww">Eww</a></h3>

To use this for eww I recommend you copy the cavawrapper program into the eww config directory with eww.yuck.<br>
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

<h2>Arguments and Help</h2>

You can get a list of arguments their uses through the use of the -h and --help argument.
`./cavawrapper -h`

<h1>Output</h1>
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
