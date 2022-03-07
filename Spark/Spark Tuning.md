# Spark Tuning

## partition

> **Partition 이란?** 
 RDDs나 Dataset를 구성하고 있는 최소 단위 객체
> 

Spark에서는 하나의 최소 연산을 Task라고 표현
하나의 Task에서 하나의 Partition이 처리됩니다. 또한, 하나의 Task는 하나의 Core가 연산 처리합니다.

즉, **1 Core = 1 Task = 1 Partition** 

<br/>

**default partition** 

spark.default.paraellism : 기본 200 

RDD의 데이터는 클러스터를 구성하는 여러 서버(노드)에 나누어 저장

> 이때, 나누어진 데이터를 **`파티션`**이라는 단위로 관리
> 

HDFS를 사용하는 경우에는 기본적으로 HDFS 블록과 파티션이 1:1으로 구성

스파크 API (DataFrame, DataSet)를 사용하면 파티션의 크기와 수를 쉽게 조정 가능

<br/>

**ref.** 

[https://m.blog.naver.com/syung1104/221103154997](https://m.blog.naver.com/syung1104/221103154997)

<br/>

**shuffle partition** 

관련 설정 : spark.sql.shuffle.partitions

`Spark 성능`에 가장 크게 영향을 미치는 Partition으로, J**oin, groupBy** 등의 연산을 수행할 때 Shuffle Partition이 쓰입니다.

shuffle size  = shuffle read + shuffle write 

> Shuffling means the `reallocation` of data between multiple Spark stages.
> 


<br/>

**Shuffle Write**

- the sum of all **written serialized data** on all executors before transmitting (normally at the end of a stage)
- Bytes and records written to disk in order to be read by a shuffle in a future stage

<br/>
<br/>

**Shuffle Read**

- the sum of **read serialized data** on all executors at the beginning of a stage.
- Total shuffle bytes and records read (includes both data read locally and data read from remote executors

[Web UI 읽는 법](https://spark.apache.org/docs/3.0.0-preview2/web-ui.html)

- 25GB X 5 Cores X 50 instances (Core 당 **5 GB** 메모리)
- default partition = 1500

|  | 실험 1  | 실험2 | 실험3 | 실험4 |
| --- | --- | --- | --- | --- |
| shuffle partitions | 1500 | 1000 | 600 | 200 |
| 소요시간  | 79 min | 89 min | 97 min | 103 min |
| shuffle size  | 21 GB | 22 GB | 23 GB  | 25 GB |
| shuffle size / partition  | 14 MB  | 22 MB  | 40 MB | 125 MB  |
| shuffle spill  | 0 | 0 | 0 | 0 |

실험 결과 

- core 당 **메모리**가 충분해서, shuffle spill 이 일어나지 않는다.
- shuffle partition 의 크기를 매우 낮출 수 있고, 이는 성능의 향상으로 이어진다.
- 카카오 성능 실험 (core 당 **2 GB** 의 메모리 사용)

[Spark Shuffle Partition과 최적화](https://tech.kakao.com/2021/10/08/spark-shuffle-partition/)


<br/>
<br/>

**output partition** 

관련 설정 : df.repartition(cnt), df.coalesce(cnt)

> Output Partition은 파일을 저장할 때 생성하는 Partition
> 

<br/>
<br/>
<br/>

## **스파크 메모리 구조**

Spark에서의 메모리 사용 : 2개(**Execution, Storage**)

- Execution 메모리는 Shuffle, Join, Sort, Aggregation에서 사용되는 메모리
- Storage 메모리는 **Caching** 이나 클러스터 전반에 내부 데이터를 전파에 사용되는 메모리


<img width="529" alt="Screen Shot 2022-03-07 at 5 21 51 PM" src="https://user-images.githubusercontent.com/59246354/156994078-bf6025ee-5f42-4b3a-b900-0a42cd28774c.png">

<br/>

**JVM Memory Management** 

- storage : 기본 60%

[https://dhkdn9192.github.io/apache-spark/spark_executor_memory_structure/](https://dhkdn9192.github.io/apache-spark/spark_executor_memory_structure/)

Executor 메모리 설정이 다음 표와 같을 경우, **각 메모리 영역**에 할당되는 실제 사이즈 계산

| conf | value |
| --- | --- |
| spark.executor.memory | 25 G |
| spark.executor.memoryOverhead | 3 G |
| spark.dynamic.Allocation.enabled | false |
| spark.memory.storageFraction | 0.1 |
| spark.memory.fraction | 0.8 |
- Reserved Memory : **300MB**
- User Memory : (25 * 1024MB - 300MB) * (1 - 0.8) = 5,060 MB
- Spark Memory : (25 * 1024MB - 300MB) * 0.8 = 20,320MB
- Storage Memory : (25 * 1024MB - 300MB) * 0.8 * 0.1 = 2,024MB
- Execution Memory : (25 * 1024MB - 300MB) * 0.8 * (1 - 0.1) = 18,216MB

<br/>
<br/>

**memory 설정** 

관련 설정 : spark.executor.memoryOverhead

- Spark의 memoryOverhead 설정을 off-heap용 메모리 공간을 임의로 지정할 수 있다.

```scala
private[yarn] val resource = Resource.newInstance(executorMemory + memoryOverhead, executorCores)
```

> **메모리가 부족한 경우에 `-executor-memory`를 늘려야할까? memoryOverhead를 늘려야할까?**
> 
- `-executor-memory`를 늘려야하는 경우
    - GC가 자주 발생하는 경우
    - 이때는 on-heap이 부족하다는 이야기이다
- memoryOverhead를 늘려야하는 경우
    - GC는 적지만, yarn에 의해 executor가 죽는 경우
    - off-heap 설정 변경 해준다.
    

[[데이터야놀자2021] 머신러닝 유니버스를 여행하는 히치하이커를 위한 데이터 팀이 AWS 위에서 Kubernetes 를 활용하는 법 - 박훈님](https://www.youtube.com/watch?v=7E6oHVQIiug)

- 8:00 ~  : pyspark 사용시 offheap 메모리 설정

<br/>
<br/>

## 번외

Locality level Summary 

- PROCESS LOCAL : 같은 JVM 안에서 처리 되었다.
- JVM 이란? SPARK? ⇒ 메모리 구조!?
