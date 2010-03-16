#!/bin/bash

#Glue skripta za generisanje simbola.
#Generalno, pajplajn je sledeci:
#	1. Dampujemo shemu
#	2. Vadimo componentu title-B.sym (gschem je obavezno dodaje)
#	3. Novodobijenu shemu parsujemo u json
#	4. Json parsujemo symgen skriptom u tmp fajl.
#	%. Iz tmp fajla pomotju tragesym generisemo simbol


