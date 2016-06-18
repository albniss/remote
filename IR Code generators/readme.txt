1) Para criar remotos com o irremote, o lircd deve estar desligado
	rm -f albert && sudo irrecord -d /dev/lirc0 albert
2) Para listar raw, devemos desligar o lircd
	mode2 -d /dev/lirc0
3) Para enviar comandos, o lircd deve estar LIGADO. O comando é (o segundo):
	irsend LIST albert ""
	irsend SEND_ONCE albert KEY_POWER
