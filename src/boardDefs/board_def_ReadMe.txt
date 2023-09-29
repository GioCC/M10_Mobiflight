File 'config_board.h' : File di configurazione per le diverse schede M10

Il file deve essere nella directory principale dei sorgenti

Prima di includere il .h, va definita la costante 'BOARDTYPE' con uno dei valori sotto elencati.
NB: Per permettere l'utilizzo della configurazione anche alle unità di compilazione che non vedono 
la definizione della costante nel sorgente principale, 'BOARDTYPE' è definita direttamente in 'config_board.h'.

Fa riferimento ai file di cfg singoli da trovare nel path '.\boardcfg\'.

Valori per 'BOARDTYPE':

T01_Radio
T02_ADF_DME
T03_XPDR_OBS_CLK
T04_AP
T05_Radio_LCD
T06_Multi_LCD
T07_AP_LCD
T08_Audio
T09_EFIS
