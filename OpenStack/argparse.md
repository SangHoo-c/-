CLI 라이브러리를 분석 도중, argparse 를 발견했다. 실행 옵션은 많은 코드에서 볼 수 있었을 것이다. 

CLI 를 이용해 openstack 을 작동시킬때 `openstack 파일명 옵션들...`으로 실행시키기 때문에, argparse에 대한 이해는 필요하다.

argparse는 python에 기본으로 내장되어 있다.

```python
import argparse
import os
```

`import os`는 output directory를 만드는 등의 역할을 위해 필요하다.

argparse를 쓰려면 기본적으로 다음 코드가 필요하다.

```python
import argparse

parser = argparse.ArgumentParser(description='Argparse Tutorial')
# argument는 원하는 만큼 추가한다.
parser.add_argument('--print-number', type=int, 
                    help='an integer for printing repeatably')

parser.add_argument('--foo', '-f') # optional
parser.add_argument('bar')         # positional

args = parser.parse_args()

for i in range(args.print_number):
    print('print number {}'.format(i+1))
print('args.foo:', args.foo)
print('args.bar:', args.bar)
```

1. [ArgumentParser](https://docs.python.org/3/library/argparse.html?highlight=argparse#argparse.ArgumentParser)에 원하는 description을 입력하여 parser 객체를 생성한다. description 외에도 usage, default value 등을 지정할 수 있다.
2. 그리고 `add_argument()` method를 통해 원하는 만큼 인자 종류를 추가한다.
3. `parse_args()` method로 명령창에서 주어진 인자를 파싱한다.
4. `args`라는 이름으로 파싱을 성공했다면 `args.parameter` 형태로 주어진 인자 값을 받아 사용할 수 있다.

```bash
# -h, --help 는 내장된 옵션이다. 
(venv2) hoo@sangui-MacBookPro pythonProject % python main.py -h
usage: main.py [-h] [--print-number PRINT_NUMBER]

Argparse Tutorial

optional arguments:
  -h, --help            show this help message and exit
  --print-number PRINT_NUMBER
                        an integer for printing repeatably
(venv2) hoo@sangui-MacBookPro pythonProject % python main.py --print-number 5
print number 1
print number 2
print number 3
print number 4
print number 5
```



**Positional & Optional**

```python
parser.add_argument('--foo', '-f') # optional
parser.add_argument('bar')         # positional
```

positional 인자와 optional 인자가 있다. 인자의 이름 앞에 `-`가 붙어 있으면 optional, 아니면 positional 인자로서 필수로 지정해야 한다.

```bash
(venv2) hoo@sangui-MacBookPro pythonProject % python main.py --print-number 5 --foo 10 1
print number 1
print number 2
print number 3
print number 4
print number 5
args.foo: 10
args.bar: 1
```

Ref. https://greeksharifa.github.io/references/2019/02/12/argparse-usage/

