ArdSim M10

Progetto Arduino per serie pannelli FSP-M10

Inizialmente basato su libreria ArdSim (v1.5), da cui il nome, non è più dipendente da quella.

Sostituisce il precedente progetto FSP_M10Panels.

Rispetto al precedente, oltre a gestire il prelevamento dei dati verso un buffer interno, introduce classi relative ad elementi istanziabili (pulsanti, LED, encoder...), dotati di una funzione di callback per l'azione corrispondente e associabili ai dati del buffer.

Esiste una versione MBED per usare una scheda ARM ST-Nucleo come test rig (associata a un Arduino UNO come proxy).
