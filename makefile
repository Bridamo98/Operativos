all: test_manager.c test_talker.c
	gcc -o manager test_manager.c -I.
	gcc -o talker test_talker.c -I.
mmake: test_manager.c
	gcc -o manager test_manager.c -I.
tmake: test_talker.c
	gcc -o talker test_talker.c -I.
