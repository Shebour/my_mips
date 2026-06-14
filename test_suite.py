#! /usr/bin/env python3

import json
import subprocess
from colorama import Fore

test_file = "test/test.json"
binary = "build/coverage/my_mips"

with open(test_file, 'r') as f:
    data = json.load(f)
if not data:
    print(f"Failed to read {test_file}")
    exit(2)

passed = 0
total = 0

def check_test(exp_stdout, exp_stderr, got_stdout, got_stderr):
    error_happened = False
    if exp_stdout != got_stdout:
        print(f"Got stdout : {got_stdout}\nExpected stdout: {exp_stdout}")
        error_happened = True
    if exp_stderr != got_stderr:
        print(f"Got stderr : {got_stderr}\nExpected stderr: {exp_stderr}")
        error_happened = True
    if error_happened:
        print(f"{Fore.RED}KO{Fore.RESET}")
    else:
        print(f"{Fore.GREEN}OK{Fore.RESET}")
        
    return not error_happened

def execute_test(test_data):
    test_name       : str           = test_data['name']
    param           : str | None    = test_data['param']
    _input          : str | None    = test_data['input']
    expected_stdout : str | None    = test_data['expected_stdout']
    expected_stderr : str | None    = test_data['expected_stderr']
    flags           : str | None    = test_data.get('flags')

    if expected_stdout and "{binary_name}" in expected_stdout:
        expected_stdout = expected_stdout.replace("{binary_name}", binary)
    if expected_stderr and "{binary_name}" in expected_stderr:
        expected_stderr = expected_stderr.replace("{binary_name}", binary)
    print(f"=== {test_name} ===")
    args = [binary]
    if flags:
        args += flags.split()
    if param is not None:
        args.append(param)
    try:
        proc = subprocess.run(args, capture_output=True, text=True,
                              input=_input, timeout=2)
        exit_status = proc.returncode
        out_proc = proc.stdout
        err_proc = proc.stderr
    except subprocess.TimeoutExpired:
        print(Fore.RED + "Timeout" + Fore.RESET)
        print(f"{Fore.RED}KO{Fore.RESET}")
        return False
    return check_test(expected_stdout, expected_stderr, out_proc, err_proc)
    

for test_data in data["Tests"]:
    total += 1
    if execute_test(test_data):
        passed += 1
print(f"{passed}/{total}")
