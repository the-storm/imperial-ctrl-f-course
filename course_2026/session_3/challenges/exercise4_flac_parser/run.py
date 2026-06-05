#!/usr/local/bin/python
import subprocess
import tempfile
import sys

if __name__ == '__main__':
    try:
        size = int(input('Size: '))

        print("File: ", end='', flush=True)
        flac = b''
        for i in range(size):
            flac += sys.stdin.buffer.read(1)

        with tempfile.NamedTemporaryFile('wb') as f:
            f.write(flac)
            f.flush()

            res = subprocess.check_output(["./parser", f.name],
                                          stdin=subprocess.PIPE,
                                          stderr=subprocess.PIPE)

            print("=== FLAC Info ===")
            print(res.decode())

    except Exception as e:
        print("[-] Something went wrong")
