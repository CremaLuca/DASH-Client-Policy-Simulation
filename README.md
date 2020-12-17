# DASH Client policy simulation

## Homework

The homework consists in simulating the playout of a media using a DASH-like protocol: the server provides the client a Media Presentation Descriptor that contains a collection of segments and for each one of them has 5 encoding levels that differ for bit size and quality, such information is contained in the `MPD.txt` file.
The client has to decide a policy to select the encoding level for each segment that maximizes a quality function.
The quality function keeps track of:

- quality of the segment
- difference of quality between segments
- delays.

The average speed connection for each segment download is given by one of the `channel.txt` file's rows.
Our goal is to design the most performing policy for a fixed quality funcion.

## Usage

In order to try the policy you gotta compile it first.

```bash
g++ -g main.cpp client.hpp request.hpp response.hpp MPD.hpp server.hpp -o main
```

And then execute the `main` executable file.

```bash
./main
```

## Contributions

You can download and execute the main.exe to see the score for yourself.
To compile I'd suggest VSCode, you can use `ctrl + shift + B`.

## Copyrigth

```
Copyright Luca Crema 2020, all rights reserved.
```
