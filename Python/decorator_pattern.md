### Decorator 패턴

* Decorator 는 function 을 인자로 전달 받으며, 그 함수 앞뒤 다른 동작들을 수행해준다. 
* 코드의 재사용성, 가독성을 높이기 위해 사용한다. 
* 프레임워크를 사용할 때 많이 볼 수 있다. 

Using decorators you can wrap objects countless number of times since both target objects and decorators follow the same interface. The resulting object will get a stacking behavior of all wrappers.

1. 데코레이터 함수는 장식을 해줄 func 을 parameter 로 전달 받습니다. 
2. 전달받은 함수는 데코레이터의 장식? 을 받습니다. 

코드의 가독성, 재사용성을 높이기 위해서 사용. 

```python
# class 형 decorator
from datetime import datetime
import time


class Timecost():
    def __init__(self, func):
        self.func = func

    def __call__(self):
        start = datetime.now()
        self.func
        end = datetime.now()
        print('timecost', end - start)
```

```python
@Timecost
def task1():
    time.sleep(3)
    print('task 1.')


@Timecost
def task2():
    time.sleep(3)
    print('task 2.')


@Timecost
def task3():
    time.sleep(3)
    print('task 3.')


if __name__ == '__main__':
    task1()
    task2()
    task3()
```



### Fraomwork 에서는 decorator 를 어떻게 사용하는가? 



1. Flask

```python
# '/' 라는 URL 로 http 요청이 들어오면 'hello world~! ' 를 리턴하는 API 코드
from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'hello world~!'
```

- 사용자 자신이 수행하고자하는 동작을 입력한 뒤에 decorator 를 달아줍니다. 
- Flask 는 해당 URL 로 http request 를 받습니다. 
- 사용자가 정의한 함수를 실행시켜 http response 를 만들어줍니다. 



2. Celery

```python
# Broker 에 연결된 Celery 앱이 브로커 내부의 큐에 Task 가 쌓이는 지 확인하고 Task 가 있으면 처리를 해주는 비동기 작업 수행 
from celery import Celery

# Celery 인스턴스 
app = Celery('tasks', broker='pyamqp://guest@localhost//')

# Task 함수 정의 
@app.task
def add(x, y):
    return x + y
```

```python
1. Celery Worker 실행 
$ celery -A {모듈명} worker --loglevel=info

2. Task 호출 
from tasks import add
add.delay(1,2)
```

Ref. https://jonnung.dev/python/2018/12/22/celery-distributed-task-queue/



**분산 비동기 작업 처리란?**  

분산 메시지 전달, 비동기 작업 큐

1. 대기중인 Job, Worker , Broker
2. Broker 는 전달받은 Job 을 Queue 에 보관
3. 적절한 Worker 는 메시지를 가져가서 Job 을 수행 

=> **Celery** 는 메시지 전달하는 (Publisher) 와 메시지를 가져와 작업을 수행하는 (Worker)를 담당 

=> 무거운 Task : 회원 가입 축하 이메일 발송 등등 

Celery Task 로 정의하고, Broker (RabbitMQ) 와 Consumer (Celery Worker)를 이용해 비동기 처리 



#### Broker

Celery 는 메시지를 작업 으로 주고 받기 때문에 Broker 역할을 하는 시스템이 필요 

=> RabbitMQ : AMQP (advanced message queuing protocol)
