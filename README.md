## **comtest**

NanoPi Fire 2A Serial port communication in C.  

<br><hr>
and you can use this link:
https://github.com/atrotech/QtE-Demo/blob/main/SerialPort.cpp

How to build
------------
```
# git clone https://github.com/atrotech/comtest.git
# cd comtest
# gcc -o comtest comtest.c
```

Usage
------------
```
./comtest -d /dev/ttyAMA3 -s 38400
```

Parameters
------------
```
# ./comtest --help
comtest - interactive program of comm port
press [ESC] 3 times to quit

Usage: comtest [-d device] [-t tty] [-s speed] [-7] [-c] [-x] [-o] [-h]
         -7 7 bit
         -x hex mode
         -o output to stdout too
         -c stdout output use color
         -h print this help
```


<br> <br>

friendlyarm NanoPi Fire2A Serial port wiki
------------

https://wiki.friendlyarm.com/wiki/index.php/Serial_port


