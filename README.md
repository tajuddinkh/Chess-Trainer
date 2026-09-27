# Chess Trainer V6.0.0

Chess Trainer is a native Windows x64 chess training application built in C++ with the Win32 API. It uses Stockfish 18 locally through UCI for analysis and training assistance.

## Current release

V6.0.0 is the current production release.

The release corrects the final two issues identified during Windows acceptance testing:
- Player Profiles, Improvement trends card bottom-border geometry.
- Running application, title-bar and taskbar icon handling.

No intentional chess-rule, training-mode, rating, profile, history or persistence behaviour changes were introduced by these corrections.

## Download

Use the [latest GitHub Release](https://github.com/tajuddinkh/Chess-Trainer/releases/latest) for the current production binaries.

Release assets:

- [Chess_Trainer.exe](https://github.com/tajuddinkh/Chess-Trainer/releases/latest/download/Chess_Trainer.exe), standalone application executable.
- [Chess_Trainer_V6_0_0_Portable.zip](https://github.com/tajuddinkh/Chess-Trainer/releases/latest/download/Chess_Trainer_V6_0_0_Portable.zip), portable package including Stockfish 18 and the required licence notices.
- [SHA256.txt](https://github.com/tajuddinkh/Chess-Trainer/releases/latest/download/SHA256.txt), authoritative release checksums.

> [!WARNING]
> **Fair-play and anti-cheating notice**
>
> Chess Trainer includes Stockfish-powered training and assisted-play features capable of producing very strong, engine-guided move suggestions. Using those features during a live game against another person, whether online or over-the-board, can provide a substantial competitive advantage and may violate the rules of the chess platform, club, tournament or event.
>
> Online chess services may use anti-cheating systems to identify engine-assisted play. Misuse of Chess Trainer during a live game may therefore lead to game sanctions, account restrictions, suspension or closure, and may be treated as cheating.
>
> Use the assisted features for study, local practice, post-game learning, or any other situation in which engine assistance is explicitly permitted. You are responsible for complying with the rules that apply to the game you are playing.

## Training modes

Chess Trainer provides five distinct operating modes, each designed for a different style of learning or practice.

| Mode | How it works |
| --- | --- |
| **Full Trainer** | Provides continuous Stockfish-guided training, with strong move suggestions and corrective assistance throughout the game. It is the most direct coaching mode for users who want active guidance while they play. |
| **Assisted Mode** | Keeps the player in control and encourages independent play. The trainer can step in when a serious mistake is detected, while optional tools such as Ask Trainer, Recovery Coach, Emergency Rescue and Full Assist provide different levels of support when needed. |
| **Human Practice** | Creates an offline practice game against the local Stockfish engine at an adjustable approximate playing strength. It is intended for realistic solo practice without requiring an online opponent. |
| **Fair Play / Record Only** | Records the game without requesting live engine moves or live move guidance. This mode is intended for clean game recording followed by later review and analysis. Users should still follow the rules of any platform or event when running companion software during a live game. |
| **Coach Mode** | Reviews the learner's proposed move before the computer replies, then presents a short teaching-oriented explanation of the likely reply and a useful follow-up plan. It is designed to encourage calculation and understanding rather than simply supplying a move. |

## Main features

- Five dedicated training and practice modes
- Player Profiles with rating, career and improvement statistics
- Local game history and saved-game archive
- Trainer Review and Game Insights
- Move-quality and training-dependency tracking
- Local Stockfish 18 analysis
- Portable local-data design

## Platform

- Windows 10/11 x64
- Native Win32/GDI application
- No installer required for the portable release

## Build from source

The production source and resources are in the `src` directory.

From an x64 Visual Studio Developer Command Prompt, change to `src` and run `Build_Local_MSVC.bat`.

## Offline operation

Chess Trainer is designed to operate locally. Final V6.0.0 release validation observed no TCP/UDP endpoints owned by Chess Trainer or Stockfish during the tested startup, profile, gameplay and shutdown sequence. This is validation evidence for the tested release, not a general network-security certification.

## Release integrity

Authoritative release hashes are recorded in [SHA256.txt](SHA256.txt).

## Stockfish

The portable package includes the unmodified official **Stockfish 18 Windows x86-64-avx2** executable.

Stockfish is licensed under GNU GPLv3. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and the [licenses](licenses/) directory.

Chess Trainer communicates with Stockfish as a separate local UCI process.

## Project source licence

No separate open-source licence has been granted for the Chess Trainer application source in this repository. The Stockfish component remains subject to its own GPLv3 terms.

## Author

Designed and developed by Tajud Din.
