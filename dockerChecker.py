#!/bin/python3

import sys
import subprocess
import time


def main():
    version = ""
    counter = 0
    home = subprocess.run("echo $HOME", shell=True, check=True, capture_output=True).stdout.decode("utf-8").replace('\n', '')

    with open(f"{home}/.salty_cpp_bot_docker_version", "r", encoding="utf-8") as f:
        version = f.read()

    while True:
        res = subprocess.run(f'docker ps | grep "{version}"', capture_output=True, check=False, shell=True)

        if res.returncode:
            res = subprocess.run(f'docker ps -a | grep "{version}"', capture_output=True, check=False, shell=True)
            res = res.stdout.decode('utf-8')
            _id = res.split()[0]
            subprocess.run(f'docker start {_id}', shell=True, check=True)
            counter += 1
            print(f"Container crashed, restart: {counter}")

        time.sleep(30)


if __name__ == '__main__':
    sys.exit(main())
