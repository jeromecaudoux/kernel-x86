#set architecture i386:x86-64
break switch_to_userland
break prepare_userland
break hi
target remote localhost:1234
c
