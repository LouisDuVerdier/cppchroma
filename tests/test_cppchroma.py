import unittest
import subprocess
import os

class UnitTests(unittest.TestCase):

    def run_cppchroma(self, args):
        here = os.path.dirname(os.path.abspath(__file__))
        cmd = [here + "/../build/cppchroma"] + args
        result = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        self.assertEqual(result.stderr, "", f"cppchroma returned errors: {result.stderr}")
        self.assertEqual(result.returncode, 0, f"cppchroma failed: {result.stderr}")
        return result.stdout

    def test_simple_echo(self):
        # Basic echo of "Bad warning!", expect it to be colorized
        args = ["echo", "Bad warning!"]
        expected = "\033[38;2;244;67;54mBad\033[0m \033[38;2;205;220;57mwarning\033[0m!\n"
        self.assertEqual(self.run_cppchroma(args), expected)

    def test_another_case(self):
        # Writing char by char "Bad warning!", expect it to be colorized even if there is some tiny delay
        args = [
            "sh",
            "-c",
            'text="$1"; i=1; len=$(expr length "$text"); while [ $i -le $len ]; do printf "%s" "$(expr substr "$text" $i 1)"; i=$((i+1)); done; printf "\n"',
            "_",
            "Bad warning!"
        ]
        expected = "\033[38;2;244;67;54mBad\033[0m \033[38;2;205;220;57mwarning\033[0m!\n"
        self.assertEqual(self.run_cppchroma(args), expected)

if __name__ == "__main__":
    unittest.main()
