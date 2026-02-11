*This project has been created as part of the 42 curriculum by [abenabbo](https://github.com/Anis-21), [qdebraba](https://github.com/D2BT), [mdsiurds](https://github.com/Maxft68).*

*signed : The Well-Know team des Q_NU*

# ft_irc

## Description

**ft_irc** is a custom implementation of an Internet Relay Chat (IRC) server, written in C++98. The goal of this project is to understand the underlying protocols that power real-time text messaging systems and to implement a server capable of handling multiple clients, channels, and commands simultaneously using non-blocking I/O.

This server complies with the core standards of the IRC protocol (RFC 1459 and RFC 2812), acting as a daemon that listens for incoming connections and routes messages between users. It supports standard features like joining channels, private messaging, channel moderation, and file transfers via DCC (Direct Client-to-Client) protocol transparency.

## Instructions

### Prerequisites
- A C++ compiler (clang++ or g++)
- Make
- A Linux/Unix environment

### Compilation
To compile the server, run the following command at the root of the repository:
```bash
make
```

### Execution
Start the server by specifying the listening port and a connection password:
```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 mysecretpassword
```

### Connecting
You can connect to the server using:
1.  **Netcat** (for raw protocol testing):
    ```bash
    nc 127.0.0.1 6667
    ```
2.  **IRC Client** (Irssi, WeeChat, HexChat):
    ```bash
    /connect localhost 6667 mysecretpassword
    ```

## Features

- **Multi-client support**: Handles multiple simultaneous connections using `poll()`.
- **Authentication**: Connection handshake and registration (`PASS`, `USER`).
- **Channel Operations**: Users can join (`JOIN`), leave (`PART`), view or set topics (`TOPIC`), and interact within channels.
- **Operator Privileges**: Channel operators can kick users (`KICK`), invite users (`INVITE`), and change channel modes (`MODE`).
- **Private Messaging**: Direct messaging between users (`PRIVMSG`).
- **Connection Management**: Graceful user disconnection handling (`QUIT`).
- **Channel Modes**:
    - `i`: Invite-only channel
    - `t`: Topic settable by channel operator only
    - `k`: Channel requires a key (password)
    - `o`: Give/take channel operator privileges
    - `l`: Set user limit for channel
- **Bonus**: CTCP & DCC (Direct Client-to-Client) transparency for file transfers.

## Usage Examples

Here is a quick reference for the supported commands:

- **Auth**: `/PASS <password>`, `/USER <username> 0 * <realname>`, `/NICK <nickname>`
- **Join Channel**: `/JOIN <#channel> [key]`
- **Leave Channel**: `/PART <#channel> [reason]`
- **Private Message**: `/PRIVMSG <target> <message>`
- **Topic**: `/TOPIC <#channel> [new_topic]`
- **Mode**: `/MODE <#channel> <+|-flag> [args]`
  - *Example:* `/MODE #test +o user1` (Promote user1 to operator)
  - *Example:* `/MODE #test +k secret` (Set channel password to 'secret')
  - *Example:* `/MODE #test +l 10` (Limit channel to 10 users)
- **Invite**: `/INVITE <nickname> <#channel>`
- **Kick**: `/KICK <#channel> <nickname> [reason]`
- **Quit**: `/QUIT [reason]`

## Resources

### References
- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Protocol definition
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - Client Protocol
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) - A more readable documentation of the protocol
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Essential for socket programming
- [Reply & Error Codes Reference](https://www.irchelp.org/protocol/rfc/chapter6.html) - Extensive listing of server numeric responses and error messages

### AI Usage
AI tools (specifically GitHub Copilot / Gemini models) were used in this project. The AI assisted in:
1.  **Protocol Understanding**: Clarifying ambiguities in the RFCs, specifically regarding numeric replies and prefix formatting.
2.  **Debugging**: Analyzing command logic failures (e.g., `JOIN` parsing, channel mode bitmasks) and suggesting fixes for C++98 compliance.
3.  **Concept Explanation**: explaining the theoretical difference between Server-Client interactions and Client-Client (DCC) protocols.
4.  **Refactoring**: Harmonizing error message formatting across different commands.
