### WSGI 란 ? 

Web Server Gateway Interface. Callable Object 를 통해 Web Server 가 request 에 대한 정보를 Application 에 전달. 

목적 :  to promote web application portability across a variety of web servers.

#### Callable Object

- Function 이나 Object 의 형태
- HTTP Request 에 대한 정보 (Method, URL, Data ,,,)
- Callback 함수 

```python
# environ : HTTP Request 정보를 담고 있는 매개변수
# start_response : Web Server 에게 결과를 돌려주기 위한 콜백 함수. 

def application(environ, start_response):
    body = b'Hello world!\n'
    status = '200 OK'
    headers = [('Content-type', 'text/plain')]
    start_response(status, headers)
    return [body]
```

```python
# Web Application 은 HTTP Request 에 대한 정보를 갖고 Business Logic 을 수행후 
# Callback Function 을 통해 Web Server 에 결과 반환. 

class Application:
    def __init__(self, environ, start_response):
        self.environ = environ
        self.start_response = start_response

    def __iter__(self):
        body = b'Hello world!\n'
        status = '200 OK'
        headers = [('Content-type', 'text/plain')]
        self.start_response(status, headers)	# 다른 함수에서 start_response 를 호출
        yield body
```



#### Web Server 와 Web Application 

Web server : (SW) Client 의 정적인 리소스 요청을 처리하는 프로그램. ex) Apache HTTP Server, Nginx

동적인 요청이 들어오면 Web Application 에게 요청을 위임, Web Application 는 Web Server 에게 로직을 수행한 결과를 다시 돌려준다. 

Web Server 가 Web Application 과 대화할 수 있는 **인터페이스** 필요. 

다양한 종류의 Web Server 와 Web Application 을 interchangable 하게 사용하기 위해서 잘 정의된 **인터페이스**가 필요. 



### WSGI Middleware 

Web Service 에 필요한, Authentication, Routing, Session, Cookie, Error Page,, 

Web Application 의 실행전 과 후에 이러한 기능들을 추가해주는 기능을 한다. **Decorator Pattern**



#### Gunicorn

WSGI Middleware 기능을 가진 라이브러리. 



Ref. https://www.python.org/dev/peps/pep-0333/
