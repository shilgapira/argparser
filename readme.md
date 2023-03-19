<br>

<h3 align=center> Argparser </h3>

<p align=center>
  The argparser library makes it easy to write user-friendly command-line interfaces. <br>
  The program defines what argumants it requires, and argparser will figure out how to parse those out.
</p><br><br>


### Features

* No hidden memory allocations, all things are on stack and that makes argparser very fast.
* Automatically generated help and usage messages.
* Modern c23 implementation.
* Issue errors when users give the program invalid arguments.
* Easy to read syntax.

<br>

### Usage

* Initialization

```c
/* main.c */

#include <argparser.h>
  
int main (int argc, char **argv)
{
    struct argparser *parser = argparser.new()
    ...
}
```

* Building simple usage message

```
usage: (basename of argv[0]) [OPTION]... [ACTION]...
```

```c
  ...
  parser.usage("[OPTION]... [ACTION]...");
  ...
```

* Building help message

```
usage: (basename of argv[0]) [OPTION]... [ACTION]...

actions:
  new               my first action
  
options:
  --verbose, -v     verbose mode
  
Have a nice day!
```

```c
  ...
  parser.actions((action[]) {
    // name,  about,             n-arguments,    userdata
    // ⌄      ⌄                  ⌄               ⌄
    { "new", "my first action", 0, NULL,        NULL },
    //                             ⌃ callback
    {0} // end of list
  });
  
  parser.options((option[] {
    // longopt,   shortopt,           n-arguments,    userdata
    // ⌄          ⌄                   ⌄               ⌄
    { "verbose", 'v', "verbose mode", 0, NULL,        NULL },
    //                 ⌃ about           ⌃ callback
    {0} // end of list 
   });
   
   parser.epilog("Have a nice day!");
}

* Setting program name instead of using basename of argv[0]

```c
parser.prog("pretty-name")
```

* Creating description for help message (under usage)

```c
parser.about("out description");
```

* Setting custom callback functions for help, usage and error

```c
parser.error_fn(function)
parser.help_fn(function)
parser.usage_fn(function)
```

* Using builtin error messages
```c
  ...
  parser.exit(true)
  ...
  
  // or
  
  char *errormsg = parser.parse(argc, argv);
  
  if (errormsg)
    ...
```

* Settings colors for builtin messages
```c

  parser.colors((color) {
      .usage = "\033[32m",
      ...
  });
```
<br>

### Installation
```sh
  $ meson setup build
  # meson install -C build
``` 
<br>

### Meson

```meson
  ...
  argparser = dependency('argparser')
  ...
```

<br>

### License
Argparser is released under the MIT license, which grants the following permissions:

* Commercial use
* Distribution
* Modification
* Private use

For more convoluted language, see the [license](https://github.com/mini-rose/argparser/blob/master/license).
