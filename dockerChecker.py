#!/bin/python3.11

import sys
import subprocess
import time


def main():
    version = ""
    with open("~/.salty_cpp_bot_docker_version", "r", encoding="utf-8") as f:
        version = f.read()

    while True:
        res = subprocess.run(f'docker ps | grep "{version}"', capture_output=True, check=False, shell=True)

        if res.returncode:
            res = subprocess.run(f'docker ps -a | grep "{version}"', capture_output=True, check=False, shell=True)
            res = res.stdout.decode('utf-8')
            _id = res.split()[0]
            subprocess.run(f'docker start {_id}', shell=True, check=True)

        time.sleep(30)


if __name__ == '__main__':
    sys.exit(main())
