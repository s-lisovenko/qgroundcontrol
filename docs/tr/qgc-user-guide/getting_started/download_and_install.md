# İndirme ve Kurulum

The sections below can be used to download the [current stable release](../releases/release_notes.md) of _QGroundControl_ for each platform.

:::tip
See [Troubleshooting QGC Setup](../troubleshooting/qgc_setup.md) if _QGroundControl_ doesn't start and run properly after installation!
:::

## Sistem Gereksinimleri

QGC tüm güncel bilgisayar ya da mobil cihazlarda iyi bir şekilde çalışacaktır. taraf uygulamalara ve mevcut sistem kaynaklarına bağlıdır.
Daha yüksek kapasiteli donanım daha iyi bir deneyim sunacaktır.
En az 8 Gb Ram'e, SSD'ye, Nvidia ya da AMD ekran kartına ve i5 veya daha iyi bir işlemciye sahip bir bilgisayar çoğu uygulama için yeterince iyi olacaktır.

En iyi deneyim ve uyumluluk için size işletim sisteminizin en yeni sürümünü kullanmanızı öneriyoruz.

## Windows {#windows}

_QGroundControl_ can be installed on 64 bit versions of Windows 10 (1809 or later) or Windows 11:

1. Download [QGroundControl-installer.exe](https://d176tv9ibo4jno.cloudfront.net/latest/QGroundControl-installer.exe).
2. exe'ye çift tıklayın.

:::info
Windows kurulum programı 3 kısayol oluşturur: **QGroundControl**, **GPU Compatibility Mode**, **GPU Safe Mode**.
Eğer başlatma veya video işleme sorunları yaşamıyorsanız ilk kısayolu kullanın.
For more information see [Troubleshooting QGC Setup > Windows: UI Rendering/Video Driver Issues](../troubleshooting/qgc_setup.md#opengl_troubleshooting).
:::

## Mac OS X {#macOS}

_QGroundControl_ can be installed on macOS 12 (Monterey) or later:

<!-- match version using https://docs.qgroundcontrol.com/master/en/qgc-dev-guide/getting_started/#native-builds -->

<!-- usually based on Qt macOS dependency -->

1. Download [QGroundControl.dmg](https://d176tv9ibo4jno.cloudfront.net/latest/QGroundControl.dmg).
2. .dmg dosyasına çift tıklayın, ardından çıkan ekranda _QGroundControl_'ü _Application_ dosyasına sürükleyin.

::: info
QGroundControl continues to not be signed. You will not to allow permission for it to install based on you macOS version.
::

## Ubuntu Linux {#ubuntu}

_QGroundControl_ can be installed/run on Ubuntu LTS 22.04 (and later):

Ubuntu, bir seri bağlantı noktasının (veya USB serisinin) robotikle ilgili kullanımına müdahale eden bir seri modem yöneticisi ile birlikte gelir.
_ QGroundControl _ 'ü kurmadan önce modem yöneticisini kaldırmalı ve seri bağlantı noktasına erişim için kendinize izin vermelisiniz.
Ayrıca video akışını desteklemek için _ GStreamer _ 'ı da yüklemeniz gerekmektedir.

QGroundControl \* 'ı ilk kez kurmadan önce:

1. On the command prompt enter:
   ```sh
   sudo usermod -a -G dialout $USER
   sudo apt-get remove modemmanager -y
   sudo apt install gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl -y
   sudo apt install libfuse2 -y
   sudo apt install libxcb-xinerama0 libxkbcommon-x11-0 libxcb-cursor-dev -y
   ```
   <!-- Note, remove install of libqt5gui5 https://github.com/mavlink/qgroundcontrol/issues/10176 fixed -->
2. Logout and login again to enable the change to user permissions.

&nbsp; _ QGroundControl _ yüklemek için:

1. Download [QGroundControl-x86_64.AppImage](https://d176tv9ibo4jno.cloudfront.net/latest/QGroundControl-x86_64.AppImage).
2. Install (and run) using the terminal commands:
   ```sh
   chmod +x ./QGroundControl-x86_64.AppImage
   ./QGroundControl-x86_64.AppImage  (or double click)
   ```

## Android {#android}

_QGroundControl_ can be installed/run on Android 9 or later:

- [Android 32/64 bit APK](https://qgroundcontrol.s3-us-west-2.amazonaws.com/latest/QGroundControl.apk)

## Old Stable Releases

Old stable releases can be found on <a href="https://github.com/mavlink/qgroundcontrol/releases/" target="_blank">GitHub</a>.

## Daily Builds

Daily builds can be [downloaded from here](../releases/daily_builds.md).
