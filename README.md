# DASH Client policy simulation

![Università degli studi di Padova](https://upload.wikimedia.org/wikipedia/it/thumb/5/53/Logo_Universit%C3%A0_Padova.svg/1200px-Logo_Universit%C3%A0_Padova.svg.png)

The repoistory contains code and a PDF report of an homework for Computer Network UNIPD classes.

## Homework
The homework consists in simulating the playout of a media using a DASH-like protocol: the server gives the client a Media Presentation Descriptor that contains a collection of segments and for each one of them has 5 encoding levels that differ for bit size and quality. The client has to use a policy to select the encoding level for each segment that maximize the quality function.
The quality function keeps track of: quality of the segment, difference of quality between segments and delays.
The second and more important part of the homework is designing the most performing policy.

## Contribution
You can download and execute the main.exe to see the score for yourself.
To compile I'd suggest VSCode, you can use `ctrl + shift + B`.
Pull requests are NOT welcome at the moment as this is an evalued homework.

## Copyrigth
```
Copyright Luca Crema 2020, all rights reserved.
```