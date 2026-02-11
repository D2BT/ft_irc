# Guide : Implementer un Bot IRC dans un serveur IRC (C++98)

## 1. Role du Bot

Le bot est un **client interne** au serveur IRC. Il se comporte exactement comme
un vrai utilisateur qui se connecte, mais il est gere automatiquement par le
serveur. Son role dans ce projet est d'envoyer des messages humoristiques (blagues
de programmation) de maniere periodique dans tous les channels ou il est present.

Le bot :
- Se connecte automatiquement au demarrage du serveur
- Rejoint automatiquement chaque channel cree
- Devient operateur de chaque channel
- Envoie un message drole toutes les 180 secondes (configurable via une macro)

---

## 2. Architecture : le Bot herite de Client

Le bot **herite de la classe `Client`**. Cela lui permet de reutiliser toute la
logique deja en place (nickname, username, fd, buffer, etc.) et d'etre traite
comme n'importe quel autre client par le serveur.

```cpp
class Bot : public Client {
private:
    bool                        _botConnected;    // evite les connexions multiples             //UTILE ?? 
    time_t                      _lastTimeSend;    // timestamp du dernier message envoyer 		// a mettre dans chaque channel
    std::vector<std::string>    _funnyBotMsg;     // liste de messages a envoyer

public:
    Bot();
    ~Bot();

    bool    getBotConnected() const;
    time_t  getLastTimeSend() const;
    void    setLastTimeSend(time_t now);
    void    setupBot(Server & server);     // connexion du bot au serveur
    void    printBotMsg(Server & server);  // envoi d'un message aleatoire
};
```

**Attributs specifiques au bot :**
| Attribut         | Role                                                    |
|------------------|---------------------------------------------------------|
| `_botConnected`  | Flag pour ne lancer `setupBot()` qu'une seule fois      |
| `_lastTimeSend`  | Permet de calculer le delai entre chaque message        |
| `_funnyBotMsg`   | Vecteur de strings contenant les messages du bot        |

---


Le bot est initialise dans la liste d'initialisation du constructeur de `Server` :
```cpp
Server::Server(int port, const std::string& password)
    : _port(port), _listenFd(-1), _password(password),
      _serverName("my_server_irc"), _bot(Bot()) {
```

---

## 4. Etape par etape : connexion du bot (`setupBot`)

Le bot se connecte au serveur **comme un vrai client TCP**. Il ouvre un socket,
se connecte en local (`127.0.0.1`) sur le port du serveur, puis s'authentifie
avec les commandes IRC standard.

```
1. Creer un socket TCP        → socket(AF_INET, SOCK_STREAM, 0)
2. Le passer en non-bloquant  → fcntl(fd, F_SETFL, O_NONBLOCK)
3. Se connecter en loopback   → connect(fd, 127.0.0.1:port)
4. Envoyer PASS <password>    → pour s'authentifier
5. Envoyer NICK BOT           → pour choisir le pseudo "BOT"
6. Envoyer USER BOT 0 * :BOT  → pour completer l'enregistrement
```

Code correspondant :
```cpp
void Bot::setupBot(Server & server) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(_fd, F_SETFL, O_NONBLOCK);

    sockaddr_in botAddr;
    botAddr.sin_family = AF_INET;
    botAddr.sin_port = htons(server.getPort());
    botAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(_fd, (struct sockaddr*)&botAddr, sizeof(botAddr));

    _botConnected = true;

    std::string pass = "PASS " + server.getPassword() + "\r\n";
    std::string nick = "NICK BOT\r\n";
    std::string user = "USER BOT 0 * :BOT_BOT\r\n";

    send(_fd, pass.c_str(), pass.size(), 0);
    send(_fd, nick.c_str(), nick.size(), 0);
    send(_fd, user.c_str(), user.size(), 0);

    _lastTimeSend = time(NULL);
}
```

**Point important :** le bot utilise `EINPROGRESS` car le socket est
non-bloquant. La connexion peut ne pas etre instantanee, mais elle aboutira
au prochain cycle de `poll()`.

---

## 5. Quand et ou appeler `setupBot`

Dans la boucle principale du serveur (`run()`), on verifie si le bot est
deja connecte. S'il ne l'est pas, on le connecte :

```cpp
// Dans Server::run(), au debut de la boucle principale
if (_bot.getBotConnected() == false)
    _bot.setupBot(*this);
```

Le flag `_botConnected` empeche que `setupBot()` soit appele plusieurs fois.

---

## 6. Le bot rejoint automatiquement les channels

Quand un channel est **cree** (pas quand un utilisateur rejoint un channel
existant), le bot est automatiquement ajoute comme membre :

```cpp
// Dans Server::createChannel()
Channel* Server::createChannel(const std::string& name) {
    _channels.insert(std::make_pair(name, Channel(name)));
    Channel* channel = &(_channels.at(name));

    // Ajouter le bot au channel
    Client* botClient = getClientByNick("BOT");
    if (botClient) {
        channel->addClient(botClient);
    }

    return channel;
}
```

Et dans `JoinCommand::execute()`, le bot est rendu **operateur** du channel :
```cpp
channel->addOperator(server.getBot().getFd());
```

---

## 7. Envoi periodique de messages (`printBotMsg`)

Toutes les `TIME_FOR_BOT` secondes (180s par defaut, defini dans `Macro.hpp`),
le serveur appelle `printBotMsg()` :

```cpp
// Dans Server::run()
time_t now = time(NULL);
if (now - _bot.getLastTimeSend() > TIME_FOR_BOT) {
    _bot.printBotMsg(*this);
    _bot.setLastTimeSend(now);
}
```

La fonction `printBotMsg()` :
1. Recupere le vrai objet `Client*` du bot via `getClientByNick("BOT")`
2. Parcourt **tous les channels** du serveur
3. Pour chaque channel ou le bot est membre ET ou il y a plus d'1 utilisateur :
   - Choisit un message aleatoire dans `_funnyBotMsg`
   - Formate le message au format IRC (`PRIVMSG`)
   - Le broadcast a tout le channel

```cpp
void Bot::printBotMsg(Server & server) {
    srand(time(NULL));

    Client* botClient = server.getClientByNick("BOT");
    if (!botClient)
        return;

    for (/* chaque channel */) {
        if (channel.getUserConnectedNbr() > 1
            && channel.isClientInChannel(botClient->getFd())) {

            ssize_t randNbr = rand() % _funnyBotMsg.size();
            std::string msg = _funnyBotMsg[randNbr];

            // Format IRC standard
            std::string prefix = ":" + botClient->getNickname()
                + "!" + botClient->getUsername()
                + "@" + server.getServerName();
            std::string fullMessage = prefix
                + " PRIVMSG " + channel.getName()
                + " :" + msg;

            channel.broadcast(server, fullMessage);
        }
    }
}
```

**Format du message IRC :**
```
:BOT!BOT@my_server_irc PRIVMSG #general :Les 3 C de la vie : Compiler, Crash, Cafe.
```

---

## 8. Gestion du fd du bot dans la boucle principale

Le bot possede un fd (file descriptor) comme tout client. Dans la boucle
`poll()`, il faut **ignorer les donnees recues sur le fd du bot** pour ne
pas essayer de parser ses propres reponses :

```cpp
// Dans Server::run(), quand on traite les evenements poll()
if (fd != _bot.getFd()) {
    handleClientData(fd);
}
```

---

## 9. Nettoyage a la fermeture

Le fd du bot doit etre ferme proprement a la destruction du serveur :

```cpp
// Dans Server::~Server()
if (_bot.getBotConnected())
    close(_bot.getFd());
```

---

## 10. Resume : checklist pour implementer un bot

Voici les etapes a suivre pour ajouter un bot a ton propre serveur IRC :

### Creer la classe Bot
- [ ] Faire heriter `Bot` de `Client`
- [ ] Ajouter les attributs : `_botConnected`, `_lastTimeSend`, `_funnyBotMsg`
- [ ] Remplir le vecteur de messages dans le constructeur

### Integrer le bot au Server
- [ ] Ajouter un attribut `Bot _bot` dans `Server`
- [ ] Ajouter un getter `getBot()`
- [ ] Definir la macro `TIME_FOR_BOT` (ex: 180 secondes)

### Connexion du bot (setupBot)
- [ ] Creer un socket TCP
- [ ] Le mettre en non-bloquant
- [ ] Se connecter a 127.0.0.1 sur le port du serveur
- [ ] Envoyer `PASS`, `NICK`, `USER` pour s'enregistrer comme un vrai client
- [ ] Mettre `_botConnected = true`

### Dans la boucle principale (run)
- [ ] Appeler `setupBot()` si le bot n'est pas encore connecte
- [ ] Ignorer le fd du bot quand on traite les donnees clients
- [ ] Verifier le timer et appeler `printBotMsg()` quand le delai est ecoule

### Quand un channel est cree
- [ ] Ajouter le bot comme membre du channel (`addClient`)
- [ ] Rendre le bot operateur du channel (`addOperator`)

### Envoi de messages (printBotMsg)
- [ ] Recuperer le client bot par son nickname
- [ ] Parcourir tous les channels
- [ ] Verifier que le bot est dans le channel et qu'il y a d'autres membres
- [ ] Choisir un message aleatoire
- [ ] Formater en `PRIVMSG` IRC valide
- [ ] Broadcast le message au channel

### Nettoyage
- [ ] Fermer le fd du bot dans le destructeur du serveur

---

## 11. Diagramme de fonctionnement

```
                    Demarrage du serveur
                           |
                    setupBot() appele
                           |
              Bot ouvre un socket TCP local
              Bot envoie PASS / NICK / USER
                           |
              Le serveur traite les commandes
              comme un client normal
                           |
                    Bot est enregistre
                           |
        ┌──────────────────┴──────────────────┐
        |                                     |
  Un user cree un channel            Timer > 180s
        |                                     |
  Le bot est ajoute                  printBotMsg()
  au channel + operateur                      |
        |                            Pour chaque channel :
        |                            - bot present ?
        |                            - > 1 membre ?
        |                            - oui → PRIVMSG random
        |                                     |
        └──────────────────┬──────────────────┘
                           |
                   Boucle poll() continue
```

---

## 12. Points d'attention

1. **Le bot est un vrai client TCP** : il passe par le meme processus
   d'authentification que n'importe quel utilisateur. Il n'a pas de
   traitement special dans les commandes IRC.

2. **Non-bloquant** : le socket du bot est non-bloquant pour ne pas
   bloquer la boucle `poll()` du serveur.

3. **Pas de commandes recues** : le bot ne traite pas les messages qu'il
   recoit. On ignore simplement les donnees sur son fd.

4. **srand()** est appele a chaque envoi de message. C'est acceptable ici
   car la frequence est faible (toutes les 3 minutes).

5. **Double reference au bot** : l'objet `_bot` dans Server est l'entite
   "logique" du bot, mais le vrai `Client*` dans la map `_clients` est
   celui qui est enregistre via les commandes NICK/USER. C'est pourquoi
   `printBotMsg()` utilise `getClientByNick("BOT")` pour recuperer le
   bon pointeur.
