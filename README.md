# Moderato

Moderato is an experimental program for solving chess problems.

## Usage

The program reads problems from standard input or a file, and writes the solutions to standard
output.

```
Moderato [inputfile]
```

## EPD-based input

Moderato
accepts [Extended Position Description](https://www.chessprogramming.org/Extended_Position_Description)
records with a single operation: direct mate fullmove count (opcode `dm`) for stipulating mate
search, or analysis count depth (opcode `acd`) for perft.

### Example

```
B1Q5/3p4/1N1b4/5p1K/2nP1k2/8/b3PB2/2q3R1 w - - dm 2;
```

Moderato outputs the requested evaluation:

```
+M2 Ba8-h1
```

## Popeye-based input

The problems and the extent of analysis can be specified by using a subset of the input language
of [Popeye](https://github.com/thomas-maeder/popeye). Moderato accepts the following specifications
and keywords:

- Directives: `BeginProblem`, `EndProblem`, `NextProblem`
- Commands: `Remark`, `Condition`, `Option`, `Stipulation`, `Pieces`
- Conditions: `Circe`, `NoCapture`, `AntiCirce`, `AndernachChess`, `AntiAndernachChess`
- Anti-Circe types: `Calvet`(default), `Cheylan`
- Options: `Try`, `Defence`, `SetPlay`, `NullMoves`, `WhiteToPlay`, `Variation`, `MoveNumbers`,
  `NoThreat`, `EnPassant`, `NoBoard`, `NoShortVariations`, `HalfDuplex`, `NoCastling`
- Stipulation types: direct, help`h`, self`s`
- Goals: mate`#`, stalemate`=`
- Piece types: king`K`, queen`Q`, rook`R`, bishop`B`, knight`S`, pawn`P`, grasshopper`G`,
  nightrider`N`, amazon`AM`
- Piece colours: `White`, `Black`

The French or German variant of the input language can be used as well. The input is not
case-sensitive. Moderato does not accept shortened forms of the keywords.

### Example

```
BeginProblem
Remark Gligor Denkovski, 3rd Pl. Liga problemista 1986
Pieces White Kh5 Qc8 Rg1 Ba8f2 Sb6 Pd4e2 Black Kf4 Qc1 Bd6a2 Sc4 Pd7f5
Stipulation #2
Option Variation Try NoBoard
EndProblem
```

Moderato outputs the requested analysis:

```
1.Ba8-f3? (2.Sb6-d5#)
1...Sc4-a3,Sc4-a5,Sc4-b2,Sc4xb6,Sc4-e5 2.Qc8xc1#
1...Sc4-e3 2.Bf2-g3#
1...Sc4-d2!
1.Ba8-g2? (2.Sb6-d5#)
1...Sc4-a3,Sc4-a5,Sc4-b2,Sc4xb6,Sc4-e5 2.Qc8xc1#
1...Sc4-d2 2.e2-e3#
1...Sc4-e3!
1.Sb6-d5+? Kf4-e4!
1.Qc8-g8? (2.Qg8-g5#,Qg8-g3#)
1...Qc1xg1!
1.e2-e3+? Sc4xe3 2.Bf2-g3#
1...Qc1xe3!
1.Bf2-g3+? Kf4-e3!
1.Rg1-g4+? f5xg4!
1.Ba8-h1! (2.Sb6-d5#)
1...Sc4-a3,Sc4-a5,Sc4-b2,Sc4xb6,Sc4-e5 2.Qc8xc1#
1...Sc4-d2 2.e2-e3#
1...Sc4-e3 2.Bf2-g3#
```

## Author

[Ivan Denkovski](mailto:denkovski@hotmail.com) is the author of Moderato.
