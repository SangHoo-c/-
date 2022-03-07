# HDFS 이해

# 하둡 분산파일시스템(HDFS) 이해

[[토크ON세미나] 아파치 하둡 입문 4강 - 하둡 분산파일시스템 이해(2) | T아카데미](https://www.youtube.com/watch?v=41hpU-4ki4M)

[HDFS 이해(1) - 하둡 분산 파일 시스템](https://velog.io/@dnstlr2933/%ED%95%98%EB%91%A1-%EB%B6%84%EC%82%B0%ED%8C%8C%EC%9D%BC-%EC%8B%9C%EC%8A%A4%ED%85%9CHDFS-%EC%9D%B4%ED%95%B4-1)

**하둡**

대용량 데이터를 분산 처리할 수 있는 자바 기반의 오픈 소스 프레임워크 

구글 플랫폼 철학 유지 

하둡은 HDFS (분산시스템) 에 데이터 저장, Map Reduce 를 통해 데이터 처리 

**구글 플랫폼 철학** 

1. 한대의 고가 장비보다 여러 대의 저가 장비가 낫다. 
2. 데이터는 분산 저장한다. 
3. 시스템 (H/W) 는 언제든지 죽을 수 있다. 
4. 시스템 확장이 쉬워야한다. 

`분산 & 자동화`

**하둡 특성**

- 수천대 이상의 리눅스 기반 범용 서버들을 하나의 클러스터로 사용
- master-slave 구조
- 파일은 block 단위로 저장
- block 데이터의 복제본 유지로 인한 신뢰성 보장 (default : 3)
    - 100 mb 저장하면, hadoop 에 300mb 저장
- 데이터 처리의 지역성(locality) 보장
    
    

## **데이터 지역성 Data Locality**

> 파일을 나누어 각각의 서버가 읽어 들이고, 가능하다면 저장되어 있는 노드에서 데이터 처리
> 

구체적으로 설명하자면, **블락을 갖고있는 노드들에 대해서 해당 노드의 TT(Task Tracker)가 자신이 갖고있는 로컬 데이터를 읽어서 처리를 진행**

이쯤에서 다시 보는 하둡 아키택처 
<br/>
<img width="669" alt="Screen Shot 2022-03-07 at 5 06 10 PM" src="https://user-images.githubusercontent.com/59246354/156991753-abc84c8e-92a2-40db-a32c-625ede25ebb6.png">
<br/>

소스가 있는 곳에 데이터를 가져오는것이 아니라, **데이터가 있는 곳에 소스를 보내 처리** 

- 파일을 여러개의 블록으로 나눈 뒤, 각 다른 서버에 저장해 두고 데이터 처리도 각 서버에서 동시에 따로 처리
- 빅데이터 처리가 클라우드 환경에서 성능이 느린 이유는 이 데이터 지역성이 보장되지 않기 때문이다.

**장점** 

- 네트워크를 이용한 데이터 전송 시간 감소
- 대용량 데이터 확인을 위한 디스크 탐색시간 감소
- 적절한 단위의 블록 크기를 이용한 CPU 처리시간 증가

**왜 하둡을 쓰지?** 

- 배치성의 데이터 (고객이 사이트에 머무르는 시간 , 이동경로 ,, )
- 비싼 RDMS 에 저장 대신 HDFS 에 저장
- **(하둡 덕분에) 이제는 더 이상 많은 데이터를 저장하기 위해 큰 비용이 들지 않는다.**
    - 로그 데이터 저장 가능해짐! ⇒ 분석해 볼 수 있다!

```python
# Distributed computing 
- 데이터를 분산, 분산된 데이터를 처리

# Parallel computing 
- 데이터를 공유 storage 에 올리고, core 수를 늘려서 처리
```

### HDFS

- Data is too big to store on one machine! ⇒ store in **multiple** machine!
- 파일을 128 MB 블록으로 나누어 분산된 서버에 저장
    - minimize overhead : disk seek time is almost constant.
        
<img width="522" alt="Screen Shot 2022-03-07 at 5 07 37 PM" src="https://user-images.githubusercontent.com/59246354/156991973-9097f25c-7b5b-4c4c-adbc-898e856f5e0c.png">

- **저사양의** 서버로 스토리지 구성 가능
- 네임노드(마스터), 데이터노드(슬레이브)
- DB 는 아니다..!! (append 만 가능)

```python
# 하둡에서 블록 (block) 하나의 크기가 큰 이유? 

- HDFS 블록은 128 MB 와 같이 매우 큰 단위 
- 블록이 큰 이유는 탐색 비용을 최소화 할 수 있기 때문 
- 하드디스크에서 블록의 시작점을 탐색하는데 걸리는 시간을 줄일 수 있다. 
- 네트워크를 통해 데이터 전송에 더 많은 리소스 할당 가능

결론 : 데이터 처리를 더 빨리 할 수 있다. 
```

**Not good for** 

- lots of small files
- latency

**네임노드** 

- 메타데이터관리 : FS 유지하기 위한 메타데이터 (메모리에서 관리 - **빠르다**)
- 데이터노드 모니터링 : 데이터노드는 네임노드에게 3초마다 하트비트(heartbeat) 전송
- 블록관리 : 장애가 발생한 데이터노드의 블록 새로운 데이터노드에 복제
- maps a **filename** to list of **Block IDs**
- maps a **Block IDs** to DataNodes
- client 는 네임노드에 접근해서 block 의 위치를 찾는다.

**데이터노드**

- maps a **Block ID** to a physical location on disk.
- client 는 block 의 위치를 얻고 데이터노드에 **직접** 접근한다.
    - prevetn NN from being the bottleneck of the cluster.
    - allow HDFS to scale to large number of concurrent clients.
    - spread the data traffic across the cluster.
- client 가 HDFS 에 저장하는 파일을 local disk 에 유지
- 연산량이 늘어난다 ⇒  **scale-out** 가능

특정 노드가 fail 된다면? (on-premise 환경)  


<img width="456" alt="Screen Shot 2022-03-07 at 5 07 56 PM" src="https://user-images.githubusercontent.com/59246354/156992013-b4ad4810-1a02-4340-9b90-a12b0c2de709.png">




**falut tolerence**

- HDFS 는 block 을 copy 한다.
- 각 12 개의 compute 노드에 분산되어 있고, 같은 lack 에 포함되어 있지 않는다.
- HDFS files are divides into blocks (128MB)
- one block are replicated multiple times (default : 3 times)
- locality : 분산되어 저장되어 있으면 `병렬성`이 높아진다.

> 하둡의 데이터 유실은 없나요?
> 

복사된 block 이 있는 node 가 모두 장애가 있는 경우 (3 개의 node 에서 모두 장애가 있다.)

⇒ 즉, 하둡 클러스터에서 데이터 유실은 거의 없다. 

병렬적인 프로그램은 전문가의 역할을 필요로 했다. (lock, semaphore, ...)

잘 못 짜게 되면 심각한 성능저하를 일으킨다. 

H/W 에 대한 fault tolerence 보장의 어려움 

> MapReduce 를 쓰자!!!
>
