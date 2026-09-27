# Third-party notices

Chess Trainer uses **Stockfish 18**, Windows x86-64-avx2, as a separate local chess-engine executable communicating through the UCI protocol.

## Stockfish 18 identity

- Project: Stockfish
- Release tag: `sf_18`
- Upstream release date: 31 January 2026
- Packaged executable SHA-256: `c86215fa1977d53b82ed854540a4c7b025be4cd042276c85ba3de53fb9118911`
- Official Windows x86-64-avx2 archive SHA-256: `6f6c272ebd6ea594377715235c8a7326f75940ef4f4f856f45106028fe6ae900`

Release verification checks that the packaged `stockfish.exe` is byte-identical to the executable extracted from the official Stockfish 18 Windows x86-64-avx2 release archive.

## Licence and corresponding source

Stockfish is free software licensed under the **GNU General Public License version 3 (GPLv3)**.

The repository includes the GPLv3 licence text at:

- [licenses/Stockfish-GPL-3.0.txt](licenses/Stockfish-GPL-3.0.txt)

Corresponding-source identification and the exact upstream source location for the packaged Stockfish release are recorded at:

- [licenses/Stockfish-Source-Info.txt](licenses/Stockfish-Source-Info.txt)

Upstream source at the exact release tag:

- https://github.com/official-stockfish/Stockfish/tree/sf_18

Chess Trainer does not modify the Stockfish executable. Stockfish remains subject to its own copyright and GPLv3 terms.

This notice records the distribution material and source location used by the project; it is not legal advice.
