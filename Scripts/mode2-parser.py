#!/usr/bin/env python
# -*- encoding:utf8 -*-

from __future__ import print_function
import argparse


class Parser(object):

    @classmethod
    def parse(cls, target_file):
        print("Target file : %r" % target_file)

        if target_file is None or len(target_file) == 0:
            return

        with open(target_file, "r") as file:
            raw = file.readlines()

        if raw is None:
            return

        counter = 0
        for line in raw[1:-1]:
            code = line.split(" ")[1].strip()
            if code is None or len(code) == 0:
                continue
            counter += 1
            if counter % 8 == 0:
                print(code)
            else:
                print(code, end=" ")
        print(" ")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', required=True)
    args = parser.parse_args()

    Parser().parse(args.file)
