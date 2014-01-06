playalong
=========

A simple, yet useful music player for musicians. It simply plays an audio file
(mp3, wav, flac, ogg... whatever libavcodec supports), with the possibility to
have an A-B-replay (i.e. a loop of only a part of the song). Both markers can be
set very fine grained, to make it easy to match an exact bar. (+/- 10 ms).

This is very handy, if you want to play along some part of a song.

Features
--------

* Plays whatever is supported by libavcodec (wav, mp3, ogg, flac...)
* Plays to whatever libao supports (alsa, oss...)
* Very fine grained loop adjusting (+/- 10 ms)
* A very simple ncurses interface

* (although I didn't tested it yet, it should be more or less platform
  agnostic()


Future Features
---------------
* slow down playback (without changing the pitch)
* make key mappings configurable

Keymaps
-------


    Used keys are (think of two arrow crosses):
    ____________________________
    |                          |
    | q   e       u            |
    |   s d f   h j k          |
    |                          |
    |      [space]      <   >  |
    |__________________________|

    q - Quit

    e - delete marker a
    s - move marker a left (-10 ms)
    d - set marker a
    f - move marker a right (+10 ms)

    u - delete marker b
    h - move marker b left (-10 ms)
    j - set marker b
    k - move marker b right (+10 ms)

    [space] - pause
    [left/right arrow] - seek in song

Installation
------------

Requirements:
* libao
* libavcodec
* libavformat
* libavutil
* ncurses
* a C++11 capable compiler (tested with g++ 4.8 and clang++ 3.3)


If you have these libs installed, it's as simple as

    $ make

If you want to install it system-wide do as root

    $ make install
