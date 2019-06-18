# JSON Configurations

- The keys are case sensitive and values are case insensitive!

## Working Mode

key: **mode**, value: **server** or **client**

```JSON
    "mode":"server",
```

or

```JSON
    "mode":"client",
```

## Server Configurations

- key: **server_addr**, value: IP address or URL, like: `127.0.0.1`, `example.com`, ...
- key: **server_port**, value: the server port number, like: `8388`

```JSON
    "server":"127.0.0.1",
    "server_port":8388,
```

## Proxy Configurations

- key: **proxy_addr**, value: IP address or URL, like: `127.0.0.1`, `example.com`, ...
- key: **proxy_port**, value: the proxy port number, like: `8388`

```JSON
    "local":"127.0.0.1",
    "local_port":1080,
```

## Encrypt/Decrypt

key: **algorithm**, value is one of the following ones:
- `aes-256-cfb`

key: **password**, any thing for encrypt/decrypt if not empty

```JSON
    "method":"aes-256-cfb",
    "password":"password",
```

## Miscellaneous

key: **timeout**, value: number in seconds, default is ``

```JSON
    "timeout":600,
```

key: **http_proxy**, value: **true** or **false**

```JSON
    "http_proxy":true,
```

key: **log_level**, value is one of the following ones: **debug**,
**info**, **warn**, **error**, **fatal**

```JSON
    "log_level": "info",
```
