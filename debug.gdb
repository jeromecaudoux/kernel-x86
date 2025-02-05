#set architecture i386:x86-64
break switch_to_userland
break crt0.S:93
break hi
target remote localhost:1234
c
