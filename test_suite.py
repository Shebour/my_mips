#! /usr/bin/env python3

import json
import subprocess
from colorama import Fore

test_file = "test/test.json"
binary = "build/bin/my_mips"

with open(test_file, 'r') as f:
    data = json.load(f)
if not data:
    print(f"Failed to read {test_file}")
    exit(2)

passed = 0
total = 0

def execute_test(test_data):
    param = test_data['param']
    expected : str = test_data['expected']
    if "{binary_name}" in expected:
        expected = expected.replace("{binary_name}", binary)
    print(f"=== {test_data['name']} ===")
    try:
        if param == None:
            proc = subprocess.run([binary], capture_output=True, text=True, timeout=2)
        else:
            proc = subprocess.run([binary, "test/" + param], capture_output=True,
                                text=True, timeout=2)
        exit_status = proc.returncode
        out_proc = proc.stdout
        err_proc = proc.stderr
    except subprocess.TimeoutExpired:
        print(Fore.RED + "Timeout" + Fore.RESET)
        return False
    if out_proc != expected and err_proc != expected:
        print(f"{Fore.RED}KO{Fore.RESET}: {param}")
        print(f"Got: {out_proc}\nExpected: {expected}")
        return False
    else:
        print(f"{Fore.GREEN}OK{Fore.RESET}")
        return True
    

for test_data in data["Tests"]:
    total += 1
    if execute_test(test_data):
        passed += 1
print(f"{passed}/{total}")
