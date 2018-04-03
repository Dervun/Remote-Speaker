# Remote Speaker
It's a cross platform application that can send audio from device to device and play it in real time.

## Installation
For deploying the application to your Android device, read [Getting Started with Qt for Android](http://doc.qt.io/qt-5/androidgs.html) or look [QtWS16- All about Qt on Android: Say hello to Qt on Android](https://www.youtube.com/watch?v=dmKNxyi_YNk).
## Download
- Android: link to the [Play Market](https://play.google.com/store/apps/details?id=org.qtproject.remote_speaker)
- Windows: link to the [executable file in .rar](https://drive.google.com/open?id=1J8ARUl1yWT4SzjU74Je2S9hMy6u81Jjy) (run Remote_Speaker.exe)

## Needed
- C++11 compiler
- Qt 5.* (I recommend Qt5.9)
### For deploying on Android:
- Android SDK
- Android NDK (I recommend Android NDK r15c)
- Android SDK Tools (in my case only Android SDK Tools 25.2.5 correctly work with Qt 5.9 and NDK 15)
- Java SE Development Kit (JDK) (I recommend jdk v8_151)


## Usage
Both of your devices (*receiving* and *sending*) must be in the same WLAN [Wireless Local Area Network](https://en.wikipedia.org/wiki/Wireless_LAN).
For get this, you can:
1. Connect both devices to the same router
2. Create a [Wireless ad hoc network](https://en.wikipedia.org/wiki/Wireless_ad_hoc_network) and connect devices to it

### Catching of output sound:
#### Windows
If you do not have an **input device**, such as "Stereo mix" on your PC, please read [How to Enable “Stereo Mix” in Windows and Record Audio from Your PC](https://www.howtogeek.com/howto/39532/how-to-enable-stereo-mix-in-windows-7-to-record-audio/) (Windows 7 and later)
#### Ubuntu
If you do not have an **input device**, such as "Audio Analog Stereo", please read [How to record output to speakers?](https://askubuntu.com/a/229365) or [Stereo mix with Ubuntu 10](http://wiki.audacityteam.org/wiki/Stereo_mix_with_Ubuntu_10). 
#### Mac OS
With Jack Audio - [Advanced Audio Capture - Mac OS X](https://obsproject.com/forum/resources/advanced-audio-capture-mac-os-x.142/),  other ways - [well, Audacity for the Mac exists, too](https://discussions.apple.com/message/12989820#message12989820)
#### Android
Unfortunately, Android does not have such functionality ([explanation](https://android.stackexchange.com/a/96770)).

### Launching
0. Make sure that both devices in the same *WLAN*
1. Run application on your PC
2. Select **Sender**
3. Select needed **input device** (e.g. *Stereo mix* on *Windows*)
4. Run application on your second device (e.g. on *Android*)
5. Select **Receiver**
6. Select needed **output device** (e.g. *default* on *Android*)
7. Make sure that the current settings are supported by **receiver**
8. Enter ip and port of **sender** on your second device
9. Make sure that you enter correct ip and port, press **Connect**

If you did all this correctly, sound will start playing on your second device. To disable the sound on the PC (on first device, **sender**) your need to disable the sound at the system level (e.g. **Fn+F8** on my PC Acer Aspire v3-571g).
You can mute sound on your receiving device by pressing **Myte** button. Also you can control the *size of buffer* by corresponding slider (the larger the buffer, the greater the sound delay, but less loss). 

## Some screenshots
[link](https://drive.google.com/drive/folders/1BQSqVzIvuKc7TidtIADVIbvDwFS_vnwl?usp=sharing)
