
## Enable hive support 에 대하여.

```python
# https://spark-korea.github.io/docs/sql-data-sources-hive-tables.html
# warehouse_location은 hive 데이터베이스와 테이블의 기본 위치를 지정합니다.
warehouse_location = abspath('spark-warehouse')

spark = SparkSession \
    .builder \
    .appName("Python Spark SQL Hive integration example") \
    .config("spark.sql.warehouse.dir", warehouse_location) \
    .enableHiveSupport() \
    .getOrCreate()
```

spark 코드 작성중, hive table 을 사용하기 위해 `.enableHiveSupport()` 라는 구문을 넣곤 합니다. 

이는 어떤 것을 의미하는 걸까요? 

Hive 에서 쿼리를 작성하는 것은 HDFS 에 존재하는 파일을 읽는 것입니다. 

```python
# Hive
SELECT *
FROM vcrm_7218142.test
```

Hive 는 위와 같은 쿼리로 vcrm_7218142.test 의 위치에서 파일을 읽어 결과를 내놓습니다. 

hdfs 에 대한 정보가 전혀 없는데, 어떻게 가능할까요? 

이건 바로 hive-metastore 에서 저장된 정보를 활용하기 때문입니다. 

`/etc/hive/3.0.1.0-187/0/hive-site.xml` 


<img width="728" alt="Screen Shot 2022-03-07 at 5 11 26 PM" src="https://user-images.githubusercontent.com/59246354/156992521-8cb0137c-baff-4683-a221-f9db4cc5d8e2.png">




“thrift://pb01mn001. ...” 에 hive metastore 가 저장되어 있음을 확인할 수 있습니다. 

해당 경로에 hive table 의  meta-data (location, partition 정보) 가 저장되어 있는거죠. 

그렇기 때문에, spark.sql 을 이용하여 쿼리를 실행해도, 해당 테이블의 경로를 찾아서 해당하는 파일을 읽어올 수 있습니다. 

따라서 아래 두 방법은 같은 동작을 수행합니다.


```python
df1 = spark.sql("""
SELECT * 
FROM vcrm_7218142.test
""")

df2 = spark.read.parquet("/user/hive/warehouse/vcrm_7218142.db/test") 

```




