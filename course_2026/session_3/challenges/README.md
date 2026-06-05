# Session 3 Challenges

Work inside the AFL++ container:

```bash
docker compose -f docker-compose.pre-session.yml run --rm aflplusplus bash
cd /workspace/challenges
```

Each exercise has a `build.sh` script. It creates `build/` for CMake files and copies the binary back next to the source.

To clean one exercise before rebuilding:

```bash
./build.sh clean
```

To submit a crash, package exactly one file:

```bash
mkdir -p submit
cp path/to/crash submit/input.bin
tar -czf crash.tar.gz -C submit input.bin
```

Upload `crash.tar.gz` to the matching challenge in CTRL-F.

## Exercise 1

Build a non-ASan binary for zzuf:

```bash
cd /workspace/challenges/exercise1
./build.sh
```

Skim `simple_crash.cpp` first. Can you find a crashing input manually just from the conditions?

Try to find a crash manually first :)

Then try zzuf with small stdin seeds:

```bash
cd /workspace/challenges/exercise1
./build.sh
printf '133\n' > seed.txt
zzuf -i -b 1 -s 53 -r 0.01 ./simple_crash < seed.txt
```

To get the input that crashed the binary you can use 

```bash
zzuf -i -b 1 -s 53 -r 0.01 cat < seed.txt > input.bin
```

When you understand the crashing input, write it to `input.bin`, package it, and upload it.

## Exercise 2

Build:

```bash
cd /workspace/challenges/exercise2
./build.sh
```

Skim the code. Suggest a small command string for the starting corpus before running AFL++.

Fuzz stdin:

```bash
afl-fuzz -i corpus -o findings -m none -d -- ./medium
```

Package one file from `findings/default/crashes/` and upload it.

## Exercise 3

Build:

```bash
cd /workspace/challenges/exercise3
./build.sh
```

Skim the code. Suggest a simple stdin value for the starting corpus before running AFL++.

Fuzz stdin:

```bash
afl-fuzz -i corpus -o findings -m none -d -- ./specs-slice
```

Package one file from `findings/default/crashes/` and upload it.

## Exercise 4: FLAC Parser

Build:

```bash
cd /workspace/challenges/exercise4_flac_parser
./build.sh
```

Skim the parser. Suggest a small FLAC-like file seed before running AFL++.

Fuzz file input:

```bash
afl-fuzz -i corpus -o findings -m none -d -- ./flac_parser @@
```

Package one file from `findings/default/crashes/` and upload it.

## Exercise 5: Audio Metadata

Build:

```bash
cd /workspace/challenges/exercise5_audio_meta
./build.sh
```

Skim the format parser. Suggest one valid-looking metadata seed before running AFL++.

A dictionary gives AFL++ useful tokens to inject while mutating inputs. For a binary format, add magic bytes, block types, and common strings from the parser.

Build `audio_meta.dict` from tokens you see in the code. Below is an example: 

```bash
cat > audio_meta.dict <<'EOF'
"IMAF"
EOF
```

Fuzz file input:

```bash
afl-fuzz -i corpus -o findings -x audio_meta.dict -m none -d -- ./audio_meta @@
```

Package one file from `findings/default/crashes/` and upload it.

## Exercise 6: Comment Server

Build:

```bash
cd /workspace/challenges/exercise6_comment_server
./build.sh
```

Skim the command parser. Suggest a short command sequence for the starting corpus before running AFL++.

Fuzz file input:

```bash
afl-fuzz -i corpus -o findings -x comment_server.dict -m none -d -- ./comment_server @@
```

Package one file from `findings/default/crashes/` and upload it.
