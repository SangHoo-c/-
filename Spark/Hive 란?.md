# Hive

## HIVE 란?

- Hive 는 metastore 라는 `저장소를` 만들어, 하둡에서 처리된 **meta-data 의 구조**를 metastore 에 저장. (MySQL 등 JDBC 를 지원하는 모든 DB 를 이용하여 metastore 구축 가능)
- FS 에 저장된 데이터에 catalog 와 metasotre 를 제공하여 RDMS, Table, Partition 을 제공하여 데이터 구조화가 가능하도록 한다.
- HIVE 에서 **`테이블` 은 HDFS 상에 저장된 파일과 디렉토리 구조에 대한 메타 정보 라고 할 수도 있다.**
- HDFS, HBase 와 같은 데이터 저장 시스템에 저장되어 있는 대용량 데이터를 분석

### 궁금증

**Q1. Hive 는 DBMS 인가?** 

- It is an RDBMS-like database, but is not 100% RDBMS. 
⇒ [https://logz.io/blog/hive-vs-spark/](https://logz.io/blog/hive-vs-spark/)
- Hive 란, open-source distributed **data warehousing system** which operates on HDFS.
    - 하지만, It differs from a relational database in a way that it stores schema in a database and processed data into HDFS.
    - hive 는 transaction  없다.
    - 즉, 우리가 생각하는 DBMS 와는 조금 다르다.

**Q2. Hive 는 왜 쓰는가?** 

- 하둡 환경에서 복잡한 MR 코드를 SQL 과 유사한 간단한 HiveQL 로 처리 가능하다.
    - 친근한 SQL 인터페이스
    - 또한, 빠른 쿼리를 위해 **인덱스** 기능 포함 (비트맵 인덱스)
- HiveQL 제공, MR 의 모든 기능을 지원
- petabytes of data 를 batch processing 이용해서 빠르게 처리 가능

**Q3. 왜 Spark 에서 Hive 를 쓰는가?**

[Enable Hive Support ](https://www.notion.so/Enable-Hive-Support-b722eda60d3a472b9654fdc2fe677199) 

- Hive metastore 사용

**구성 요소**

- JDBC / ODBC Driver : HIVE 의 쿼리를 다양한 DB 와 연결하기 위한 driver 제공
- CLI
- Query Engine : 입력한 쿼리 분석 → 실행계획 수립 → HiveQL 을 MR 코드로 변환 & 실행
- `MetaStore` : HIVE 에서 사용하는 table 의 스키마 정보 저장 & 관리, 기본적으로 Derby DB 가 사용되나 다른 DBMS (MySQL ... ) 로 변경 가능

[점프 투 파이썬](https://wikidocs.net/23469)

## Hive Python Connection

hive 연결하기 

- pyhive (puretransport
- pyodbc (driver 설치 필요)

 

| pyhive | PyHive is a collection of Python DB-API
 and SQLAlchemy interfaces for Presto and Hive. |
| --- | --- |
| pyodbc | pyodbc is an open source Python module that makes accessing ODBC databases simple. It implements the DB API 2.0
specification but is packed with even more Pythonic convenience. |
| thrift | Python bindings for the Apache Thrift RPC system. 
다양한 언어를 지원하는 RPC 프레임워크. 
다양한 플랫폼 간의 편리하게 사용할 수 있는 통합 RPC 환경을 제공. 
교차언어간에 자연스럽게 통신할 수 있는 RPC 클라이언트 / 서버 로 쉽게 코드 생성가능. 
언어간의 Serialization 가능.  |
| PyHamcrest | PyHamcrest is a framework for writing matcher objects, allowing you to declaratively define “match” rules. (unit-test 에 사용 )  |
| pure-transport | This package provides a thrift_transport for hive connections using PyHive. This is mainly beneficial as an implementation of Pure-SASL for Windows users where SASL is difficult to install. The other use case facilitated is support for SSL Sockets, however it is left for the user to configure the socket. |
| six | It provides utility functions for smoothing over the differences between the Python versions with the goal of writing Python code that is compatible on both Python versions. |
| jupyter  | 웹에서 python 코드를 작성 / 실행 해주는 라이브러리  |
| pandas | 행/열 로 이루어진 데이터 객체 다룰 수 있게 된다.  |
|  |  |
|  |  |

## phive

**ORM 이란?**

object-relational mapping (객체 관계 매핑) 

DB 내의 리소스(테이블) 을 객체화하여, CRUD 와 같이 공통된 접근기법으로 사용할 수 있다. 

- **ORM 장점**
1. DBMS 와 느슨한 결합.  ORM에 대한 이해로 CRUD를 다룰 수 있다. 비즈로직 집중가능 ⇒ **개발 생산성** 
2. 객체를 통하여 대부분의 데이터를 접근 및 수정 ⇒ **코드 가독성**
3. 데이터 구조 변경시, 객체에 대한 변경만 하면 됨  ⇒ **유지보수성**
- **ORM 단점:**
1. 복잡한 쿼리 작성시, ORM 사용에 대한 난이도가 급격히 증가.
2. 호출 방식에 따라, 성능 차이. 
3. DBMS 고유의 기능을 전부 사용하지는 못함.

ex) **SQLAlchemy : python 을 위한 SQL 툴킷 & ORM 이다.** 

**Presto 란?**

**0. PrestoDB 란 무엇인가?**

쿼리엔진, 쿼리엔진이기에 스토리지(저장) 기능은 제공하지 않는다.

**0-1 그럼 왜 이런 서비스가 제공되는가?**

SQL 이 지원되지 않는 서비스에서 데이터 가공 및 조회하기 위해 사용 (ex. mongoDB 에서 SQL 사용하기)

물론 SQL 이 지원되는 서비스에서도 사용이 가능. 

**0-2 그럼 무슨 장점이 있어서 사용하는가?**

바로 이기종 간의 데이터베이스 혹은 파일 간에 데이터 조회 및 가공이 된다.

( 이기종간 서비스에서 select, join, group by 등등 가능 )

**0-3. 단점은?**

단점으로는 분산 서비스들이 다 그렇듯 ... 설정 및 관리가 싱글 서버 대비 어렵다.

**0-4. 믿을만한 서비스인가?**

presto는 페이스북에서 개발 & 운영

[Presto - Hadoop Conference Japan 2014](http://www.slideshare.net/frsyuki/presto-hadoop-conference-japan-2014)

[Facebook Presto presentation](http://pt.slideshare.net/GuorongLIANG/facebook-presto-presentation)

[Using Presto in our Big Data Platform on AWS](http://techblog.netflix.com/2014/10/using-presto-in-our-big-data-platform.html)

## puretransport

**thrift_transport 란?** 

> Thrift 란? 
다양한 언어를 지원하는 RPC 프레임워크 
다양한 플랫폼 간의 편리하게 사용할 수 있는 통합 RPC 환경을 제공
> 

> RPC 란? 
remote procedure call 
별도의 원격제어를 위한 코딩 없이, 
다른 주소 공간의 함수나, 프로시저를 실행할 수 있게 해주는 프로세스간 통신.
> 

> sasl library 란? 
보안...
> 

> Serialization (직렬화) 이란? 
어떤 환경의 데이터 구조를 다른 환경에 전송 / 저장 하기 위해 
재구성 가능한 byte 의 포멧으로 변환하는 과정을 말한다.
> 

**KyroSerializer**

- java serializer 보다 좋은 성능..
- 추가 리서치 필요 ...

**직렬화** 란?

- 객체의 상태를 영속화 하는 매커니즘
- 객체를 다른 환경에 저장했다가 나중에 재구성 할 수 있게 만드는 과정

**직렬화 필요 이유** 

- 참조형 데이터는 병렬적으로 구성되어 있어, 하나의 데이터로 전송 불가
- JS 에선 참조형 데이터를 표준 포멧인 JSON 으로 변환 합니다.
- Node http 통신 과정에서 내부적으로 JSON 객체를 Buffer 객체로 변환하기도 합니다.

 

**직렬화 예시** 

`Object` to `JSON`

```jsx
let obj = {a:1};
let json = JSON.stringify(obj); 

// json = "{"a":1}"

```

`JSON` to `Buffer`

```jsx
let bufer = Buffer.from(json);

// <Buffer 61 31>
```

> **JSON** 
“key-value” 로 이루어진 개방형 표준 포멧. 데이터 object 를 전달하기 위해 
인간이 읽을 수 있는 텍스를 사용
> 

## cursor object 란?

실질적으로 데이터베이스에 SQL문장을 수행하고, 조회된 결과를 가지고 오는 역할을 한다.

- Cursor 란? 
A cursor is an object which helps to execute the query and fetch the records from the database. The cursor plays a very important role in executing the query.
    
    데이터베이스 커서는 일련의 데이터에 순차적으로 액세스할 때 검색 및 "현재 위치"를 포함하는 데이터 요소
    
    데이터베이스에서의 커서 또한 유사한 개념이다. 방대한 양의 데이터에서 특정 위치, 특정 로우(row)를 가리킬때 커서가 사용된다. 위키피디아에서는 커서에 대해 '많은 로우 중 한 로우를 가리키는 포인터와 같다'고 설명하고 있다.
    
    **커서란 현재 작업중인 레코드를 가리키는 오브젝트.**
    

```python
#python cursor_object.py

#import the library
import mysql.connector

# creating connection
conn = mysql.connector.connect(
  host="localhost",
  user="ID",
  password="PW"
)

# import the cursor from the connection (conn)
mycursor = conn.cursor()

mycursor.execute("SELECT * FROM MOVIE")
mycursor.close()
conn.close()
```

## ODBC 란?

open database connectivity 

- 어떤 응용프로그램을 사용하는지에 관계없이
- 데이터베이스를 자유롭게 사용하기 위하여 만든 응용프로그램의 표준방법.
- 응용프로그램과 DBMS 중간에 데이터베이스 처리 프로그램을 두어 이를 가능하게 한다.

> DBMS 와 상관없이 동일한 API 로 통신한다.
> 

**ORM** *object-relational mapping* is literally a layer of software over **JDBC**.

An **ORM library**, such as **[Hibernate](https://hibernate.org/orm/)**, creates **Java classes** to match each **SQL table** and its **columns** become **fields**.

```python
ssh -L 8890:localhost:8890 7218142@pb01gn001
```
