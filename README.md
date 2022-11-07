# VeebreNet
Brand new network and website system for terminal. So yes, you are looking to a stupid re-invented internet. Damn.

### About system:

If a browser(any VeebreNet client) wants to connect to an VeebreNet server, client does these in order:

- Connects to server with *TCP/IP socket*.
- Sends *optV* value to server.
  - **optV**: {version[0],version[1],dir[0],dir[1],dir[2]}
  - First 2 bytes defines client's VeebreNet version. If server not supports that version, we will see what happens next. 
  - Last 3 bytes defines the directory id that client wants from server. In example 0,0,0 for index page. But it is not forced to index for 0,0,0. Server can reply with anything.
- Server sends a preWeb value for response.
  - **preWeb**: {errorId,size}
  - First byte will be 0 if server sended a respond that represent a successful connection. Or server will send an errorId. errorIds listed below:
    - 0: successful connection.
    - 1: wrong or unsupported version.
    - 2: directory not found.
  - Second byte will represent size as 2 power of *size*. So if client gets 5 as size, client will read 32 (2 power of 5) bytes.
- Client reads *preWeb* and if ```preWeb[0]``` is 0, clients sends *optSize*. That will represent option count that will be sended to server. Otherwise the connection cuts off and thrown into the ocean.
  - **optSize** : represents options's size for next operations.
- And then, client sends *optType* as option's types as 1 byte for 1 type to server (like ``{'c','i','i','i'}``). Represented chars for variable types is listed below:
  - **optType** : {type1,type2,.....,typen}
  - 'c' = Character Type (1 byte)
  - 'i' = Integer Type (4 byte)
- And then, client sends *opts* as options. 4 byte for integers, 1 byte for characters. For integers, variable that sending to server will be seperated to 4 bytes with basic calculation. And then server will assamble them again. And with *optType*, server will know wich variable is integer and wich one is character (or chicken).
  - **opts** : {int1B1,int1B2,int1B3,int1B4,char1,int2B1, ......}
  - An integer value will be {intB1,intB2,intB3,intB4}
- And finally, server sends page and client reads it with reading ``2 power of preWeb[1]`` bytes.

If a visual description is required:

```
Client                     Server
------                     ------
optV          -----------> reads optV
reads preWeb  <----------- preWeb
if preWeb[0] not equals to 0 connection stops here.
optSize       -----------> reads optSize
optType       -----------> reads optSize byte for reading optType
opts          -----------> reads opts with looking to readed optType
reads page    <----------- sends page
```
