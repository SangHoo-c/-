# 왜 Spark 와 Hive 를 같이 쓸까?

> Spark 에서 Hive 를 사용해서 얻을 수 있는 장점은 뭘까?
> 

> **SparkSQL vs Hive on Spark** ?
> 

[How does Apache Spark and Apache Hive work together?](https://www.quora.com/How-does-Apache-Spark-and-Apache-Hive-work-together)

[Comparing Apache Hive vs. Spark | Logz.io](https://logz.io/blog/hive-vs-spark/)

**Typical HDFS/Hive/Spark set up**

**유저 입장** 

1. HDFS 에 저장된 data 
2. Apache Hive provides a way for you to project schema onto your data on HDFS 
⇒ SQL 쿼리 가능 
3. HDFS 에 있는 data 는 hive 를 통해서 SQL 쿼리로 작업 가능 
⇒ 내부적으로 hive map-reduce job 실행 
4. Execution engine 으로 mr 대신 Apache Spark 사용 
⇒ Same SQL interface, same raw data on HDFS, 
⇒ but use **Spark** instead of Hive MapReduce to do the computation / query execution.

**실행 관점**

1. When SparkSQL uses hive
    
    SparkSQL can use `HiveMetastore` to get the metadata of the data stored in HDFS. This metadata enables SparkSQL to do better optimization of the queries that it executes. Here Spark is the query processor.
    
2. When Hive uses Spark [See the JIRA entry: HIVE-7292](https://issues.apache.org/jira/browse/HIVE-7292)
    
    Here the the data is accessed via spark. And Hive is the Query processor. So we have all the deign features of Spark Core to take advantage of. But this is a Major Improvement for Hive and is still "in progress" as of Feb 2 2016.
    
3. There is a third option to process data with SparkSQL
    
    Use SparkSQL without using Hive. Here SparkSQL does not have access to the metadata from the Hive Metastore. And the queries **run slower**. 
    

I have done some performance tests comparing options 1 and 3. The results are [here](https://hivevssparksql.wordpress.com/).

**가장 깔끔한 정리**

***Hive** is a SQL layer on top of Hadoop. Hive uses a SQL-like HiveQL query language to execute queries over the large volume of data stored in HDFS. HiveQL queries are executed using Hadoop MapReduce, **but Hive can also use other distributed computation engines like Apache Spark and Apache Tez.** Since HiveQL is so SQL and many data analysts know SQL already, Hive has always been a viable choice for data queries with Hadoop for storage (HDFS) and processing (MapReduce).*

<br/>

**SparkSQL vs Hive on Spark 의 결론** 


⇒ 실행관점 1 에 의해서 hive metastore 를 사용하는 경우, 성능향상이 가능하다. 

⇒ hive metastore 를 spark 와 함께 사용하자. 

[Enable Hive Support ](https://github.com/SangHoo-c/lsh-tech-blog/blob/master/Spark/hive%20support%20%5B1%5D%20(enable%20hive%20support).md) ⇒ 해결  good. 

<br/>
<br/>
<br/>

# 번외.

```python
검색 키워드 : enable hive support 왜 하는지?????? 
+) catalog 가 뭔지? 

spark 에 두가지 catalogs (meta store 에 접근 가능한 interface) 

1. hive   (enalbe-hive-support)
2. in-memory  (default)

hive-metastore 에서 정보를 가져올 수 있는 것! 
SparkSQL can use HiveMetastore to get the metadata of the data 
stored in HDFS. 

This metadata enables SparkSQL to do better optimiaztion of the quries 
that is executes. 

Spark 가 query 를 처리하는 engine 역할을 한다. 

```

Spark SQL can read / write data stored in Apache Hive. 

Spark know two **catalogs***
1. hive 
2. in-memory 

`spark.sql.catalogImplementaion` 의 default 는 in-memory 이고 
enableHiveSupport() 를 설정하면, hive 로 설정된다. 

So if you enable hive support, spark.catalog.listTables().show() will show you all tables from the **hive-metastore**.

But this does not mean *hive is used for the **query***, it just means that spark communicates with the **hive-metastore**, the execution engine is always **spark**.

[Spark 2: how does it work when SparkSession enableHiveSupport() is invoked](https://stackoverflow.com/a/52170175)

**이유** 

hive metastore 를 spark 와 함께 사용하는 경우를 확인. ⇒ 성능 향상위해서. 

#### 어떻게 성능 향상이 있을까? 

예상 : hive 의 partition 정보, table 정보 등... bucket ? 에 대한 정보를 spark 에 전달해서 처리 

예상 2 : 만약 hive 의 meta store 정보를 사용하지 않고 hdfs 에 spark 가 직접 접근해서 처리하는 경우, 이러한 partition .... 정보등을 제공받지 못해서 효율 (처리 속도) 이 떨어진다.

=> meta store 이용 방식에 대한, 추가 학습 필요. 


