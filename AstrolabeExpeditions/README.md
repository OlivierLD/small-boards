# Astrolabe Expéditions

Some links:
- Git repos
  - [LittObs](https://github.com/astrolabe-expeditions/LittObs_OSOCTD)
  - [SenseOcean](https://github.com/astrolabe-expeditions/SensOcean_OSOTSG/tree/v5-fireebeetle)
- [Programmer SenseOcean](https://cedriccoursonopensourceoceanography.notion.site/Programmer-SensOcean-b69c2cf5203f4e7b8d01bfecacec98ce#7b0cb02975d64c3b85b823d8f587f83b)

---
From Cédric:

Actuellement, on a des données physiques océanographiques fournies par deux types d'instruments :
- Les TSG (thermosalinographe) qui mesure température et salinité en surface (associer à horloge et GPS, ainsi que pression et température atmosphérique). Github ici : https://github.com/astrolabe-expeditions/SensOcean_OSOTSG
- Les CTD (Conductivity Temperature Deph), qui mesure temperature pression et salinité sous l'eau. github ici: https://github.com/astrolabe-expeditions/LittObs_OSOCTD

Ces deux instruments partagent les memes composant et la meme technologie.

Pour récupérer les données on a commencé à créer un interface http qui est accessible via l'ESP32 de l'instrument en mode serveur.
On peut se connecter à cette interface avec un téléphone, tablette, pc ... comme on se connecte à n'importe quel point wifi.

Pour le moment ce code à uniquement été testé dans nos CTD (notion : https://cedriccoursonopensourceoceanography.notion.site/Module-CTD-proto-V3-2603377dba93452893825b9292c5238f?source=copy_link ) et fonctionne bien pour récupérer les données.
Des étudiants de l'ICAM ont travaillé dessus cet été pour pouvoir configurer l'instrument (via la modification d'un fichier config.txt présent sur la carte SD de l'instrument), leur code est ici : https://drive.google.com/drive/folders/1U3J64AeshezPJEH22gu6gXVyI7REljTA?usp=sharing

Il faudrait donc faire le point et uniformiser le travail fait avant avec le travail fait par l'ICAM (normalement deja dans le code ICAM).
Le formaliser proprement pour le mettre sur tout nos instruments.

Et ensuite on pourrait ajouter une brique NMEA, pour que nos instruments envoie les données en NMEA à l'ordinateur de bord du bateau.

Et on peut aussi imaginer un plugin pour OpenCPN qui récupérer et affiche ces données puis les envoie une fois au port sur notre futur plateforme de donnée.

---

Pour les ESP32 on utilise des ESP32 Firebettle. (celui la : https://www.gotronic.fr/art-module-firebeetle-esp32-dfr0478-26810.htm)

Il embarque wifi/bluetooth, et un procésseur assez puissant pour faire tourner un serveur. Mais surtout il ne consomme que 10uA en veille.... indispensable pour nos instrument qui reste immergé plusieurs mois.

---

pour aider à la prise en main : https://cedriccoursonopensourceoceanography.notion.site/Programmer-SensOcean-b69c2cf5203f4e7b8d01bfecacec98ce?source=copy_link

---