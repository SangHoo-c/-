"""

Type	-31-                                 format (bits)                                 -0-
R	opcode (6)	rs (5)	rt (5)	rd (5)	shamt (5)	funct (6)
I	opcode (6)	rs (5)	rt (5)	immediate (16)
J	opcode (6)	address (26)


python 파일 읽기
https://wikidocs.net/26

opcode 자료
https://opencores.org/projects/plasma/opcodes

big vs small 엔디안
https://stackoverflow.com/questions/28707615/loading-and-storing-bytes-in-mips/28708786
https://www.geeksforgeeks.org/bit-manipulation-swap-endianness-of-a-number/


1. byte 단위로 입력을 받는다.
2. hex -> bin 으로 변화한다.
3. str 을 list 로 받아서, 의미있는 부분을 parsing 한다,
4. hash-table (명령어 매칭) 해둔 테이블을 기반으로 명령어를 찾는다.

"""

opcode_table = {
    # r-type => goto func_table
    "000000": "R",

    # j-type
    "000010": "j",
    "000011": "jal",

    # i-type
    "000100": "beq",
    "000101": "bne",
    "001000": "addi",
    "001001": "addiu",
    "001010": "slti",
    "001011": "sltiu",
    "001100": "andi",
    "001101": "ori",
    "001110": "xori",
    "001111": "lui",

    # i-type : memory access
    "100000": "lb",
    "100001": "lh",
    "100011": "lw",
    "100100": "lbu",
    "100101": "lhu",
    "101000": "sb",
    "101001": "sh",
    "101011": "sw"
}

func_table = {
    "100000": "add",
    "100001": "addu",
    "100100": "and",
    "011010": "div",
    "011011": "divu",
    "001001": "jalr",
    "001000": "jr",
    "010000": "mfhi",
    "010010": "mflo",
    "010001": "mthi",
    "010011": "mtlo",
    "011000": "mult",
    "011001": "multu",
    "100111": "nor",
    "100101": "or",
    "000000": "sll",
    "000100": "sllv",
    "101010": "slt",
    "101011": "sltu",
    "000011": "sra",
    "000111": "srav",
    "000010": "srl",
    "000110": "srlv",
    "100010": "sub",
    "100011": "subu",
    "001100": "syscall",
    "100110": "xor"
}

import sys
import os


def binary_process():
    tot = []
    tot_hex = []
    _ins = ""
    _idx = 0
    for b in data:
        _tm = bin(b)[2:]
        _len = len(_tm)
        while _len < 8:
            _tm = '0' + _tm
            _len += 1

        _ins += _tm
        _idx += 1
        if _idx % 4 == 0:
            _tmp = ""
            for j in range(8):
                _bin = _ins[4 * j: 4 * j + 4]
                _tmp += hex(int(_bin, 2))[2:]

            tot_hex.append(_tmp)
            tot.append(_ins)
            _ins = ""
    return tot, tot_hex


def r_type_ins(it, code):
    _rs = int(it[6:11], 2)  # _rs = hex(int(it[6:11], 2))[2:]
    _rt = int(it[11:16], 2)
    _rd = int(it[16:21], 2)
    _shamt = int(it[21:26], 2)
    # _shamt = two_complement(it[21:26])
    _funct = ins[26:]

    if code in ["add", "addu", "and", "nor", "or", "slt", "sltu", "sub", "subu", "xor"]:
        print("%s, $%s, $%s, $%s" % (code, _rd, _rs, _rt))
    elif code in ["div", "divu", "mult", "multu"]:
        print("%s, $%s, $%s" % (code, _rs, _rt))
    elif code in ["jr"]:
        print("%s, $31" % code)
    elif code in ["jalr"]:
        print("%s, $%s, $%s" % (code, _rd, _rs))
    elif code in ["mfhi", "mflo"]:
        print("%s, $%s" % (code, _rd))
    elif code in ["mthi", "mtlo"]:
        print("%s, $%s" % (code, _rs))
    elif code in ["sll", "sra", "srl"]:
        print("%s, $%s, $%s, %s" % (code, _rd, _rt, _shamt))
    elif code in ["sllv", "srav", "srlv"]:
        print("%s, $%s, $%s, $%s" % (code, _rd, _rt, _rs))
    elif code in ["syscall"]:
        print("%s" % code)
    else:
        print(" error, input code :  %s---- " % code)


def ji_type_ins(it, code):
    if code in ["j", "jal"]:
        target = int(it[6:], 2)
        print("%s, %s" % (code, target))
        return

    _rs = int(it[6:11], 2)
    _rt = int(it[11:16], 2)
    # _offset = int(it[16:], 2)
    _offset = two_complement(it[16:])

    if code in ["beq", "bne"]:
        print("%s, $%s, $%s, %s" % (code, _rs, _rt, _offset))
    elif code in ["addi", "addiu", "slti", "sltiu", "andi", "ori", "xori"]:
        print("%s, $%s, $%s, %s" % (code, _rt, _rs, _offset))
    elif code in ["lui"]:
        print("%s, $%s, %s" % (code, _rt, _offset))
    elif code in ["lb", "lh", "lw", "lbu", "lhu", "sb", "sh", "sw"]:
        print("%s, $%s, %s($%s)" % (code, _rt, _offset, _rs))
    else:
        print(" error, input code :  %s---- " % code)


def two_complement(b):
    if b[0] == "0":
        return int(b, 2)
    else:
        return -2**15 + int(b[1:], 2)


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print("파일 경로를 입력해주세요.")
    else:
        fname = sys.argv[1]
        _, ext = os.path.splitext(fname)
        if ext != ".bin":
            print(".bin 파일을 입력해주세요. ")
            sys.exit()
        try:
            f = open(fname, 'rb')
        except OSError:
            print("파일을 read / open 할 수 없습니다. ", fname)
            sys.exit()

        with f:
            data = f.read()
            f.close()

    instructions, binary_rep = binary_process()
    for idx in range(len(instructions)):
        ins = instructions[idx]
        op = ins[:6]
        print("inst", idx, ": ", binary_rep[idx], end=" ")

        # print(ins)
        try:
            opcode = opcode_table[op]
        except KeyError:
            print("unknown instruction")
            continue

        if opcode == "R":
            funct = ins[26:]
            try:
                func_code = func_table[funct]
            except KeyError:
                print("unknown instruction")
                continue

            # print(func_code)
            r_type_ins(ins, func_code)
        else:
            # print(opcode)
            ji_type_ins(ins, opcode)
            pass


"""

... shift 연산같은 경우, 최대 길이 입력을 받고, 옮기고 끝은 0 으로 채우는등,,, 구현이 필요
... rotate 함수가 있지만, 이는 회전,,,

4 bytes
00 22 00 20

r-type
0000 0000 / 0010 0010 / 0000 0000 / 0010 0000
000000  00001 00010 00000 00000 100000
opcode  rs    rt    rd   shift  funct
r-type  1     2     0    no-s   add

a(10), b, c, d(13) ,e ,f
8D 42 00 20


i-type
1000  1101  0100  0010   0000   0000   0010   0000
100011  01010   00010   0000    0000    0010    0000
opcode  rs      rt      16 bit number
lw      10      2       32

00 02 10 80
0000    0000    0000    0010    0001    0000    1000    0000
000000  00000   00010   00010   00010   000000
o       rs      rt      rd      s(sa)   f




inst 19: 83d00024 lb $16, 36($30)
19 : 10000011110100000000000000100100
1000 0011 1101 0000 0000 0000 0010 0100

100000  11110   10000   0000 0000 0010 0100
opcode  rs      rt      offset
lb      30      16      36
6       5       5       16

02  80  f8  09
0000    0010   1000  0000   1111    1000    0000    1001
000000  10100   00000   11111   00000   001001
        rs              rd

inst 5: 03e00008 jr $31
0000 0011 1110 0000 0000 0000 0000 1000

000000  11 1110 0000 0000 0000 0000 1000
opcode  address
6       26
000000  1111 1000 0000 0000 0000 0010 00 ...



inst 62: 0c100006 jal 1048582

0000    1100    0001    0000    0000    0000    0000    0110
000011  00000100000000000000000110

a 10
b 11
c 12
d 13 = 8 4 1 = 1101

inst 19: 83d00024 lb $16, 36($30)

1000    0011    1101    0000    0000    0000    0010    0100
100000  11110   10000   00000 00000 100100
opcode  rs      rt      offset


a 10
b 11
c 12

inst 0: 3c1d8000 lui $29, -32768

0011    1100    0001    1101    1000    0000    0000    0000
001111    00000     11101  10000000000000000


8fbf0014 lw $31 -32748($29)
8fbf0014 lw $31, 20($29)


1000   1111    1011    1111    0000    0000    0001    0100

100011  11101   11111   0000 0000 0001 0100



inst 29: 02020018 mult $16, $2


"""
