#! /usr/bin/env python3

import yaml
import subprocess
from yaml.loader import SafeLoader
from colorama import Fore

test_file = "test/test.yaml"
binary = "build/bin/my_mips"

with open(test_file, 'r') as f:
    data = yaml.load(f, Loader=SafeLoader)
if not data:
    print(f"Failed to read {test_file}")
    exit(2)


passed = 0
total = 0

for test in data:
    total += 1
    filename = test['file']
    output = test['output']
    print(f"{filename}")
    try:
        proc = subprocess.run([binary, "test/" + filename], capture_output=True,
                              text=True, timeout=2)
        exit_status = proc.returncode
        out_proc = proc.stdout
        err_proc = proc.stderr
    except subprocess.TimeoutExpired:
        print(Fore.RED + "Timeout" + Fore.RESET)
        continue
    if out_proc != output:
        print(f"{Fore.RED}KO{Fore.RESET}: {filename}")
        print(f"Got: {out_proc}\nExpected: {output}")
    else:
        print(f"{Fore.GREEN}OK{Fore.RESET}")
        passed += 1
print(f"{passed}/{total}")

