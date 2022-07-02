# Hinfosvc - Server v jazyce C

Hodnocení: 19/20

Program poskytující informace o počítači, na kterém je spuštěný.

## Použití
 -  Server side
 	- ./hinfosvc PORT

-  Client side:
	- GET http://HOSTNAME:PORT/ARGUMENT
	- Curl http://HOSTNAME:PORT/ARGUMENT > outputfile
	- Zadání adresy http://HOSTNAME:PORT/ARGUMENT do libovolného prohlížeče

-  Argumenty
	- <b>hostname</b> - vrací síťové jméno počítače
	- <b>cpu-name</b> - vrací název CPU
	- <b>load</b> - Vrací aktuální zátěž

<b>Příklad 1 Localhost</b>

	Server:
	root@root:/dir$ ./hinfosvc 8080

	Client:
	user@root:/dir2$ GET http://localhost:8080/hostname
	hostname

<b>Příklad 2 - Merlin</b>

	Server:
	xjmeno00@merlin: ~/test$ ./hinfosvc 8080

	Client:
	root@root: ~/test$ GET http://merlin.fit.vutbr.cz:8080/hostname
	merlin.fit.vutbr.cz
