#!/bin/python3.11

import sys
import subprocess
import time


def main():
    version = sys.argv[1]

    while True:
        res = subprocess.run(f'docker ps | grep "{version}"', capture_output=True, check=False, shell=True)

        if res.returncode:
            res = subprocess.run(f'docker ps -a | grep "{version}"', capture_output=True, check=False, shell=True)
            res = res.stdout.decode('utf-8')
            print(res.split())

        time.sleep(30)


if __name__ == '__main__':
    sys.exit(main())
