# Anno1800-Scripting-API

## Disclaimer

This is a pet/passion/hobby project!

No guarantees and no promises.

I never coded anything of this kind before, so expect some limitations :P

**Non-affiliated project !**

This project is in no way affiliated with Ubisoft Blue Byte
or Ubisoft


**Also this might count as cheating, so be careful** !

Finally, if you look through the code you will notice that many places are quite messy. Since I've never done this before and the game changes I cannot tell if certain techniques work or not. Much of the code could use a caring hand...

I would however love any feedback on any aspect of the code!

## Introduction

A tool to make scripts for Anno 1800
The idea of the project is to implement a scripting interface with [Anno 1800](https://www.ubisoft.com/en-us/game/anno-1800/). The game itself allows users to control most things through UI and in addition, shipping of goods via trade routes. The goal of this project is to expose as much functionality of the game through a C# scripting API.

### Requirements

  * Visual Studio 2019 (C++ and C#)

### Getting started
**How to install and use**

I will probably change a lot when I'm happier with the code. But at current, following should work.

  * Clone the repo to a local folder
  * Build the Anno1800ScriptingAPI.sln solution file (in debug mode)
  * Run Anno 1800
  * Configure the debugging settings by specifying $(TargetDir) as the working directory, as can be seen in the following image doc/surveillancevan_debug_config.png
    Launch the solution (still in debug mode)
  * Run code using the monocle project (**with administrator rights**)

### License

```
    This file is part of Anno1800-Scripting-API.

    Anno1800-Scripting-API is libre software: you can redistribute it and/or modify
    it under the terms of the MIT License as published by
    the MIT.

    Anno1800-Scripting-API is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the MIT License
    along with Anno1800-Scripting-API.  If not, see <https://opensource.org/licenses/> <https://opensource.org/licenses/MIT>.
```
