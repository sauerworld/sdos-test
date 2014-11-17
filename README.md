Sauerbraten Community Edition
===

About ComEd
---

Sauerbraten Community Edition, short ComEd, is a Sauerbraten client providing additional features desired by the community that are not included in the official Sauerbraten release.
In opposition to other projects, ComEd's development is not reserved to a defined group of developers. Instead, it encourages the whole community to contribute.
Contribution can happen in any imaginable way: Be it via the submission of ideas, game content, source code or reporting of bugs - there can never be enough helping hands.


Features
---

The core features of ComEd v0.1 include:

- detailed ammo bar
- chat colors
- (automatic) client-sided demo recording
- eased demo navigation via `demotime`
- extra event handlers, e.g. `onspawn`
- *Flat GUI* for a more modern look
- frag messages placed in the middle of the screen
- placeable game clock
- additional scoreboard columns
- score hud
- in-game team health display
- spectating from the followed player's point of view
- team killing information, e.g. `getlasttkvictim`
- weapon accuracy variables
- weapon trail customization

Most of the options are changeable via a GUI by typing `/comed`.
For a full list of commands, feel free to browse the [ComEd documentation](http://comed.sauerworld.org/documentation/).

As the client is based on pisto's [SDoS Test Client](https://github.com/pisto/sdos-test), it also has all of its SDL2 features, allowing more direct input and an overall smoother gaming expecience.


Installation
---

For installation instructions, read the *INSTALL.md* file.


Compilation
---

The complete source code of ComEd is available via [GitHub](https://github.com/sauerworld/community-edition).

For zipped release builds containing additional packages in pak/, use the included *buildcomed* script.
	./buildcomed win|lin|mac|macX |noclean

If you only want to compile the binaries, a simple `make` should suffice.
For cross compilation, use an adequate PREFIX= argument (check *buildcomed* for possible values).


Contact
---

In order to enhance ComEd, we really need **your** constructive feedback and ideas. You can contact us either via the Sauerworld forums or the GameSurge IRC network.

- [Official ComEd website](http://comed.sauerworld.org)
- [Sauerworld forums](http://sauerworld.org/forum/)
- #sauerworld at irc.gamesurge.net