# BackpropTools: Training and Inference on a Teensy 4.1
Note training is done on a Teensy with the PSRAM extension to fit the full replay buffer (as used in the other training benchmarks on the other platforms like PC, Mac, etc.). It is possible to successfully train the Pendulum with a smaller replay buffer (e.g. 2000) steps on the pure Teensy without the PSRAM extension by changing the memory location of the buffers in the code. Tested with Arduino `2.0.4` and the teensy extension `1.58.0` by Paul Stoffregen. 

- Set the Sketchbook location under the Arduino preferences to the `Arduino` directory in this repository. 
- Add `https://www.pjrc.com/teensy/package_teensy_index.json` to the additional board manager URLs.
- Install the teensy extension through the board manager
- By default Arduino is using C++14 but BackpropTools is built around C++17. The support can be enabled by modifying the `~/.arduino15/packages/teensy/hardware/avr/1.58.0/platform.txt`:
```diff
- recipe.cpp.o.pattern="{compiler.path}{build.toolchain}{build.command.g++}" -c {build.flags.optimize} {build.flags.common} {build.flags.dep} {build.flags.cpp} {build.flags.cpu} {build.flags.defs} -DARDUINO={runtime.ide.version} -DARDUINO_{build.board} -DF_CPU={build.fcpu} -D{build.usbtype} -DLAYOUT_{build.keylayout} "-I{build.path}/pch" {includes} "{source_file}" -o "{object_file}"
+ recipe.cpp.o.pattern="{compiler.path}{build.toolchain}{build.command.g++}" -c {build.flags.optimize} {build.flags.common} {build.flags.dep} {build.flags.cpp} {build.flags.cpu} {build.flags.defs} -std=gnu++17 -DARDUINO={runtime.ide.version} -DARDUINO_{build.board} -DF_CPU={build.fcpu} -D{build.usbtype} -DLAYOUT_{build.keylayout} "-I{build.path}/pch" {includes} "{source_file}" -o "{object_file}"
```
- Make sure to restart after changing the `platform.txt`
