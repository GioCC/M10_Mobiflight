File 'config_board.h' : File di configurazione per le diverse schede M10

Il file deve essere nella directory principale dei sorgenti

Prima di includere il .h, va definita la costante 'BOARDTYPE' con uno dei valori sotto elencati.
NB: Per permettere l'utilizzo della configurazione anche alle unità di compilazione che non vedono 
la definizione della costante nel sorgente principale, 'BOARDTYPE' è definita direttamente in 'config_board.h'.

Fa riferimento ai file di cfg singoli da trovare nel path '.\boardcfg\'.

Valori per 'BOARDTYPE':

M10_01_Radio
M10_02_ADF_DME
M10_03_XPDR_OBS_CLK
M10_04_AP
M10_05_Radio_LCD
M10_06_Multi_LCD
M10_07_AP_LCD
M10_08_Audio
M10_09_EFIS
