# Awale Game - README

## Description des fonctionnalités

Ce projet implémente une version du jeu d'Awale, permettant à plusieurs joueurs de jouer au jeu traditionnel africain. L'application utilise une architecture client-serveur, et chaque joueur peut accéder à diverses fonctionnalités via des commandes textuelles.

Lorsqu'un client se connecte avec son pseudo, le menu suivant est envoyé, permettant d'accéder à une gamme complète de fonctionnalités :

### Commandes disponibles

- **[LOP] List Online Players**  
  Affiche la liste des joueurs actuellement connectés au serveur.

- **[APF] [player name] Add a Player to Friends List**  
  Ajoute un joueur spécifique à la liste d'amis de l'utilisateur.

- **[CAP] [player name] Challenge a Player**  
  Permet de défier un joueur connecté à une partie d'Awale.

- **[LSG] List Ongoing Games**  
  Affiche la liste de toutes les parties en cours sur le serveur.

- **[LYG] List Your Ongoing Games**  
  Affiche uniquement les parties en cours où l'utilisateur participe.

- **[LFG] List Finished Games**  
  Montre toutes les parties terminées sur le serveur.

- **[YFG] List Your Finished Games**  
  Liste uniquement les parties terminées où l'utilisateur a joué.

- **[WAG] [game id] Watch a Game**  
  Permet de regarder une partie en cours via son identifiant. L'utilisateur pourra être refusé de regarder la partie si un joueur est en mode privé et qu'il n'est pas ami avec un des joueurs.

- **[SND] [message] Chat with Online Players**  
  Envoie un message public visible par tous les joueurs connectés.

- **[SPM] [player name] [message] Send a Private Message**  
  Envoie un message privé à un joueur en ligne spécifique.

- **[DYP] Display Your Profile**  
  Affiche le pseudo et la bio de l'utilisateur.

- **[DPP] [player name] Display a Player's Profile**  
  Permet de consulter le profil d'un autre joueur.

- **[BIO] [new bio] Modify Your Bio**  
  Met à jour la bio de l'utilisateur.

- **[PVM] [on/off] Turn Private Mode On/Off**  
  Active ou désactive le mode privé. En mode privé, seulement les amis des joueurs peuvent regarder la partie.

- **[WFG] Watch a Game Already Played**  
  Permet de revoir une partie terminée.

- **[LFR] List Friend Requests**  
  Affiche les demandes d'amis reçues par l'utilisateur.

- **[LSF] List Friends**  
  Montre la liste complète des amis de l'utilisateur.

## Compilation et Exécution

Depuis la racine du projet, générez les fichiers de compilation en exécutant la commande suivante :

```sh
mkdir build
cd build
cmake ..
```

Pour compiler le serveur et l'executer, lancer les commandes suivantes:

```sh
make server
./server
```

Pour compiler le client et l'executer, lancer les commandes suivantes:

```sh
make client
./client 127.0.0.1 <pseudo du client>
```

### Prérequis

- Assurez-vous d'avoir CMake installé sur votre système. Vous pouvez vérifier cela en exécutant la commande suivante :

  ```sh
  cmake --version
  ```

  Si CMake n'est pas installé, vous pouvez l'installer en utilisant votre gestionnaire de paquets. Par exemple, sur Ubuntu :

  ```sh
  sudo apt-get install cmake
  ```

  Sur macOS, vous pouvez utiliser Homebrew :

  ```sh
  brew install cmake
  ```

  Sur Windows, vous pouvez télécharger l'installateur depuis le [site officiel de CMake](https://cmake.org/download/).
