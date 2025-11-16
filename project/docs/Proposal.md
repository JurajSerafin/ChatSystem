# Chat System

## Overview

A chatting application with user authentication.

If the user is accessing the application for the first time, they are asked to register by specifying a **login** and a **password**, which will serve as their credentials for signing in.

After a successful registration, their account is created and the user is assigned a unique **tag**, which will further serve as their unique identifier.

Within the application, users can send messages to each other on the network. Both account data and messages are encrypted asymmetrically.

### Sending a Message

The application's layout is typical: it displays a list of the user's open chats. Selecting one of them opens a dialog window with a text input bar, where messages can be composed and sent.

### Finding a User

The application includes a search bar used to find other users. After entering a user's **login** or **tag**, a list of matching users is displayed.

### Finding a Sent Message

Within each chat log, users can search for previously sent messages by entering keywords into a dedicated search bar. The search results are shown in a standard vertical list format.

## Technology

- **wxWidgets** – GUI
- **Boost.Asio** – networking, asynchronous I/O
- **OpenSSL** – cryptography
- **libpqxx** – storing server-side data in **PostgreSQL**
- **sqlite3** – caching client-side data in **SQLite**
- **nlohmann/json** – serialization
- **spdlog** – logging

### API

The application uses a **WebSocket API** for real-time communication (read receipts, push notifications) and a **REST API** for all other functionality.

### Extensibility

The application is expected to be easily extensible in every practical aspect—such as adding new message types, supporting different communication protocols, or defining new user roles— all through well-specified interfaces.
