# ft_irc
https://modern.ircdocs.horse/

Nicknames are non-empty strings with the following restrictions:

    They MUST NOT contain one of the following characters: space ('', 0x20), decimal (',', 0x2C), asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation point ('!', 0x21), to sign ('@', 0x40).
    They MUST NOT start with one of the following characters: dollar ('$', 0x24), Colon (':', 0x3A).
    They should NOT start with a character listed as a channel type, channel adhesion prefix, or a prefix listed in the multi-prefixExtension IRCv3.
    They should not contain point character ('.', 0x2E).

Modes de channel IRC
Les modes sont des options qui changent le comportement d’un salon (channel) IRC.
i : Invite-only. Seuls les invités peuvent rejoindre le channel.
t : Seuls les opérateurs peuvent changer le topic du channel.
k : Le channel est protégé par un mot de passe (key).

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes ./ircserv <port> <password>

listen()
Peut échouer si :
La socket n’a pas été correctement créée ou bind (pas de bind avant listen).
Le port est déjà utilisé par un autre programme.
Tu n’as pas les droits nécessaires (ex : ports < 1024 sans sudo).