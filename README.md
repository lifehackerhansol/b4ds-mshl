# extlink2argv

Launch extensions from Moonshell 2 and Wood R4/RPG, if app supports argv.

## How to use:
1. Copy `extlink2argv.nds` to `/moonshl2/extlink` and rename it to `<rom-extension>.extlink2argv.nds`
1. Create a text file named `<rom-extension>.extlink2argv.txt` in that exact location
1. In the text file, write the file path of the modern homebrew you are trying to launch in UNIX format, i.e.:
    - `/_nds/GBARunner2_arm7dldi_ds.nds`
1. Go wild


### License (nds_loader_arm9):
```
    Copyright (C) 2005 - 2010
	    Michael "Chishm" Chisholm
	    Dave "WinterMute" Murphy
    
    extlink2argv
    Copyright (C) 2021 lifehackerhansol

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
```

### License (libnds / libands):
Modifications by lifehackerhansol:
- extend DLDI size to 32KB. MSHL2 will corrupt the binary in RAM otherwise
```
  Copyright (C) 2005 - 2008
   Michael Noland (joat)
   Jason Rogers (dovoto)
   Dave Murpy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.
```

### License for everything else:
```
Copyright (c) 2021-present lifehackerhansol

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
