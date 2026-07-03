# RP2040-based Custom Keyboard

**For a more in-depth look of the design process, see [here](https://docs.google.com/presentation/d/1JhzegaFFm75VKtiX3EKCw5-WWyUxq2r7/edit?usp=sharing&ouid=113504317876679119192&rtpof=true&sd=true).**

<img width="1124" height="453" alt="image" src="https://github.com/user-attachments/assets/27e902c2-ed73-4fd9-ab9d-9d78b41300d5" />

## PCB Design

I designed the PCB in KiCad, starting with the [base RP2040 wiring diagram](https://pip-assets.raspberrypi.com/categories/814-rp2040/documents/RP-008279-DS-1-hardware-design-with-rp2040.pdf#%5B%7B%22num%22%3A9%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C115%2C841.89%2Cnull%5D) included in its documentation. For keyboard-specific footprints, I used the [marbastlib library](https://github.com/ebastler/marbastlib). For the layout, I didn't want something too big (like a 100% layout) because it leaves less room for the mouse, but I also didn't want it to be too small (like a 60%), since I enjoyed keeping dedicated function keys, so I finally decided on a TKL (80% layout) board. While designing this keyboard, I decided I wanted to add a few knobs (for volume adjustment, brightness adjustment, etc.) so I included two rotary encoders in my design. 

<img width="1081" height="897" alt="image" src="https://github.com/user-attachments/assets/d8aafd0c-c055-4f29-9dca-b345106625a9" />
<img width="1614" height="607" alt="image" src="https://github.com/user-attachments/assets/33859a59-8ad9-45d5-b0f7-4df9c485e9e3" />
(<img width="900" height="703" alt="image" src="https://github.com/user-attachments/assets/52b8658d-9d50-429d-bdde-e87906b5daf0" />

## Case Design

To design the case, I used AutoDesk Fusion. I decided to keep the case design minimal/basic, since I didn't really care about the exterior at all. The reason why the main body and plate is cut in two is because my friend's 3D printer was too small to print the entire thing in one piece.

<img width="962" height="620" alt="image" src="https://github.com/user-attachments/assets/dd406b09-a317-47dc-bc22-e03ba3d1c605" />
<img width="1344" height="874" alt="image" src="https://github.com/user-attachments/assets/03144a83-bcda-419f-89c1-751f078d8b3f" />

## Code

I could have made my life significantly easier by just uploading QMK firmware to my keyboard, as it supports the RP2040, but I didn't want to take a shortcut by using premade software for a fully custom keyboard.

To start, the official RP2040 SDK GitHub includes several example code snippets, which were immensely helpful in understanding USB HID communcation. I decided build off of the [dev_hid_composite example snippet](https://github.com/raspberrypi/pico-examples/blob/master/usb/device/dev_hid_composite/main.c) for my keyboard's firmware.

## Conclusion

Overall, this project was a wonderful learning experience for me, as it helped me learn basic schematic/PCB design, and introduced me to C programming. Though, this is not the end of my keyboard designing journey, as I have many other ideas for the next version of my RP2040 keyboard.. (RGB, modularity, OLED screens, fun stuff.)
