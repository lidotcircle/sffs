#!/usr/bin/env python3


# Copyright (c) 2023 by Li Dianyuan
#
# This file is provided under the terms of the MIT License:
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
from abc import ABC, abstractmethod

import argparse
import os
import glob
import sys


class DFA(ABC):
    # 0: undetermined; 1: accept; 2: reject
    @abstractmethod
    def state(self):
        pass

    @abstractmethod
    def feed(self, char):
        pass

    @abstractmethod
    def reset(self):
        pass

    @abstractmethod
    def end(self):
        pass


class CharStream:
    def __init__(self, text):
        self.text = text
        self.idx = 0

    def next(self):
        ans = self.text[self.idx]
        self.idx += 1
        return ans

    def eof(self):
        return self.idx == len(self.text)

    def moveback(self):
        self.idx -= 1


class RuleEvaluator:
    def __init__(self):
        self.rules = []

    def addRule(self, sm, cb):
        self.rules.append([len(self.rules), sm, cb, 0])

    def __reset_rule(self):
        for rule in self.rules:
            rule[1].reset()

    def eval(self, stream):
        accumulatedText = ""
        while not stream.eof():
            char = stream.next()
            accumulatedText += char
            for u in self.rules:
                if u[1].state() == 0:
                    u[1].feed(char)
                elif u[1].state() == 1:
                    u[3] = u[3] + 1

            waiting = False
            allDead = True
            for u in self.rules:
                if u[1].state() == 1 and not waiting:
                    for _ in range(u[3]):
                        stream.moveback()
                        accumulatedText = accumulatedText[:-1]
                    u[2](accumulatedText, stream)
                    accumulatedText = ""
                    self.__reset_rule()
                    allDead = False
                    break
                elif u[1].state() == 0:
                    allDead = False
                    waiting = True

            if allDead:
                print("failure")
                break


def char_is_space(char):
    return char == " " or char == "\t"


# ^[[:space:]]*[#][[:space:]]*include
class DFA_include(DFA):
    def __init__(self):
        super().__init__()
        self.sval = 0

    def state(self):
        if self.sval >= 0:
            return 0
        elif self.sval == -1:
            return 1

        return 2

    def feed(self, char):
        if (self.state() != 0):
            return

        if self.sval == 0:
            if char == "#":
                self.sval = 1
            elif not char_is_space(char):
                self.sval = -2
        elif self.sval == 1:
            if char == "i":
                self.sval = 2
            elif not char_is_space(char):
                self.sval = -2
        elif self.sval == 2:
            if char == "n":
                self.sval = 3
            else:
                self.sval = -2
        elif self.sval == 3:
            if char == "c":
                self.sval = 4
            else:
                self.sval = -2
        elif self.sval == 4:
            if char == "l":
                self.sval = 5
            else:
                self.sval = -2
        elif self.sval == 5:
            if char == "u":
                self.sval = 6
            else:
                self.sval = -2
        elif self.sval == 6:
            if char == "d":
                self.sval = 7
            else:
                self.sval = -2
        elif self.sval == 7:
            if char == "e":
                self.sval = -1
            else:
                self.sval = -2

    def reset(self):
        self.sval = 0

    def end(self):
        pass


# ^[[:space:]]*[#][[:space:]]*pragma[[:space:]]*once
class DFA_pragmaonce(DFA):
    def __init__(self):
        super().__init__()
        self.sval = 0

    def state(self):
        if self.sval >= 0:
            return 0
        elif self.sval == -1:
            return 1

        return 2

    def feed(self, char):
        if (self.state() != 0):
            return

        if self.sval == 0:
            if char == "#":
                self.sval = 1
            elif not char_is_space(char):
                self.sval = -2
        elif self.sval == 1:
            if char == "p":
                self.sval = 2
            elif not char_is_space(char):
                self.sval = -2
        elif self.sval == 2:
            if char == "r":
                self.sval = 3
            else:
                self.sval = -2
        elif self.sval == 3:
            if char == "a":
                self.sval = 4
            else:
                self.sval = -2
        elif self.sval == 4:
            if char == "g":
                self.sval = 5
            else:
                self.sval = -2
        elif self.sval == 5:
            if char == "m":
                self.sval = 6
            else:
                self.sval = -2
        elif self.sval == 6:
            if char == "a":
                self.sval = 7
            else:
                self.sval = -2
        elif self.sval == 7:
            if char_is_space(char):
                self.sval = 8
            else:
                self.sval = -2
        elif self.sval == 8:
            if char == "o":
                self.sval = 9
            elif not char_is_space(char):
                self.sval = -2
        elif self.sval == 9:
            if char == "n":
                self.sval = 10
            else:
                self.sval = -2
        elif self.sval == 10:
            if char == "c":
                self.sval = 11
            else:
                self.sval = -2
        elif self.sval == 11:
            if char == "e":
                self.sval = -1
            else:
                self.sval = -2

    def reset(self):
        self.sval = 0

    def end(self):
        pass


# ^.*$
class DFA_singleline(DFA):
    def __init__(self):
        super().__init__()
        self.sval = 0

    def state(self):
        return self.sval

    def feed(self, char):
        if char == '\n' or char == '\r':
            self.sval = 1

    def reset(self):
        self.sval = 0

    def end(self):
        self.sval = 1


class TranslationUnitX:
    def __init__(self, include_file_map):
        self.include_file_map = include_file_map
        self.result = []
        self.included = set()
        self.has_pragmaonce = False

    def log(self, *args, **kwargs):
        print(*args, file=sys.stderr, **kwargs);

    def pushtext(self, newtext):
        self.result.append(newtext)

    def push_pragmaonce(self):
        if not self.has_pragmaonce:
            self.result.append("#pragma once")
            self.has_pragmaonce = True

    def inc_has(self, inc):
        if inc not in self.include_file_map:
            return False

        file = self.include_file_map[inc]
        return file in self.included

    def need_proc(self, inc):
        return inc in self.include_file_map

    def process(self, file):
        self.log(f"INFO: processing {file}")
        self.included.add(file)

    def readfile(self, file):
        return open(file, "r").read()

    def _generate_bundler(self):
        valid_suffix = [ '.h', '.c', '.hpp', '.cpp', '.cx', '.cxx', '.hx', '.hxx', '' ]
        text = "#pragma once\n"
        for key in self.include_file_map:
            if self.include_file_map[key] != key:
                continue
            if os.path.splitext(key)[1] not in valid_suffix:
                continue
            text += f"#include\"{key}\"\n"
        return text

    def eval(self, filename):
        self.result = []
        self.included = set()
        self.has_pragmaonce = False
        real_filename = filename
        text = ""
        if filename is None:
            real_filename = os.path.join(os.path.curdir, "__dummy_filename")
            text = self._generate_bundler()
            self.log(text)
            self.log()
        else:
            text = self.readfile(filename)
        streamTr = StreamTranslator(self, CharStream(text), real_filename)
        streamTr.eval()
        return ''.join(self.result)


class StreamTranslator:
    def __init__(self, tu, stream, filename):
        self.translationUnit = tu
        self.stream = stream
        self.filename = filename
        self.ruleEv = RuleEvaluator()

        def inc_rule(text, _):
            self.__rule_include(text)
        def pgm_rule(text, _):
            self.__rule_pragmaonce(text)
        def sgl_rule(text, _):
            self.__rule_singleline(text)

        self.ruleEv.addRule(DFA_include(),    inc_rule)
        self.ruleEv.addRule(DFA_pragmaonce(), pgm_rule)
        self.ruleEv.addRule(DFA_singleline(), sgl_rule)

    def eval(self):
        self.translationUnit.process(self.filename)
        self.ruleEv.eval(self.stream)

    def __rule_include(self, text):
        mark = " "
        ii = 0
        def abort_inc():
            for _ in range(ii):
                self.stream.moveback()
            self.translationUnit.pushtext(text)

        while not self.stream.eof() and char_is_space(mark):
            mark = self.stream.next()
            ii += 1

        if mark != "\"" and mark != "<":
            abort_inc()
            return

        closed = False
        endmark = ">" if mark == "<" else "\""
        mkt = ""
        while not self.stream.eof():
            char = self.stream.next()
            ii += 1
            if char == endmark:
                closed = True
                break;
            else:
                mkt += char

        if not closed:
            abort_inc()
            return

        if mkt.startswith("./"):
            mkt = os.path.join(os.path.dirname(str(self.filename)), mkt[2:])

        if not self.translationUnit.need_proc(mkt):
            abort_inc()
            return

        if self.translationUnit.inc_has(mkt):
            # TODO purge until next line
            return

        mktfilename = self.translationUnit.include_file_map[mkt]
        mkttext = self.translationUnit.readfile(mktfilename)
        mkteval = StreamTranslator(self.translationUnit, CharStream(mkttext), mktfilename)
        mkteval.eval()

    def __rule_pragmaonce(self, _):
        self.translationUnit.push_pragmaonce()

    def __rule_singleline(self, text):
        self.translationUnit.pushtext(text)


def add_inc2map(incmap, incpath):
    files = glob.glob(incpath + "/**/*", recursive=True)
    for inc in files:
        if not os.path.isfile(inc):
            continue
        pt = os.path.relpath(inc, incpath)
        kk = []
        pt, u = os.path.split(pt)
        kk.append(u)
        while pt != "":
            pt, u = os.path.split(pt)
            kk.insert(0, u)
        incmap[inc] = inc
        incmap["/".join(kk)] = inc


def main():
    description = "Amalgamate C/C++ source and header files by resolving include directives."
    argsparser = argparse.ArgumentParser(description=description)
    
    argsparser.add_argument("-i", "--inc", action="append",
        required=True, metavar="", help="include path (repeatable)")
    
    argsparser.add_argument("-s", "--source",
        required=False, metavar="", help="input file, if not specified bundle all files with include")
    
    argsparser.add_argument("-o", "--output",
        required=False, metavar="", help="output file")
    
    args = argsparser.parse_args()

    include_file_map = {}
    for incpath in args.inc:
        add_inc2map(include_file_map, os.path.abspath(incpath))

    translationUnit = TranslationUnitX(include_file_map)
    output = translationUnit.eval(args.source)
    if args.output is not None:
        with open(args.output, 'w') as f:
            f.write(output)
    else:
        print(output)


if __name__ == "__main__":
    main()

